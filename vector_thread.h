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
    using iterator = typename vector<T>::iterator;
    using size_type = typename vector<T>::size_type;
    using reference = typename vector<T>::reference;
    
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

    void push_back(const T& value);
    void push_back(T&& value);
    void emplace_back(T&& value);
    iterator insert(iterator pos, const T& value);
    iterator emplace(iterator pos, T&& value);
    void pop_back();
    iterator erase(iterator start, iterator end);
    iterator erase(iterator pos);

    iterator begin() { return m_vector.begin(); }
    iterator end() { return m_vector.end(); }
    size_type size() { return m_vector.size(); }
    reference front() { return m_vector.front(); }
    reference back() { return m_vector.back(); }

private:
    void changeSize(size_t size) 
    { 
        size_t result = change2pow2(size);
        m_vector.reserve(result); 
    }

    vector<T> m_vector;
    ReadWriteLock lock;
};


template <typename T>
void vector_thread<T>::push_back(const T& value)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.push_back(value);
}

template <typename T>
void vector_thread<T>::push_back(T&& value)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.push_back(std::forward(value));
}

template <typename T>
void vector_thread<T>::emplace_back(T&& value)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.emplace_back(std::forward(value));
}

template <typename T>
typename vector_thread<T>::iterator vector_thread<T>::insert(typename vector_thread<T>::iterator pos, const T& value)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    return m_vector.insert(pos, value);
}

template <typename T>
typename vector_thread<T>::iterator vector_thread<T>::emplace(typename vector_thread<T>::iterator pos, T&& value)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    return m_vector.emplace(pos, value);
}

template <typename T>
void vector_thread<T>::pop_back()
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.pop_back();
}

template <typename T>
typename vector_thread<T>::iterator vector_thread<T>::erase(typename vector_thread<T>::iterator start, typename vector_thread<T>::iterator end)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.erase(start, end);
}

template <typename T>
typename vector_thread<T>::iterator vector_thread<T>::erase(typename vector_thread<T>::iterator pos)
{
    ReadWriteLockGuard rwlg(lock, lockType::write);
    m_vector.erase(pos);
}
