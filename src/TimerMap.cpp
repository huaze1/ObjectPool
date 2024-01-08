
#include<string.h>
#include<iostream>
using namespace std;


#include"../include/Timer.hpp"
#include "../include/TimerMap.hpp"


// std::unordered_map<int,Timer> timers;
// bool m_stop;
// int m_epollfd;
// std::vector<epoll_event> m_events;

TimerMap::TimerMap()
    :m_stop(true),m_epollfd(-1)
{
    m_events.resize(initevent);
    //m_events.reserve(initevent);
    //m_events.assign(initevent);
}
TimerMap::~TimerMap()
{
    set_stop(true);
}

bool TimerMap::init()
{
    bool res = false;
    m_epollfd = epoll_create1(EPOLL_CLOEXEC);
    if(m_epollfd > 0) 
    { 
        res = true;
        m_stop = false; 
    }
    return res;
}
bool TimerMap::add_timer(Timer &tv)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = tv.get_fd();
    if(epoll_ctl(m_epollfd,EPOLL_CTL_ADD,tv.get_fd(),&ev) < 0)
    {
        fprintf(stderr,
        "timer_manager: epoll_ctl EPOLL_CTL_ADD failed ,error = %s\n",
        strerror(errno)); 
        return false;        
    }
    timers[tv.get_fd()] = std::move(tv);
    return true;
}
//void TimerMap::remove_timer(Timer &tv);

void TimerMap::loop(int timeout)
{
    while(!m_stop)
    {
        cout<<"loop "<<timeout<<endl;
        int n = epoll_wait(m_epollfd,m_events.data(),m_events.size(),timeout);
        cout<<"epoll_wait : n "<<n<<endl;
        for(int i = 0;i<n;++i)
        {
            int fd = m_events[i].data.fd;
            // std::unordered_map<int,Timer>::iterator it = timers.find(fd);
            auto it = timers.find(fd);
            if(it != timers.end())
            {
                Timer &tv = it->second;
                tv.handle_event();
            }
        }
        if(n >= m_events.size())
        {
            m_events.resize(m_events.size()*2);
        }
    }
}
void TimerMap::set_stop(bool stop)
{
    m_stop = stop;
    close(m_epollfd);
    m_epollfd = -1;
}