
#ifndef TIMER_HPP
#define TIMER_HPP

#include<sys/timerfd.h>
#include<functional>
#include<iostream>
using namespace std;
class Timer
{
public:
    using TimerCallback = std::function<void(void)>;
private:
    int m_fd; // 
    TimerCallback m_callback;
    bool settimer(size_t interval); // ks;
public:
    Timer();
    ~Timer();

    Timer(const Timer &) = delete;
    Timer& operator=(const Timer &) = delete;

    Timer(Timer &&);
    Timer & operator=(Timer &&);
    
    bool init();
    bool set_timer(const TimerCallback &cb,size_t intrval); // ks;
    bool reset_timer(size_t interval);
    bool delete_timer();
    void handle_event();
    int get_fd() const;
    void swap(Timer &other);
};

#endif 