// library/async.cpp
#include "async.h"
#include <iostream>
#include <string>

namespace async {

class ContextImpl final : public Context {
private:
  id_type m_id{0};
  size_type N{3};
  std::string m_command{};

public:
  ContextImpl(id_type id, size_type N) : m_id(id), N(N){};
  ~ContextImpl(){};

  id_type id() override { return m_id; };

  void receive(const char_type **ch, size_type *size) {
    std::getline(std::cin, m_command);
    *size = m_command.size();
    *ch = m_command.c_str();
  };

  void disconnect(){
      // DO NOTHING
  };
};

std::shared_ptr<Context> connect(size_type N) {
  static size_type counter = 0;
  ++counter;
  return std::make_shared<ContextImpl>(counter, N);
};

void receive(std::weak_ptr<Context> ctx_ptr, const char_type **ch,
             size_type *size) {
  if (auto ptr = ctx_ptr.lock()) {
    if (auto ptr1 = std::dynamic_pointer_cast<ContextImpl>(ptr)) {
      ptr1->receive(ch, size);
    };
  };
};

void disconnect(std::weak_ptr<Context> ctx_ptr) {
  if (auto ptr = ctx_ptr.lock()) {
    if (auto ptr1 = std::dynamic_pointer_cast<ContextImpl>(ptr)) {
      ptr1->disconnect();
    };
  };
};

} // namespace async
