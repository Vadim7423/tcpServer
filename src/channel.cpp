#include "channel.h"
#include <mutex>

Channel::Channel(const Range& range)
    : m_range(range)
{}

void Channel::setValue(float val)
{
    std::lock_guard lock(m_mutex);
    if(val > m_range.max()) {
        val = m_range.max();
    } else if(val < m_range.min()) {
        val = m_range.min();
    }

    m_value = val;
}

float Channel::value() const
{
    std::shared_lock lock(m_mutex);
    return m_value;
}

void Channel::setStatus(Status status)
{
    std::lock_guard lock(m_mutex);
    m_status = status;
}

Channel::Status Channel::status() const
{
    std::shared_lock lock(m_mutex);
    return m_status;
}

void Channel::setRange(const Range &range)
{
    std::lock_guard lock(m_mutex);
    m_range = range;
}

const Range &Channel::range() const
{
    std::shared_lock lock(m_mutex);
    return m_range;
}
