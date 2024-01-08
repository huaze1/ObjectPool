#ifndef OBJECTPOOL2_HPP
#define OBJECTPOOL2_HPP

#include <list>
#include <memory>
#include <iostream>
using namespace std;

template <class TObject>
class ObjectPool
{
private:
    std::list<TObject *> idlepool;

public:
    ObjectPool() {}
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
        if (idlepool.empty())
        {
            return std::shared_ptr<TObject>(
                new TObject(std::forward<Args>(args)...),
                [&](TObject *p) -> void
                {
                    idlepool.push_back(p);
                });
        }
        else
        {
            auto ptr = idlepool.front();
            idlepool.pop_front();
            return std::shared_ptr<TObject>(
                ptr,
                [&](TObject *p) -> void
                {
                    idlepool.push_back(p);
                });
        }
    }
    size_t getsize() const
    {
        return idlepool.size();
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