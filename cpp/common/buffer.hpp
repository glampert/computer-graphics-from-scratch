#pragma once

#include <cstdint>
#include <cassert>
#include <cstring>
#include <type_traits>

namespace cgfs
{

template<typename T, std::int64_t S = 768>
struct Buffer final
{
    static constexpr std::int64_t kMaxSize = S;

    std::int64_t m_count{ 0 };
    T m_storage[kMaxSize];
    
    Buffer() = default;

    auto push_back(const T value)
    {
        assert(m_count < kMaxSize);
        m_storage[m_count++] = value;
        return *this;
    }

    auto pop_back()
    {
        assert(m_count > 0);
        --m_count;
        return *this;
    }

    auto operator[](const std::int64_t index) const -> const T
    {
        assert(index >= 0 && index < m_count);
        return m_storage[index];
    }

    auto operator[](const std::int64_t index) -> T&
    {
        assert(index >= 0 && index < m_count);
        return m_storage[index];
    }

    auto data() { return m_storage; }
    auto data() const { return m_storage; }
    auto size() const { return m_count; }
};

template<typename T, std::int64_t S>
inline auto concatenate(const Buffer<T, S>& lhs, const Buffer<T, S>& rhs) -> Buffer<T, S>
{
    static_assert(std::is_trivially_copyable_v<T>); // We're using memcpy here.

    Buffer<T, S> result{};

    assert(lhs.size() + rhs.size() <= result.kMaxSize);

    std::memcpy(result.data(), lhs.data(), lhs.size() * sizeof(T));
    std::memcpy(result.data() + lhs.size(), rhs.data(), rhs.size() * sizeof(T));

    result.m_count = lhs.size() + rhs.size();
    
    return result;
}

} // cgfs
