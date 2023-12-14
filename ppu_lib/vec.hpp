#pragma once

#include <array>
#include <type_traits>

template <typename T, size_t N>
struct Vec
{
    using StorageType = std::array<T, N>;
    static Vec Zero()
    {
        return Vec{};
    }

    Vec() = default;
    Vec(const StorageType &x) : elem(x) {}

    template <typename T2>
    Vec<T2, N> cast() const
    {
        Vec<T2, N> result;
        for (int i = 0; i < N; i++)
        {
            result.elem[i] = static_cast<T2>(elem[i]);
        }
        return result;
    }

    T &operator[](std::size_t idx) { return elem[idx]; }
    const T &operator[](std::size_t idx) const { return elem[idx]; }

    StorageType elem;
};

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() + std::declval<T2>()), N> operator+(const Vec<T1, N> &lhs, const Vec<T2, N> &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() + std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] + rhs[i];
    }
    return ResultType(elems);
}
// Operators

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() - std::declval<T2>()), N> operator-(const Vec<T1, N> &lhs, const Vec<T2, N> &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() - std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] - rhs[i];
    }
    return ResultType(elems);
}

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() * std::declval<T2>()), N> operator-(const Vec<T1, N> &lhs, const T2 &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() * std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] * rhs;
    }
    return ResultType(elems);
}

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() * std::declval<T2>()), N> operator-(const T1 &lhs, const Vec<T2, N> &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() * std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs * rhs[i];
    }
    return ResultType(elems);
}

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() / std::declval<T2>()), N> operator/(const Vec<T1, N> &lhs, const T2 &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() / std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] / rhs;
    }
    return ResultType(elems);
}

template <typename T>
struct Vec2 : public Vec<T, 2>
{
    using Base = Vec<T, 2>;

    static Vec2 Zero()
    {
        return Base::Zero();
    }

    Vec2(Vec<T, 2> b) : Base(std::move(b)) {}

    Vec2(T x, T y) : Base({x, y}) {}

    T &x() { return this->elem[0]; }
    const T &x() const { return this->elem[0]; }

    T &y() { return this->elem[1]; }
    const T &y() const { return this->elem[1]; }
};

// Operators

template <typename T1, typename T2>
constexpr Vec2<decltype(std::declval<T1>() - std::declval<T2>())> operator-(const Vec2<T1> &lhs, const Vec2<T2> &rhs)
{
    using RHSBase = typename Vec2<T1>::Base;
    using LHSBase = typename Vec2<T2>::Base;

    const LHSBase &lhsBase = lhs;
    const RHSBase &rhsBase = rhs;
    return lhsBase - rhsBase;
}
/*
template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() * std::declval<T2>()), N> operator-(const Vec<T1, N> &lhs, const T2 &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() * std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] * rhs;
    }
    return ResultType(elems);
}

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() * std::declval<T2>()), N> operator-(const T1 &lhs, const Vec<T2, N> &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() * std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs * rhs[i];
    }
    return ResultType(elems);
}

template <typename T1, typename T2, size_t N>
constexpr Vec<decltype(std::declval<T1>() / std::declval<T2>()), N> operator/(const Vec<T1, N> &lhs, const T2 &rhs)
{
    using ResultType = Vec<decltype(std::declval<T1>() / std::declval<T2>()), N>;
    typename ResultType::StorageType elems;
    for (int i = 0; i < N; i++)
    {
        elems[i] = lhs[i] / rhs;
    }
    return ResultType(elems);
}*/

template <typename T>
struct Vec3 : public Vec<T, 3>
{
    using Base = Vec<T, 3>;
    Vec3(T x, T y, T z) : Base({x, y, z}) {}

    T &x() { return this->elem[0]; }
    const T &x() const { return this->elem[0]; }

    T &y() { return this->elem[1]; }
    const T &y() const { return this->elem[1]; }

    T &z() { return this->elem[2]; }
    const T &z() const { return this->elem[2]; }
};