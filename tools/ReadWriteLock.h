  
#include <atomic>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using std::unique_lock;
using std::mutex;
using std::condition_variable;

class ReadWriteLock
{
public:
    ReadWriteLock() 
        : writeCount(0), readCount(0) { }
    
    void read_lock();
    void write_lock();
   
    void unlock();

private:
    void read_unlock();
    void write_unlock();

    int writeCount;
    int readCount;
    mutex mt;
    condition_variable cv;
};

void ReadWriteLock::read_lock()
{
    unique_lock<mutex> loc(mt);
    cv.wait(loc, [this] { return writeCount == 0;});
    ++readCount;
    //cout << "read lock." << endl;
}

void ReadWriteLock::read_unlock()
{
    //unique_lock<mutex> loc(mt);
    //cout << "read unlock." << endl;
    --readCount;
    cv.notify_all();
}

void ReadWriteLock::write_lock()
{
    unique_lock<mutex> loc(mt);
    cv.wait(loc, [this] { return writeCount == 0;});
    ++writeCount;
    cv.wait(loc, [this] { return readCount == 0;});
    //cout << "write lock." << endl;
}

void ReadWriteLock::write_unlock()
{
    //unique_lock<mutex> loc(mt);
    --writeCount;
    //cout << "write unlock." << endl;
    cv.notify_all();
}

void ReadWriteLock::unlock()
{
    unique_lock<mutex> loc(mt);
    if (writeCount)
    {
        write_unlock();
    }
    else if (readCount)
    {
        read_unlock();
    }
}
