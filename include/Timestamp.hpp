
#ifndef TIMESTAMP_HPP
#define TIMESTAMP_HPP
#include<iostream>
#include<string>

using namespace std;
// 1s -> 1000ks;                10^3
// 1s -> 1000*1000 ms;          10^6
// 1s -> 1000 * 1000 * 1000 ns; 10^9
class Timestamp
{
private:
    std::int64_t msec_;  // 微秒 // 1970 .... 2023 7 23
    static const size_t BUFFLEN = 128;
    static const int KMSP = 1000 * 1000;
public:
    Timestamp(const std::int64_t ms = 0);
    ~Timestamp();
    Timestamp(const Timestamp&) = default; // C++11
    Timestamp & operator=(const Timestamp &) = default; 
    Timestamp & now();
    void swap(Timestamp &other);
    std::string toString() const;
    std::string toFormattedString(bool showms = true) const;
    bool valid() const ;
    std::int64_t get_mesc() const;
    time_t get_sec() const ;
public:
    // s;
    bool operator<(const Timestamp &other) const;
    bool operator==(const Timestamp &other) const;
    bool operator!=(const Timestamp &other) const;
    std::int64_t operator-(const Timestamp &other) const;
    Timestamp operator+(const std::int64_t seconds) const;
    // ms;
    std::int64_t distmsec(const Timestamp &other) const;
public:
    static Timestamp Now();
    static Timestamp invalid();
};

#endif 