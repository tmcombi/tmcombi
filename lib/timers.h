#ifndef LIB_TIMERS_H_
#define LIB_TIMERS_H_

#include <chrono>
#include <memory>

class Timers
{
public:
    static Timers& server();
    Timers(const Timers&) = delete;
    Timers& operator=(const Timers&) = delete;

    typedef std::multimap<std::string,std::chrono::time_point<std::chrono::steady_clock>>::iterator Handler;
    typedef std::chrono::duration<double> Duration;

    static Handler start(const std::string &);
    static std::string stop(Handler);

    static void report(std::ostream &);

private:
    static std::shared_ptr<Timers> instance;
    Timers() = default;

    static std::multimap<std::string,std::chrono::time_point<std::chrono::steady_clock>> registered_timers_;
    static std::map<std::string,std::pair<size_t,Duration>> calls_and_durations_;

};

std::shared_ptr<Timers> Timers::instance = nullptr;
std::multimap<std::string,std::chrono::time_point<std::chrono::steady_clock>> Timers::registered_timers_;
std::map<std::string,std::pair<size_t,Timers::Duration>> Timers::calls_and_durations_;

Timers& Timers::server()
{
    if (instance == nullptr)
    {
        instance = std::shared_ptr<Timers>(new Timers);
    }
    return *instance;
}

Timers::Handler Timers::start(const std::string & s) {
    return registered_timers_.insert(std::make_pair(s,std::chrono::steady_clock::now()));
}

std::string Timers::stop(Timers::Handler h) {
    Duration d = std::chrono::steady_clock::now() - h->second;
    const std::string & s = h->first;

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d);
    std::string report_string = "Timer = \"" + s + "\" - " + std::to_string(ms.count()) + "ms";

    auto it = calls_and_durations_.find(s);
    if (it == calls_and_durations_.end()) {
        calls_and_durations_[s] = std::make_pair(1,d);
    } else {
        it->second.first++;
        it->second.second += d;
    }
    registered_timers_.erase(h);
    return report_string;
}

void Timers::report(std::ostream & os = std::cout) {
    os << "START: Timers report" << std::endl;
    for (auto it = calls_and_durations_.begin(); it != calls_and_durations_.end(); ++it) {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(it->second.second);
        os << "Timer name = \"" << it->first << "\": calls="
           << it->second.first << ", time=" << ms.count() << "ms" << std::endl;
    }
    if (!registered_timers_.empty()) {
        for (auto it = registered_timers_.begin(); it != registered_timers_.end(); ++it) {
            os << "Warning: timer \"" << it->first << "\" has never been stopped" << std::endl;
        }
    }
    os << "END:   Timers report" << std::endl;
}

#endif
