#include "row_channels.h"
#include <stdexcept>

RowChannels::RowChannels(uint8_t size, const Range& default_range)
{
    for(uint8_t i = 0; i < size; ++i) {
        m_queue.emplace_back(new Channel(default_range));
    }
}

Channel &RowChannels::channel(uint8_t idx)
{
    if(idx >= m_queue.size())
        throw std::out_of_range("out of range");

    return *m_queue[idx];
}

const Channel &RowChannels::channel(uint8_t idx) const
{
    return *m_queue.at(idx);
}

size_t RowChannels::size() const
{
    return m_queue.size();
}

