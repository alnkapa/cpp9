#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <condition_variable>
#include <cstddef>
#include <list>
#include <mutex>
#include <type_traits>

// Блокируемая очередь
template <typename T> class BlockingQueue {
private:
  std::list<T> m_store;
  std::size_t m_size{0};
  std::condition_variable cv;
  std::mutex m;

public:
  // Добавляет элемент в очередь, блокируя выполнение, если очередь переполнена
  explicit BlockingQueue(std::size_t size = 0) : m_size(size){};
  // Добавляет элемент в очередь, блокируя выполнение, если очередь переполнена
  void add(T value) {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [this] { return m_store.size() <= m_size; });
    m_store.emplace_back(std::move(value));
    lk.unlock();
    cv.notify_one();
  }

  // Извлекает элемент из очереди, блокируя выполнение, если очередь пуста
  T take() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [this] { return !m_store.empty(); });
    T value = std::move(m_store.front());
    m_store.pop_front();
    lk.unlock();
    cv.notify_one();
    return value;
  }
};
#endif