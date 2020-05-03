#include <vector>
#include <initializer_list>
#include <cmath>

#include "tools/ReadWriteLockGuard.h"

using std::vector;
using std::initializer_list;
using std::copy;
using std::move;

int change2pow2(size_t size)
{
    size_t n = 0;
    while (size)
    {
        ++n;
        size /= 2;
    }

    return pow(2, n);
}

template <typename T>
class vector_thread
{
public:
    //默认初始化
    vector_thread()
        : m_vector() {}
    
    //列表初始化
    vector_thread(const initializer_list<T>& il)
    {
        changeSize(il.size());     

        copy(il.begin(), il.end(), back_inserter(m_vector));

        /*
        for (auto i : il)
        {
            m_vector.push_back(i);
        }
        */
    }

    //拷贝初始化
    vector_thread(vector_thread& rhs)
    {
        ReadWriteLockGuard rwlg(rhs.lock, lockType::read);
        
        changeSize(rhs.size());
        copy(rhs.begin(), rhs.end(), back_inserter(m_vector));
    }

    //移动初始化
    vector_thread(vector_thread&& rhs)
    {
        ReadWriteLockGuard rwlg(rhs.lock, lockType::write);
        changeSize(rhs.size());

        move(rhs.begin(), rhs.end(), back_inserter(m_vector));
    }

    //拷贝赋值
    vector_thread& operator=(vector_thread& rhs)
    {
        ReadWriteLockGuard rwlg(rhs.lock, lockType::read);
        ReadWriteLockGuard rwlgself(lock, lockType::write);

        m_vector.resize(rhs.size());
        copy(rhs.begin(), rhs.end(), begin());

        return *this;
    }

    //移动赋值
    vector_thread& operator=(vector_thread&& rhs)
    {
        ReadWriteLockGuard rwlg(rhs.lock, lockType::write);
        ReadWriteLockGuard rwlgself(lock, lockType::write);

        m_vector.resize(rhs.size());
        move(rhs.begin(), rhs.end(), begin());

        return *this;
    }

    //列表赋值
    vector_thread& operator=(initializer_list<T>& il)
    {
        ReadWriteLockGuard rwlgself(lock, lockType::write);

        changeSize(il.size());
        move(il.begin(), il.end(), back_inserter(m_vector));

        return *this;
    }

    //析构函数
    ~vector_thread()
    {
        ReadWriteLockGuard rwlg(lock, lockType::write);
        m_vector.~vector();
    }

    typename vector<T>::iterator begin() { return m_vector.begin(); }
    typename vector<T>::iterator end() { return m_vector.end(); }
    typename vector<T>::size_type size() { return m_vector.size(); }
    typename vector<T>::reference front() { return m_vector.front(); }
    typename vector<T>::reference back() { return m_vector.back(); }

private:
    void changeSize(size_t size) 
    { 
        size_t result = change2pow2(size);
        m_vector.reserve(result); 
    }

    vector<T> m_vector;
    ReadWriteLock lock;
};
