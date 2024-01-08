#ifndef OBJECTPOOL5_HPP
#define OBJECTPOOL5_HPP

#include"Timestamp.hpp"
#include"Timer.hpp"
#include"TimerMapThread.hpp"

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
    struct PoolObject
    {
        TObject *object;
        Timestamp last_used;
    public:
        PoolObject(TObject *p,const Timestamp &now)
        :object(p),last_used(now)
        {

        }

    };
private:
    //std::list<TObject *> idlepool;
    std::list<PoolObject> idlepool;
    size_t maxTotal; // 8 
    size_t maxIdle;  // 6
    size_t minIdle;  // 2
    size_t maxIdleTime; // 60s;
    size_t totalObject = 0;
    size_t maxWaitTime = 2; ///10;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    TimerMap timermap;


    void release(TObject *ptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex); 
        if(idlepool.size() < maxIdle)
        {
            //idlepool.push_back(ptr);
            idlepool.push_back({ptr,Timestamp::Now()});
        }
        else
        {
            --totalObject;
            delete ptr;
            cout<<"空闲池已满, 直接删除object"<<endl;
        }
        m_cv.notify_all();
    }
    void evictionLoop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(idlepool.size() <= minIdle) return ;
        int num = idlepool.size() - minIdle;
        num = num > 3 ? 3: num;
        auto it = idlepool.begin();
        for(; it != idlepool.end();)
        {
            auto idleTime = Timestamp::Now() - it->last_used;
            if(idleTime >= maxIdleTime)
            {
                cout<<"驱逐对象..."<<endl;
                delete it->object;
                --totalObject;
                idlepool.erase(it++);
                if(--num == 0) break;
            }
            else
            {
                ++it;
            }
        }
    }
public:
    ObjectPool(size_t maxtotal = 8, 
               size_t maxidle = 6,
               size_t minidle = 2,
               size_t maxidletime = 60)
        :maxTotal(maxtotal),
        maxIdle(maxidle),
        minIdle(minidle),
        maxIdleTime(maxidletime)
    {
        Timer tv;
        tv.init();
        tv.set_timer(bind(&ObjectPool::evictionLoop,this),4000);
        timermap.init(2000);
        timermap.add_timer(tv);

    }
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
            auto ptr = idlepool.front().object;
            idlepool.pop_front();
            return std::shared_ptr<TObject>(
                ptr,
                [&](TObject *p) -> void
                {
                    release(p);
                });
        }
        return std::shared_ptr<TObject>(nullptr);
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
            delete pobj.object;
            pobj.object = nullptr;
        }
        idlepool.clear();
    }
};
#endif 