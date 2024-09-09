#ifndef THREAD_H
#define THREAD_H

#include <thread>

class Thread
{
public:
	Thread(){}
	~Thread(){
		if(thread_){
			Thread::Stop();
		}
	}

	int Start(){}
	int Stop(){
		abort_ = true;
		if(thread_ && thread_->joinable()){
			thread_->join();
			delete thread_;
			thread_ = NULL;
		}
		return 0;
	}

	virtual void Run() = 0;

protected:
	bool abort_ = false;
	std::thread *thread_ = NULL;
};

#endif // THREAD_H
