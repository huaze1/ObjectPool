#ifndef OBJECTPOOL3_HPP
#define OBJECTPOOL3_HPP

#include <list>
#include <memory>
#include <iostream>
using namespace std;

template <class TObject>
class ObjectPool
{
private:
    std::list<TObject *> idlepool;
    size_t maxTotal; // 8 
    size_t maxIdle;  // 6
    size_t totalObject = 0;

    void release(TObject *ptr)
    {
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
            delete pobj;
            pobj = nullptr;
        }
        idlepool.clear();
    }
};
#endif 