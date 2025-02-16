#include "timer.h"

std::unique_ptr<Timer> Timer::s_instance;

Timer::Timer()
{
    m_thread.reset(new std::thread(&Timer::ticker, this));
}

Timer &Timer::instance()
{
    if(!s_instance) {
        s_instance.reset(new Timer);
    }

    return *s_instance;
}

void Timer::ticker()
{
    while(!is_finished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for(auto& [key, func] : m_funcs) {
            func();
        }
    }
}

Timer::~Timer()
{
    if(m_thread->joinable()) {
        m_thread->join();
    }
}

int Timer::registerFunc(std::function<void ()> func)
{
    static int key = 0;
    m_funcs[key] = func;
    ++key;
    return key - 1;
}

void Timer::unsetFunc(int id)
{
    if(m_funcs.count(id)) {
        m_funcs.erase(id);
    }
}

void Timer::stop()
{
    is_finished = true;
}

void Timer::clear()
{
    m_funcs.clear();
}

bool Timer::isFinished() const
{
    return is_finished;
}
