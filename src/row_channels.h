#ifndef ROW_CHANNELS_H
#define ROW_CHANNELS_H

#include <vector>
#include <memory>
#include "channel.h"

class Range;
class RowChannels
{
public:
    RowChannels(uint8_t size, const Range& default_range);
    Channel& channel(uint8_t idx);
    const Channel& channel(uint8_t idx) const;
    size_t size() const;

private:
    std::vector<std::shared_ptr<Channel>> m_queue;
};

#endif // ROW_CHANNELS_H
