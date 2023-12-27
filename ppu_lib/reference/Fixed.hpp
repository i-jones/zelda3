#pragma once

class Fixed16
{
public:
    using Storage = int;

    Fixed16 static fromRaw(Storage value)
    {
        return Fixed16(value);
    }

    int toInt() const
    {
        return _value >> 8;
    }

    Storage raw() const
    {
        return _value;
    }

private:
    Fixed16(Storage value) : _value(value) {}
    Storage _value;
};

class Int13
{
public:
    using Storage = std::int16_t;
    Int13 static fromRaw(Storage value)
    {
        return Int13(value);
    }

    operator int() const { return _value; }

    friend Int13 operator-(const Int13 lhs, const Int13 rhs)
    {
        return Int13(Int13::clip(lhs._value - rhs._value));
    }

    Storage raw() const { return _value; }

private:
    Int13(Storage value) : _value(value) {}
    Storage static clip(Storage n)
    {
        // 13-bit sign extend: --s---nnnnnnnnnn -> ssssssnnnnnnnnnn
        return n & 0x2000 ? (n | ~1023) : (n & 1023);
    }
    Storage _value;
};

inline Fixed16 operator*(const Fixed16 lhs, const Int13 rhs)
{
    return Fixed16::fromRaw(lhs.raw() * rhs.raw() & ~63);
}

inline Fixed16 operator+(const Fixed16 lhs, const Int13 rhs)
{
    return Fixed16::fromRaw(lhs.raw() + (rhs.raw() << 8));
}

inline Fixed16 operator+(const Fixed16 lhs, const Fixed16 rhs)
{
    return Fixed16::fromRaw(lhs.raw() + rhs.raw());
}