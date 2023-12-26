#pragma once

#include <array>
#include <span>

class VRAM
{
public:
    using AddrType = uint16_t;
    static constexpr uint16_t Size = 0x8000;
    using Storage = uint16_t[Size];

    VRAM(Storage &s) : storage(s) {}

    template <typename T>
    const T &readAs(AddrType address) const
    {
        address = address & (Size - 1);
        // assert(address < Size);
        const char *base = reinterpret_cast<const char *>(&storage[address]);
        const char *end = base + sizeof(T);
        // assert(end <= reinterpret_cast<const char *>(&storage[Size]));

        return *reinterpret_cast<const T *>(base);
    }

    void write(std::span<const char> data);

private:
    Storage &storage;
};