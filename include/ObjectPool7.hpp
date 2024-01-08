#ifndef KeyedObjectPool7_HPP
#define KeyedObjectPool7_HPP

#include "Timestamp.hpp"
#include "Timer.hpp"
#include "TimerMapThread.hpp"

#include <list>
#include <map>
#include <unordered_map>

#include <memory>
#include <iostream>
#include <mutex>
#include <condition_variable>
using namespace std;

template <class TKey, class TObject>
class KeyedObjectPool
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
    //std::list<PoolObject *> idlepool;
    std::map<TKey,std::list<PoolObject*> > idlepool;
    //std::unordered_map<TKey,std::list<PoolObject*> > idlepool;
    //std::multimap<TKey,PoolObject *> idlepool;
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
    size_t getIdleTotal() const
    {
        size_t sum = 0;
        for(auto &obj: idlepool)
        {
            sum += obj.second.size();
        }
        return sum;
    }
    void release(const TKey &key,PoolObject *ptr)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        size_t sum = getIdleTotal();
        borrpool.remove(ptr);
        if (ptr->tag == 3)
        {
            cout << "放逐对象 delete " << endl;
            delete ptr;
        }else if (sum  < maxIdle)
        {
            // idlepool.push_back(ptr);
            ptr->last_used = Timestamp::Now();
            ptr->tag = 1;
            idlepool[key].push_back(ptr);
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
        //clog<<"Object::evictionLoop..."<<endl;
        //std::map<TKey,std::list<PoolObject*> > idlepool;
        std::unique_lock<std::mutex> lock(m_mutex);
        size_t sum = getIdleTotal();
        if(sum <= minIdle) 
        {
            return ;
        }
        int num = sum - minIdle;
        num = num > 3 ? 3 : num; // 
        auto it = idlepool.begin();
        for (; it != idlepool.end();)
        {
            auto p = it->second.begin();
            bool del = false;
            for(; p != it->second.end(); )
            {
                auto idleTime = Timestamp::Now() - (*p)->last_used;
                if (idleTime >= maxIdleTime)
                {
                    cout << "驱逐对象 delete ..." << endl;
                    delete (*p);
                    --totalObject;
                    it->second.erase(p++);
                    del = true;
                    break;
                }
                else
                {
                    ++p;
                }
            }
            #ifdef DELTREENODE
            if(del)
            {
                if(it->second.empty())
                {
                    idlepool.erase(it++);
                }
                else 
                {
                    ++it;
                }
                if(--num == 0)
                {              
                    break;
                }
            }
            #else
            if(del && --num == 0)
            {
                break;
            }
            ++it;
            #endif 
        }
    }

public:
    KeyedObjectPool(size_t maxtotal = 8,
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
        tv.set_timer(bind(&KeyedObjectPool::evictionLoop, this), 4000);
        ta.init();
        ta.set_timer(bind(&KeyedObjectPool::abandonedLoop, this), 6000);

        timermap.init(2000);
        timermap.add_timer(tv);
        timermap.add_timer(ta);
    }
    ~KeyedObjectPool()
    {
        clear();
    }
    KeyedObjectPool(const KeyedObjectPool &) = delete; // C++11
    KeyedObjectPool &operator=(const KeyedObjectPool &) = delete;

public:
    template <class... Args>
    std::shared_ptr<PoolObject> acquire(const TKey &key,Args &&...args)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (idlepool[key].empty() && totalObject >= maxTotal)
        {
            // m_cv.wait(lock);
            if (std::cv_status::timeout ==
                m_cv.wait_for(lock, std::chrono::milliseconds(maxWaitTime * 1000)))
            {
                cout << "timeout wait ... " << endl;
                return std::shared_ptr<PoolObject>(nullptr);
            }
        }
        if (totalObject < maxTotal && idlepool[key].empty())
        {
            ++totalObject;
            auto ptr = std::shared_ptr<PoolObject>(
                new PoolObject(std::forward<Args>(args)...),
                [&](PoolObject *p) -> void
                {
                    release(key,p);
                });
            borrpool.push_back(ptr.get());
            return ptr;
        }
        else if (!idlepool[key].empty())
        {
            //std::map<TKey,std::list<PoolObject*> > idlepool;
            auto ptr = idlepool[key].front();
            idlepool[key].pop_front();
            ptr->borrowed_at = Timestamp::Now();
            ptr->tag = 2;
            borrpool.push_back(ptr);
            return std::shared_ptr<PoolObject>(
                ptr,
                [&](PoolObject *p) -> void
                {
                    release(key,p);
                });
        }
        return std::shared_ptr<PoolObject>(nullptr);
    }
    size_t getIdleObjectNum() const
    {
        return getIdleTotal();
    }
    size_t getTotalObjectNum() const
    {
        return totalObject;
    }
    size_t getActivateObejctNum() const
    {
        return totalObject - getIdleObjectNum();
    }
    size_t getKeyedIdleNum(const TKey &key) const
    {
        return idlepool.at(key).size();
    }
    void clear()
    {
        for (auto &obj : idlepool)
        {
            for(auto &p : obj.second)
            {
                delete p;
                p=nullptr;
            }
            obj.second.clear();
        }
        idlepool.clear();
    }
};
#endif