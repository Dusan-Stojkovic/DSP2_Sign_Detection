#ifndef MESSAGEQUEUE_HPP
#define MESSAGEQUEUE_HPP

#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "opencv2/core/core.hpp"

#define MAX_MESSAGE_PARAMS 8
#define MAX_MESSAGES 1024

enum MQWait
{
	NONE,
	PROCESSED,
	RECEIVED
};

class Message
{
public:
	int id;

	Message() { }
	Message(int type, const int w, cv::Mat* data) : id(type), wait((MQWait)w), image(data) { }

	void setImage(cv::Mat* img) { image = img; }
	cv::Mat getImage() { return *image; }

private:
	MQWait wait;
	cv::Mat* image;
};

class MessageQueue
{
public:
	MessageQueue();
	void enable(bool set);
	bool getNextMessage(Message* msg, bool waitForMessage);
	void postMessage(Message* msg);
	~MessageQueue();

private:
	void sleepUntilMessage();

	volatile int Head;
	volatile int Tail;
	volatile bool isEnabled;
	volatile bool isPosted;
	volatile bool isProcessed;
	volatile bool isReceived;

	std::mutex m;
	std::condition_variable posted;
	std::condition_variable received;
	std::condition_variable processed;

	Message messages[MAX_MESSAGES];
	MQWait wait;
};

#endif
