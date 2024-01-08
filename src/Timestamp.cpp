
#include<time.h>
#include<sys/time.h> 
// time; liunx mills  ms; ns;
#include "../include/Timestamp.hpp"


// std::int64_t msec_;  // 微秒 // 1970 .... 2023 7 23
// static const size_t BUFFLEN = 128;
// static const int KMSP = 1000 * 1000;

Timestamp::Timestamp(const std::int64_t ms)
    :msec_(ms)
{}
Timestamp::~Timestamp()
{}

Timestamp &Timestamp::now()
{
    *this = Timestamp::Now();
    return *this;
}
void Timestamp::swap(Timestamp &other)
{
    std::swap(this->msec_, other.msec_);
}
std::string Timestamp::toString() const
{
    char buff[BUFFLEN]={0};
    std::int64_t seconds = msec_ / KMSP;
    std::int64_t microseconds = msec_ % KMSP;
    snprintf(buff,sizeof(buff),"%ld.%06ld",seconds,microseconds);
    return std::string(buff);
}
std::string Timestamp::toFormattedString(bool showms) const
{
    char buff[BUFFLEN]={0};
    std::int64_t seconds = msec_ / KMSP;// 1970 ... 
    std::int64_t microseconds = msec_ % KMSP;
    struct tm tm_time;
    localtime_r(&seconds,&tm_time);
    int pos = snprintf(buff,BUFFLEN,"%4d/%02d/%02d-%02d:%02d:%02d",
                           tm_time.tm_year+ 1900,
                           tm_time.tm_mon + 1,
                           tm_time.tm_mday,
                           tm_time.tm_hour,
                           tm_time.tm_min,
                           tm_time.tm_sec);
    if(showms)
    {
        snprintf(buff+pos,BUFFLEN-pos,".%ldZ",microseconds);
    }

    return std::string(buff);
}
bool Timestamp::valid() const
{
    return msec_ > 0;
}
std::int64_t Timestamp::get_mesc() const
{
    return msec_;
}
time_t Timestamp::get_sec() const
{
    return (msec_ / KMSP);
}


bool Timestamp::operator<(const Timestamp &other) const
{
    return this->msec_ < other.msec_;
}
bool Timestamp::operator==(const Timestamp &other) const
{
    return this->msec_ == other.msec_;
}
bool Timestamp::operator!=(const Timestamp &other) const
{
    return !(*this == other);
}
// s
std::int64_t Timestamp::operator-(const Timestamp &other) const
{
    std::int64_t diff = this->msec_ - other.msec_;
    return (diff / KMSP);
}
Timestamp Timestamp::operator+(const std::int64_t seconds) const
{
    return Timestamp(this->msec_ + seconds * KMSP);
}
// ms;
std::int64_t Timestamp::distmsec(const Timestamp &other) const
{
    return this->msec_ - other.msec_;
}

Timestamp Timestamp::Now()
{
    struct timeval tv={};
    gettimeofday(&tv,nullptr);
  //  std::int64_t seconds = tv.tv_sec;
    return Timestamp(tv.tv_sec * KMSP+ tv.tv_usec);
}
Timestamp Timestamp::invalid()
{
    return Timestamp();
}
