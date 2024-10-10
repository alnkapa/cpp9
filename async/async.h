// library/async.h
#ifndef ASYNC_H
#define ASYNC_H

#include <cstddef>

namespace async {
using char_type = char;
using size_type = std::size_t;
using context_type = std::size_t;

context_type connect(size_type N = 3);

void receive(context_type &ctx, const char_type *ptr, size_type size);

void disconnect(context_type &ctx);

} // namespace async

#endif // ASYNC_H
