// library/async.cpp
#include "async.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <vector>

namespace async {

void run(context_type ctx, size_type N);

static std::unordered_map<context_type, std::thread> ctx_map{};
static std::mutex ctx_lock{};

context_type connect(size_type N) {
  static size_type counter{0};
  std::lock_guard lock(ctx_lock);
  context_type ctx = ++counter;
  ctx_map.emplace(ctx, std::thread(run, ctx, N));
  return ctx;
};

void receive(context_type &ctx, const char_type *ptr, size_type size) {
  std::unique_lock lock(ctx_lock);
  const auto &it = ctx_map.find(ctx);
  if (it == ctx_map.end()) {
    throw std::runtime_error("Context not found");
  }
  context_type ctx_value = it->first;
  lock.unlock();
  // TODO: do with it
};

void disconnect(context_type &ctx) {
  std::lock_guard lock(ctx_lock);
  const auto &it = ctx_map.find(ctx);
  if (it == ctx_map.end()) {
    throw std::runtime_error("Context not found");
  }
  if (it->second.joinable()) {
    it->second.join();
  }
  ctx_map.erase(it);
};

} // namespace async
