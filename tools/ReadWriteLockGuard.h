#include "ReadWriteLock.h"

enum class lockType
{
    read,
    write
};

class ReadWriteLockGuard
{
public:
    ReadWriteLockGuard(ReadWriteLock& lock, lockType type)
        : m_lock(lock)
    {
        if (type == lockType::read)
        {
            m_lock.read_lock();
        }
        else if (type == lockType::write)
        {
            m_lock.write_lock();
        }
    }

    ~ReadWriteLockGuard()
    {
        m_lock.unlock();
    }

    //删除不使用的默认函数
    ReadWriteLockGuard() = delete;
    ReadWriteLockGuard(const ReadWriteLockGuard&) = delete;
    ReadWriteLockGuard& operator=(const ReadWriteLockGuard&) = delete;
    ReadWriteLockGuard(ReadWriteLockGuard&&) = delete;
    ReadWriteLockGuard& operator=(ReadWriteLockGuard&&) = delete;

private:
    ReadWriteLock& m_lock;
};