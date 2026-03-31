#include "Feed.h"
#include "Util/stdafx.h"
#include "Memory/ObjectMemory.h"
#include <esp_log.h>
#include <mutex>
#include <utility>
#include <Core/Application.h>

static const char* tag = "Feed";

Feed::Feed() : dataAvailable(0){
	rxBuf = newObject<RingBuffer>(this, RxBufSize);

	readBuf.resize(ReadBufSize);
	for(auto& frameImg: frameImgs){
		frameImg.resize(128 * 128);
	}

	readTask = newObject<Threaded>(this, [this](){ readLoop(); }, "FeedRead", 40, 4096, 8, 0);

	decodeTask = newObject<Threaded>(this, [this](){ decodeLoop(); }, "FeedDecode", 50, 4096, 8, 0);
}

Feed::~Feed(){
	readTask->stop();
	decodeTask->stop(0);
	dataAvailable.release();
	while(decodeTask->running()){
		delayMillis(1);
	}
}

bool Feed::nextFrame(std::function<void(const Color* img)> cb){
	std::unique_lock lock(readyFrameMut);
	if(readyFrame == -1) return false;

	volatile int frameIndex = readyFrame;
	if(frameIndex == -1) return false;

	volatile const Color* img = frameImgs[frameIndex].data();

	readyFrame = -1;
	lock.unlock();

	cb((const Color*) img);

	freeImgs[frameIndex] = true;

	return true;
}

void Feed::readLoop(){
	if(!working){
		return;
	}

	Application* app = getApp();
	if(app == nullptr) {
		return;
	}

	const UDPListener* udp = app->getService<UDPListener>();
	if(udp == nullptr) {
		return;
	}

	const int64_t bytes = udp->read(readBuf.data(), readBuf.size());

	if(bytes <= 0){
		return;
	}

	std::lock_guard lock(rxMut);
	rxBuf->write(readBuf.data(), bytes);
	dataAvailable.release();
}

void Feed::decodeLoop(){
	if(!working){
		return;
	}

	dataAvailable.acquire();

	std::unique_lock lock(rxMut);
	if(!findFrame()) {
		return;
	}

	rxBuf->skip(sizeof(FeedFrame::Header));
	size_t size;
	rxBuf->read(reinterpret_cast<uint8_t*>(&size), sizeof(size_t));
	rxBuf->skip(sizeof(size_t));

	size_t available = rxBuf->readAvailable();
	auto frame = deserializeFrame(*rxBuf.get(), size);

	size_t readTotal = available - rxBuf->readAvailable();
	rxBuf->skip(size - readTotal); // skip frame if deserialize exited early

	rxBuf->skip(sizeof(FeedFrame::Trailer));

	lock.unlock();

	if(!frame || frame->size == 0 || frame->data == nullptr){
		return;
	}

	int freeImg = -1;
	for(int i = 0; i < 3; i++){
		if(freeImgs[i]){
			freeImg = i;
			break;
		}
	}

	if(freeImg == -1) {
		return;
	}

	freeImgs[freeImg] = false;
	auto imgBuf = frameImgs[freeImg].data();

	jpeg.openRAM((uint8_t*) (frame->data), frame->size, [](JPEGDRAW* data) -> int{
		for(int y = data->y, iy = 0; y < data->y + data->iHeight; y++, iy++){
			size_t offset = y * 128 + data->x;
			size_t ioffset = iy * data->iWidth;
			memcpy((uint8_t*) data->pUser + offset * 2, (uint8_t*) data->pPixels + ioffset * 2, data->iWidth * 2);
		}
		return 1;
	});

	jpeg.setUserPointer(imgBuf);
	jpeg.setPixelType(RGB565_BIG_ENDIAN);

	if(jpeg.decode(0, 0, 0) == 0){
		ESP_LOGE(tag, "decode error: %d", jpeg.getLastError());
		freeImgs[freeImg] = true;
		return;
	}

	std::lock_guard frameLock(readyFrameMut);
	readyFrame = freeImg;
}

