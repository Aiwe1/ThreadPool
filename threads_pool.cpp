#include "threads_pool.h"

ThreadPool::ThreadPool(uint32_t num_threads, QObject *obj) : QObject(obj)
{
    threads_.reserve(num_threads);
    for (uint32_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back(&ThreadPool::run, this);
    }
}

std::string ThreadPool::wait(int64_t task_id)
{
    std::unique_lock<std::mutex> lock(completed_task_ids_mtx_);
    completed_task_ids_cv_.wait(lock, [this, task_id]()->bool {
        return completed_tasks_.find(task_id) != completed_tasks_.end();
    });


    return completed_tasks_.at(task_id);
}

void ThreadPool::waitAll() {
    std::unique_lock<std::mutex> lock(mtx_);
    completed_task_ids_cv_.wait(lock, [this]()->bool {
        std::lock_guard<std::mutex> task_lock(completed_task_ids_mtx_);
        return queue_.empty() && last_idx_ == completed_tasks_.size();
    });
}

bool ThreadPool::calculated(int64_t task_id)
{
    std::lock_guard<std::mutex> lock(completed_task_ids_mtx_);
    if (completed_tasks_.find(task_id) != completed_tasks_.end()) {
        return true;
    }
    return false;
}

ThreadPool::~ThreadPool()
{
    quite_ = true;
    for (uint32_t i = 0; i < threads_.size(); ++i) {
        q_cv_.notify_all();
        threads_[i].join();
    }
}

std::unordered_map<int64_t, std::string> &ThreadPool::getComplitedTasks()
{
    return std::ref(completed_tasks_);
}

void ThreadPool::run()
{
    while (!quite_) {
        std::unique_lock<std::mutex> lock(mtx_);
        q_cv_.wait(lock, [this]()->bool { return !queue_.empty() || quite_; });

        if (!queue_.empty()) {
            auto elem = std::move(queue_.front());
            queue_.pop_front();
            emit functionInProgress(elem.second);
            lock.unlock();

            auto s = elem.first.get();

            std::lock_guard<std::mutex> lock(completed_task_ids_mtx_);
            completed_tasks_.insert({ elem.second, s});

            emit functionReleased(elem.second, completed_tasks_.at(elem.second));
            completed_task_ids_cv_.notify_all();
        }
    }
}










