#ifndef CLOUD_LUA_CPP_UTILITY_HEADER
#define CLOUD_LUA_CPP_UTILITY_HEADER

#include <utility>

namespace Cloud
{
    template <std::size_t... _Indices>
    struct Indices {};

    template <std::size_t _N, std::size_t... _Indices>
    struct IndicesBuilder : IndicesBuilder<_N - 1, _N - 1, _Indices...> {};

    template <std::size_t... _Indices>
    struct IndicesBuilder<0, _Indices...>
    {
        using Type = Indices<_Indices...>;
    };

    template<std::size_t _IndexCount>
    using IndicesBuilderType = typename IndicesBuilder<_IndexCount>::Type;
}

#endif // CLOUD_LUA_CPP_UTILITY_HEADER
