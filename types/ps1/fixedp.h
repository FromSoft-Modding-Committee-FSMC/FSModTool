#ifndef FIXEDP_H
#define FIXEDP_H

#include <cstdint>

namespace PS1
{

// PSX fixed point math helpers
template<typename in = int32_t>
inline constexpr float fromFixed(in x)
{
    return static_cast<float>(x) / 4096.f;
}

template<typename out = int32_t>
inline constexpr out toFixed(float x)
{
    return static_cast<out>(x * 4096.f);
}

// PS1 fixed point angle helpers
// So... this stuff is kinda hacky. I don't really remember what's the reasoning for this math
// so you'll just have to trust me™.

template<typename in = int32_t>
inline constexpr float fromAngle(in x)
{
    return x * fromFixed(360);
}

template<typename out = int32_t>
inline constexpr out toAngle(float x)
{
    return static_cast<out>(x / fromFixed(360));
}

} // namespace PSX

#endif // FIXEDP_H
