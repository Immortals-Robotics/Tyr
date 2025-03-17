#pragma once

namespace Immortals::Common
{
template <class T, size_t size = 10>
class MedianFilter
{
public:
    MedianFilter() = default;

    void add(const T t_data)
    {
        if (m_empty)
        {
            m_data.fill(t_data);
            m_empty = false;
        }
        else
        {
            std::shift_left(m_data.begin(), m_data.end(), 1);
            m_data.back() = t_data;
        }
    }

    T current() const
    {
        std::array<T, size> m_temp = m_data;

        const size_t middle = m_temp.size() / 2;
        std::nth_element(m_temp.begin(), m_temp.begin() + middle, m_temp.end());
        return m_temp[middle];
    }

    void reset()
    {
        m_empty = true;
    }

private:
    std::array<T, size> m_data;

    bool m_empty = true;
};
} // namespace Immortals::Common
