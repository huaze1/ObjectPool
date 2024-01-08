#ifndef OBJECTPOOL4_HPP
#define OBJECTPOOL4_HPP

#include <list>
#include <memory>
#include <iostream>
#include <mutex>
#include <condition_variable>
using namespace std;

template <class TObject>
class ObjectPool
{
private:
    std::list<TObject *> idlepool;
    size_t maxTotal; // 8 
    size_t maxIdle;  // 6
    size_t totalObject = 0;
    size_t maxWaitTime = 2; ///10;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    void release(TObject *ptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex); 
        if(idlepool.size() < maxIdle)
        {
            idlepool.push_back(ptr);
        }
        else
        {
            --totalObject;
            delete ptr;
            cout<<"空闲池已满, 直接删除object"<<endl;
        }
        m_cv.notify_all();
    }
public:
    ObjectPool(size_t maxtotal = 8, size_t maxidle = 6)
        :maxTotal(maxtotal),maxIdle(maxidle) 
    {}
    ~ObjectPool()
    {
        clear();
    }
    ObjectPool(const ObjectPool &) = delete; // C++11
    ObjectPool &operator=(const ObjectPool &) = delete;

public:
    template <class... Args>
    std::shared_ptr<TObject> acquire(Args &&...args)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(idlepool.empty() && totalObject >= maxTotal)
        {
           // m_cv.wait(lock);
           if(std::cv_status::timeout == 
               m_cv.wait_for(lock,std::chrono::milliseconds(maxWaitTime*1000)))
           {
               cout<<"timeout wait ... "<<endl;
               return std::shared_ptr<TObject>(nullptr);
           }
        }
        if (totalObject < maxTotal && idlepool.empty())
        {
            ++totalObject;
            return std::shared_ptr<TObject>(
                new TObject(std::forward<Args>(args)...),
                [&](TObject *p) -> void
                {
                    release(p);
                });
        }
        else if(!idlepool.empty())
        {
            auto ptr = idlepool.front();
            idlepool.pop_front();
            return std::shared_ptr<TObject>(
                ptr,
                [&](TObject *p) -> void
                {
                    release(p);
                });
        }
        //return std::shared_ptr<TObject>(nullptr);
    }
    size_t getIdleObjectNum() const
    {
        return idlepool.size();
    }
    size_t getTotalObjectNum() const 
    {
        return totalObject;
    }
    size_t getActivateObejctNum() const
    {
        return totalObject - idlepool.size();
    }
    void clear()
    {
        for (auto &pobj : idlepool)
        {
            delete pobj;
            pobj = nullptr;
        }
        idlepool.clear();
    }
};
#endif 