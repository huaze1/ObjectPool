#ifndef OBJECTPOOL6_HPP
#define OBJECTPOOL6_HPP

#include "Timestamp.hpp"
#include "Timer.hpp"
#include "TimerMapThread.hpp"

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
    struct PoolObject : public TObject
    {
        Timestamp last_used;
        Timestamp borrowed_at;
        int tag; // 1: idle , 2: active , 3: aban;
    public:
        template <class... Args>
        PoolObject(Args &&...args)
            : TObject(std::forward<Args>(args)...),
              borrowed_at(Timestamp::Now()),
              tag(2)
        {
        }
    };

private:
    // std::list<TObject *> idlepool;
    std::list<PoolObject *> idlepool;
    std::list<PoolObject *> borrpool;
    size_t maxTotal;        // 8
    size_t maxIdle;         // 6
    size_t minIdle;         // 2
    size_t maxIdleTime;     // 60s;
    size_t maxBorrTime = 6; // 60s
    size_t totalObject = 0;
    size_t maxWaitTime = 2; /// 10;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    TimerMap timermap;

    void release(PoolObject *ptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        borrpool.remove(ptr);
        if (ptr->tag == 3)
        {
            cout << "放逐对象 delete " << endl;
            delete ptr;
        }else if (idlepool.size() < maxIdle)
        {
            // idlepool.push_back(ptr);
            ptr->last_used = Timestamp::Now();
            ptr->tag = 1;
            idlepool.push_back(ptr);
        }
        else
        {
            --totalObject;
            delete ptr;
            cout << "空闲池已满, 直接删除object" << endl;
        }
        m_cv.notify_all();
    }
    void abandonedLoop()
    {
        // std::list<PoolObject * > borrpool;
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!borrpool.empty())
        {
            auto it = borrpool.begin();
            for (; it != borrpool.end(); ++it)
            {
                auto borrTime = Timestamp::Now() - (*it)->borrowed_at;
                if ((*it)->tag == 2 && borrTime >= maxBorrTime)
                {
                    cout << "放逐obj tag 3..." << endl;
                    (*it)->tag = 3;
                    --totalObject;
                }
            }
        }
        m_cv.notify_all();
    }
    void evictionLoop()
    {
        clog<<"Object::evictionLoop..."<<endl;
        // std::list<PoolObject *> idlepool;
        std::unique_lock<std::mutex> lock(m_mutex);
        clog<<"idlepool.size: "<<idlepool.size()<<" minIdle: "<<minIdle<<endl;
        if (idlepool.size() <= minIdle)
        {
            clog<<" Object::evictionLoop return "<<endl;
            return;
        }
        int num = idlepool.size() - minIdle;
        num = num > 3 ? 3 : num;
        clog<<"evictionLoop num: "<<num<<endl;
        auto it = idlepool.begin();
        for (; it != idlepool.end();)
        {
            auto idleTime = Timestamp::Now() - (*it)->last_used;
            if (idleTime >= maxIdleTime)
            {
                cout << "驱逐对象 delete ..." << endl;
                delete (*it);
                --totalObject;
                idlepool.erase(it++);
                if (--num == 0)
                {
                    break;
                }
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
        : maxTotal(maxtotal),
          maxIdle(maxidle),
          minIdle(minidle),
          maxIdleTime(maxidletime),
          maxBorrTime(6)
    {
        Timer tv, ta;
        tv.init();
        tv.set_timer(bind(&ObjectPool::evictionLoop, this), 4000);
        ta.init();
        ta.set_timer(bind(&ObjectPool::abandonedLoop, this), 6000);

        timermap.init(2000);
        timermap.add_timer(tv);
        timermap.add_timer(ta);
    }
    ~ObjectPool()
    {
        clear();
    }
    ObjectPool(const ObjectPool &) = delete; // C++11
    ObjectPool &operator=(const ObjectPool &) = delete;

public:
    template <class... Args>
    std::shared_ptr<PoolObject> acquire(Args &&...args)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (idlepool.empty() && totalObject >= maxTotal)
        {
            // m_cv.wait(lock);
            if (std::cv_status::timeout ==
                m_cv.wait_for(lock, std::chrono::milliseconds(maxWaitTime * 1000)))
            {
                cout << "timeout wait ... " << endl;
                return std::shared_ptr<PoolObject>(nullptr);
            }
        }
        if (totalObject < maxTotal && idlepool.empty())
        {
            ++totalObject;
            auto ptr = std::shared_ptr<PoolObject>(
                new PoolObject(std::forward<Args>(args)...),
                [&](PoolObject *p) -> void
                {
                    release(p);
                });
            borrpool.push_back(ptr.get());
            return ptr;
        }
        else if (!idlepool.empty())
        {
            auto ptr = idlepool.front();
            idlepool.pop_front();
            ptr->borrowed_at = Timestamp::Now();
            ptr->tag = 2;
            borrpool.push_back(ptr);
            return std::shared_ptr<PoolObject>(
                ptr,
                [&](PoolObject *p) -> void
                {
                    release(p);
                });
        }
        return std::shared_ptr<PoolObject>(nullptr);
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