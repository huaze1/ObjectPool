

#include<unistd.h>
#include<stdio.h>
#include<cstdint>

#include "../include/Timer.hpp"


    // using TimerCallback = std::function<void(void)>;
    // int m_fd; //
    // TimerCallback m_callback;

bool Timer::settimer(size_t interval) // 3500 
{
    bool res = false;
    struct itimerspec new_value = {0};
    new_value.it_value.tv_sec = interval / 1000; // 
    new_value.it_value.tv_nsec = (interval %1000)*1000 *1000;
    new_value.it_interval = new_value.it_value;
    if(timerfd_settime(m_fd,0,&new_value,nullptr) == 0)
    {
        res = true;
    }
    return res;
}

Timer::Timer()
    :m_fd(-1),m_callback(nullptr)
{}
Timer::~Timer()
{
    delete_timer();
}

Timer::Timer(Timer && that)
    :m_fd(that.m_fd), m_callback(that.m_callback)
{
    that.m_fd = -1;
    that.m_callback = nullptr;
}  // Timer t2(std::move(t1));
Timer &Timer::operator=(Timer &&that)
{
    if(this == &that) return *this;
    //Timer(std::move(that)).swap(*this);
    delete_timer();
    m_fd = that.m_fd;
    m_callback = that.m_callback;
    that.m_fd = -1;
    that.m_callback = nullptr;
    return *this;
} // t2 = std::move(t1);

bool Timer::init()
{
    bool res = true;
    if(m_fd > 0) return res;
    m_fd = timerfd_create(CLOCK_MONOTONIC,0);
    if(m_fd < 0) { res = false;}
    return res;
}
bool Timer::set_timer(const TimerCallback &cb, size_t intrval)
{
    bool res = false;
    if(m_fd > 0 && settimer(intrval))
    {
        res = true;
        m_callback = cb;
    }
    return res;
} 
bool Timer::reset_timer(size_t interval)
{
    bool res = false;
    if(m_fd > 0 && m_callback != nullptr && settimer(interval))
    {
        res = true;
    }
    return res;
}
bool Timer::delete_timer()
{
    bool res = false;
    if(m_fd > 0)
    {
        close(m_fd);
        m_fd = -1;
        m_callback =nullptr;
        res = true;
    }
    return res;
}
void Timer::handle_event()
{

    //clog<<"Timer::handle_event..."<<endl;
    uint64_t expire_cnt = 0;
    if(read(m_fd,&expire_cnt,sizeof(expire_cnt)) != sizeof(expire_cnt))
    {
        return ;
    }
    if(m_callback != nullptr)
    {
       // clog<<"Timer::handle_event m_callback()"<<endl;
        m_callback();
    }
}
int Timer::get_fd() const
{
    return m_fd;
}
void Timer::swap(Timer &other)
{
    std::swap(this->m_fd,other.m_fd);
    std::swap(this->m_callback,other.m_callback);
}
