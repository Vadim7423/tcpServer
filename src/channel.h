#ifndef CHANNEL_H
#define CHANNEL_H

#include "range.h"
#include <shared_mutex>

class Channel
{
public:
    enum class Status {
        idle_state,
        measure_state,
        busy_state,
        error_state
    };

    void setValue(float val);
    float value() const;
    void setStatus(Status status);
    Status status() const;
    void setRange(const Range& range);
    const Range& range() const;

private:
    friend class RowChannels;
    Channel(const Range& range);

    mutable std::shared_mutex m_mutex;
    Range m_range;
    float m_value = 0;
    Status m_status = Status::idle_state;
};

#endif // CHANNEL_H
