#ifndef QUEUE_H
#define QUEUE_H

#include <queue>//队列
#include <condition_variable>//条件变量(线程同步)
#include <mutex>//锁

template <typename T>//声明模板
//该类是模板类,在创建对象的时候需要指定T的类型
//比如:  Queue<std::string *> queue_;
//这就创建了一个对字符串类型进行操作的队列
class Queue
{
public:
	Queue(){}
	~Queue(){}


	int Pop(T &val, const int timeout = 0){//取出队列的值
		//&不会复制内容,只是传递而已
		std::unique_lock<std::mutex> lock(mutex_);
		if(true == queue_.empty()){
			//如果队列为空,等待Push或者超时
				//超时之前都是出于堵塞状态
			cond_.wait_for(lock, std::chrono::milliseconds(timeout), [this]{
				return (false == queue_.empty()) | (true == abord_);
				//要么队列呗push,要么队列弹出
			});
		}
		if(true == abord_){
			//如果弹出
			return -1;
		}
		if(true == queue_.empty()){
			//如果依然为空
			return -2;
		}

		val = queue_.front();//取值
		queue_.pop();//删除

		return 0;
	}

	int Front(T val){//查看队列最前端的值
		std::lock_guard<std::mutex> lock(mutex_);

		if(true == abord_){
			return -1;//队列跳出状态
		}
		if(true == queue_.empty()){
			//如果依然为空
			return -2;
		}

		val = queue_.front();
		return 0;
	}

	int Push(T val){//添加内容
		std::lock_guard<std::mutex> lock(mutex_);

		if(true == abord_){
			return -1;//队列跳出状态
		}

		queue_.push(val);
		cond_.notify_one();

		return 0;
	}

	int Size(){
		std::lock_guard<std::mutex> lock(mutex_);
		return (int)queue_.size();
	}

	void Abort(){//跳出队列
		abord_ = true;
		cond_.notify_all();//激活所有线程
	}
private:
	bool abord_ = false;//队列跳出状态
	std::queue<T> queue_;//队列本体
	std::mutex mutex_;//锁
	std::condition_variable cond_;//条件变量
};

#endif // QUEUE_H
