#ifndef LIBGTE_H
#define LIBGTE_H

#include <cstdint>

namespace PS1
{

struct SVECTOR
{
    int16_t vx;
    int16_t vy;
    int16_t vz;
    int16_t pad;

    SVECTOR(int16_t x, int16_t y, int16_t z, int16_t w) : vx(x), vy(y), vz(z), pad(w) { }
};

struct VECTOR
{
    int32_t vx;
    int32_t vy;
    int32_t vz;
    int32_t pad;
};

} // namespace PSX

#endif // LIBGTE_H
