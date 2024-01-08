
#ifndef TIEMRMAPTHREAD_HPP
#define TIMERMAPTHREAD_HPP
#include<sys/epoll.h>
#include"Timer.hpp"
#include<unordered_map> // hash_table;
#include<vector>
#include<memory>
#include<thread>
using namespace std;

class TimerMap
{
private:
    std::unordered_map<int,Timer> timers;
    //std::unordered_map<int,std::shared_ptr<Timer> > timers;
    bool m_stop; // false; true;
    int m_epollfd;
    std::vector<epoll_event> m_events;
    static const int initevent = 16;
    std::thread word_thread;
public:
    TimerMap();
    ~TimerMap() ;
    TimerMap(const TimerMap &) = delete;
    TimerMap & operator=(const TimerMap &) = delete;
    bool init(int timeout);
    bool add_timer(Timer &tv);
   // void remove_timer(Timer &tv);
    void loop(int timeout);
    void set_stop(bool stop = true);
};

#endif