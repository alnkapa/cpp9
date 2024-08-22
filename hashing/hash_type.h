#ifndef HASH_TYPE_H
#define HASH_TYPE_H
#include "../global/hash.h"
#include <vector>
#include <iostream>


namespace hash
{

class HashTypeImpl : private std::vector<value_type>
{
  public:
    using std::vector<value_type>::size;
    using std::vector<value_type>::data;
    using std::vector<value_type>::at;
    HashTypeImpl(const value_type *ptr, size_type size) : std::vector<value_type>(ptr, ptr + size) {};
    HashTypeImpl() : std::vector<value_type>{} {};
    friend bool operator!=(const HashTypeImpl &lf, const HashTypeImpl &rh)
    {
        return static_cast<const std::vector<value_type> &>(lf) != static_cast<const std::vector<value_type> &>(rh);
    };
};

static_assert(hash::HashType<HashTypeImpl>, "HashTypeImpl does not satisfy HashType concept");

} // namespace hash

#endif