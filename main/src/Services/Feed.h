#ifndef NEVERA_FEED_H
#define NEVERA_FEED_H

#include <Entity/AsyncEntity.h>
#include "UDPListener.h"
#include "Util/RingBuffer.h"
#include <FeedFrame.h>
#include <JPEGDEC.h>
#include <semaphore>
#include <atomic>

// TODO we need a Feed for jpg and one for raw img data

class Feed : public Object {
	GENERATED_BODY(Feed, Object)

	typedef uint16_t Color;
public:
	Feed();
	~Feed() override;

	bool nextFrame(std::function<void(const Color* img)>);

private:
	JPEGDEC jpeg{};

	StrongObjectPtr<RingBuffer> rxBuf;
	std::mutex rxMut;

	std::binary_semaphore dataAvailable;

	StrongObjectPtr<Threaded> readTask;
	std::vector<uint8_t> readBuf;
	static constexpr size_t ReadBufSize = 4096;
	void readLoop();

	StrongObjectPtr<Threaded> decodeTask;
	void decodeLoop();
	bool findFrame();

	bool freeImgs[3] = { true, true, true };
	std::vector<Color> frameImgs[3];

	std::mutex readyFrameMut;
	int readyFrame = -1;

	constexpr static size_t JpgMaxSize = 8000; //upper size limit for JPG quality 30 on 160x120 resolution
	constexpr static size_t RxBufSize = 3 * (sizeof(FeedFrame) + JpgMaxSize);

	static std::unique_ptr<FeedFrame> deserializeFrame(RingBuffer& buf, size_t size);
};


#endif //NEVERA_FEED_H
