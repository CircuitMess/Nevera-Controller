#ifndef NEVERA_FEED_H
#define NEVERA_FEED_H

#include <Entity/AsyncEntity.h>
#include "UDPListener.h"
#include "Util/RingBuffer.h"
#include <FeedFrame.h>
#include <JPEGDEC.h>
#include <semaphore>
#include <atomic>

class Feed : public Object {
	GENERATED_BODY(Feed, Object)

	typedef uint16_t Color;
public:
	Feed();
	~Feed() override;

	bool nextFrame(std::function<void(const FeedFrame& info, const Color* img)>);

	void setPostProcCallback(std::function<void(const FeedFrame&, Color*)> callback);

private:
	JPEGDEC jpeg{};
	StrongObjectPtr<UDPListener> udp;

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
	struct {
		FeedFrame info;
		int imgIndex = -1;
	} readyFrame;

	constexpr static size_t JpgMaxSize = 8000; //upper size limit for JPG quality 30 on 160x120 resolution
	constexpr static size_t RxBufSize = 3 * (sizeof(FeedFrame) + JpgMaxSize);

	std::function<void(const FeedFrame&, Color* frame)> postProcCallback;

	static std::unique_ptr<FeedFrame> deserializeFrame(RingBuffer& buf, size_t size);
};


#endif //NEVERA_FEED_H
