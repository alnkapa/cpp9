// library/async.h
#ifndef ASYNC_H
#define ASYNC_H
#include <memory>

namespace async {
using char_type = char;
using size_type = std::size_t;
using id_type = std::size_t;

class Context {
public:
  virtual id_type id() = 0;
};

std::shared_ptr<Context> connect(size_type N = 3);

std::unique_ptr<std::string> receive(std::weak_ptr<Context>);

void disconnect(std::weak_ptr<Context>);

} // namespace async

#endif // ASYNC_H
