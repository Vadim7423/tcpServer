#include "range.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

Range::Range(float min, float max)
    : m_minimum(min)
    , m_maximum(max)
{
    if(m_minimum > m_maximum)
        throw std::logic_error("incorrect range");
}

void Range::setValue(float min, float max)
{
    if(min > max)
        throw std::logic_error("incorrect range");

    m_minimum = min;
    m_maximum = max;
}

float Range::min() const
{
    return m_minimum;
}

float Range::max() const
{
    return m_maximum;
}

std::string Range::toString() const
{
    std::stringstream left;
    left << std::fixed << std::setprecision(7) << m_minimum;

    std::stringstream right;
    right << std::fixed << std::setprecision(7) << m_maximum;

    std::string result;
    result += left.str() + ", " + right.str();
    return result;
}
