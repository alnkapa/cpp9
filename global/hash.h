#ifndef GLOBAL_H
#define GLOBAL_H
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace hash
{
enum class Algorithm
{
    SHA256,
    MD5,
    SHA1,
    CRC32
};

using value_type = unsigned char;
using size_type = std::size_t;

template <typename T>
concept HashType = std::is_constructible_v<T, const value_type *, size_type> && requires(T t) {
    { t.size() } -> std::convertible_to<size_type>;
    { t.data() } -> std::convertible_to<const value_type *>;
};

template <typename T, typename H>
concept Hasher = HashType<T> && requires(const T &t, H h) {
    { h.hash(t) } -> std::convertible_to<T>;
    { h.getAlgorithm() } -> std::convertible_to<Algorithm>;
};

} // namespace hash

#endif // GLOBAL_H