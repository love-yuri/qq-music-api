module;
#include "yuri_log.hpp"

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <functional>
#include <queue>
#include <thread>
#include <iostream>
#include <utility>
#include <vector>

export module thread_pool;

// 线程池类
class ThreadPool {
public:
  std::atomic_bool is_close;               // 是否关闭线程池
  std::mutex mutex;                        // 互斥锁
  std::condition_variable cv;              // 条件变量
  std::queue<std::function<void()>> tasks; // 存放待执行的任务
  std::vector<std::thread> threads;        // 存放线程

  // 构造函数,默认初始化 5 个线程.此时线程并不会直接启动
  explicit ThreadPool(const int n = 5) {
    is_close = false;
    threads = std::vector<std::thread>(n);
  }

  // 禁止任何 移动复制 等号初始化
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  // 析构函数,调用shutdown 结束所有任务
  ~ThreadPool() {
    shutDown();
  }

  // 启动所有线程
  void init() {
    const auto size = threads.size();
    for (int i = 0; i < size; i++) {
      threads[i] = std::thread(Task(this, i + 1));
    }
  }

  // addTask 函数结束后 task_ptr 的计数不是0 是 1 ,因为之前lambda 中 捕获了一次,所以只有当task 函数被执行才会析构
  // 所以不存 return task_ptr->get_future();后就被析构了问题
  template <typename Fun, typename... Args>
  auto addTask(Fun &&fun, Args &&...args) {
    using return_type = std::invoke_result_t<Fun, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
      [f = std::forward<Fun>(fun), ... a = std::forward<Args>(args)] {
        return std::invoke(f, a...);
      });

    {
      std::lock_guard lock(mutex);
      tasks.emplace([task] { (*task)(); });
    }

    cv.notify_one();
    return task->get_future();
  }

private:
  // 关闭线程池
  void shutDown() {
    yinfo << "ThreadPool::shutDowning....";
    is_close = true; // 将关闭标志设置为false 防止线程一直阻塞
    cv.notify_all(); // 唤醒所有线程 处理任务或者结束线程
    for (auto &thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  bool getTask(std::function<void()> &fun) {
    if (tasks.empty()) {
      return false;
    }
    fun = tasks.front();
    tasks.pop();
    return true;
  }

  class Task {
    ThreadPool *pool; // 任务所属线程池
    int id;           // 任务id

  public:
    // 构造函数设置线程池,并且
    Task(ThreadPool *pool, const int id) {
      this->pool = pool;
      this->id = id;
    }

    // 重载()()
    // 创建线程传入的的Task类,他会一直调用Task()();
    void operator()() const {
      std::function<void()> fun;
      while (true) {
        {
          std::unique_lock lock(pool->mutex);
          pool->cv.wait(lock, [&] {
            return pool->is_close || !pool->tasks.empty();
          });

          if (pool->is_close && pool->tasks.empty()) {
            return;
          }

          pool->getTask(fun);
        }

        // 不需要拦截异常
        fun();
      }
    }
  };
};

export auto thread_manager = std::make_shared<ThreadPool>(5);