bool Feed::findFrame(){
	if(rxBuf->readAvailable() < sizeof(FeedFrame::Header)) return false;

	// Search for frame header
	size_t bytesRead = 0;
	size_t bytesMatched = 0;
	for(; bytesRead < rxBuf->readAvailable(); bytesRead++){
		uint8_t byte = *rxBuf->peek<uint8_t>(bytesRead);

		if(byte == FeedFrame::Header[bytesMatched]){
			bytesMatched++;
			if(bytesMatched == sizeof(FeedFrame::Header)){
				bytesRead++;
				break;
			}
		}else{
			bytesMatched = 0;
		}
	}

	// Clear buffer if header isn't found
	if(bytesMatched != sizeof(FeedFrame::Header)){
		size_t size = rxBuf->readAvailable();
		rxBuf->clear();
		ESP_LOGD(tag, "Couldn't find frame header. Skipping %zu bytes", size);
		return false;
	}

	// Skip bytes preceding header
	if(bytesRead - bytesMatched != 0){
		rxBuf->skip(bytesRead - bytesMatched);
		ESP_LOGD(tag, "Found header. Skipping %zu bytes", bytesRead - bytesMatched);
	}

	// Needs at least 8 more bytes to continue (frame size + frame size confirmation)
	if(rxBuf->readAvailable() < sizeof(FeedFrame::Header) + sizeof(size_t) * 2){
		return false;
	}

	// Read frame size
	uint8_t frameSizeRaw[4];
	for(int i = 0; i < 4; i++){
		frameSizeRaw[i] = *rxBuf->peek<uint8_t>(sizeof(FeedFrame::Header) + i);
	}
	size_t frameSize = *((size_t*) frameSizeRaw);


	// Clear buffer if frame is malformed
	if(frameSize >= RxBufSize){
		ESP_LOGD(tag, "Frame size huge: %zu", frameSize);
		rxBuf->clear();
		return false;
	}

	// Read shifted frame size
	uint8_t frameShiftedSizeRaw[4];
	for(int i = 0; i < 4; i++){
		frameShiftedSizeRaw[i] = *rxBuf->peek<uint8_t>(sizeof(FeedFrame::Header) + sizeof(size_t) + i);
	}

	// Clear buffer if shifted size doesn't match size when shifted
	for(int i = 0; i < 4; i++){
		if(frameShiftedSizeRaw[FeedFrame::SizeShift[i]] != frameSizeRaw[i]){
			ESP_LOGD(tag, "Frame checksum doesn't match");
			rxBuf->skip(sizeof(FeedFrame::Header));
			return false;
		}
	}

	// Abort if rest of frame is missing
	if(rxBuf->readAvailable() < frameSize + sizeof(FeedFrame::Header) + sizeof(FeedFrame::Trailer) + sizeof(size_t) * 2){
		return false;
	}

	// Search for frame trailer
	size_t endOffset = frameSize + sizeof(FeedFrame::Header) + sizeof(size_t) * 2;
	for(bytesMatched = 0; bytesMatched < sizeof(FeedFrame::Trailer); bytesMatched++){
		uint8_t byte = *rxBuf->peek<uint8_t>(endOffset + bytesMatched);
		if(byte != FeedFrame::Trailer[bytesMatched]) break;
	}

	// Clear whole frame if trailer isn't found at expected offset
	if(bytesMatched != sizeof(FeedFrame::Trailer)){
		rxBuf->skip(endOffset + sizeof(FeedFrame::Trailer));
		ESP_LOGD(tag, "Trailer missmatch. Clearing %zu bytes", endOffset + sizeof(FeedFrame::Trailer));
		return false;
	}

	return true;
}

std::unique_ptr<FeedFrame> Feed::deserializeFrame(RingBuffer& buf, size_t size){
	auto frame = std::make_unique<FeedFrame>();
	frame->size = size;

	if(buf.readAvailable() < frame->size){
		ESP_LOGE(tag, "Deserialize data too short, lacks JPG frame (%d, expected %d)", buf.readAvailable(), frame->size);
		return nullptr;
	}

	frame->data = malloc(frame->size);

	if(frame->data == nullptr){
		ESP_LOGE(tag, "Couldn't allocate buffer for jpg frame data");
		return nullptr;
	}

	buf.read((uint8_t*) frame->data, frame->size);

	return frame;
}
