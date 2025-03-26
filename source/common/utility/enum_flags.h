#pragma once

namespace Tyr::Common
{
#define BIT(x) (1u << (x))

#define ENABLE_ENUM_FLAG_OPERATORS(EnumType)                                                                           \
    inline EnumType operator|(EnumType a, EnumType b)                                                                  \
    {                                                                                                                  \
        return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(a) |                                \
                                     static_cast<std::underlying_type_t<EnumType>>(b));                                \
    }                                                                                                                  \
    inline EnumType &operator|=(EnumType &a, EnumType b)                                                               \
    {                                                                                                                  \
        a = a | b;                                                                                                     \
        return a;                                                                                                      \
    }                                                                                                                  \
    inline EnumType operator&(EnumType a, EnumType b)                                                                  \
    {                                                                                                                  \
        return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(a) &                                \
                                     static_cast<std::underlying_type_t<EnumType>>(b));                                \
    }                                                                                                                  \
    inline EnumType &operator&=(EnumType &a, EnumType b)                                                               \
    {                                                                                                                  \
        a = a & b;                                                                                                     \
        return a;                                                                                                      \
    }                                                                                                                  \
    inline EnumType operator~(EnumType a)                                                                              \
    {                                                                                                                  \
        return static_cast<EnumType>(~static_cast<std::underlying_type_t<EnumType>>(a));                               \
    }                                                                                                                  \
    inline bool operator!(EnumType a)                                                                                  \
    {                                                                                                                  \
        return static_cast<std::underlying_type_t<EnumType>>(a) == 0;                                                  \
    }

} // namespace Tyr::Common
