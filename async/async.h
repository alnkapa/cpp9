// library/async.h
#ifndef ASYNC_H
#define ASYNC_H

#include <cstddef>

namespace async
{
using char_type = char;
using size_type = std::size_t;
using context_type = std::size_t;

using error_code = int;

const error_code error_ok = 0;
const error_code error_context_not_found = 1;
const error_code error_main_proccess_exit = 2;

context_type
connect(size_type N = 3);

error_code
receive(context_type &ctx, const char_type *ptr, size_type size) noexcept;

error_code
disconnect(context_type &ctx) noexcept;

} // namespace async

#endif // ASYNC_H
