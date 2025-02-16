#ifndef RANGE_H
#define RANGE_H

#include <string>

class Range
{
public:
    Range(float min = 0.0000001, float max = 0.001);
    void setValue(float min, float max);

    float min() const;
    float max() const;

    std::string toString() const;

private:
    float m_minimum;
    float m_maximum;
};

#endif // RANGE_H
