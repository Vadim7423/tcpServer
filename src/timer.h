#ifndef TIMER_H
#define TIMER_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>

class Timer
{
public:
    static Timer& instance();
    void ticker();
    ~Timer();

    int registerFunc(std::function<void()> func);
    void unsetFunc(int id);
    void stop();
    void clear();
    bool isFinished() const;

private:
    Timer();
    static std::unique_ptr<Timer> s_instance;
    std::unordered_map<int, std::function<void()>> m_funcs;
    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> is_finished = false;
};

#endif // TIMER_H
