#pragma once

#include "functions.h"

#include <deque>
#include <future>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <memory>
#include <QObject>

Q_DECLARE_METATYPE (std::string)

class ThreadPool : public QObject
{
    Q_OBJECT
public:
    ThreadPool(uint32_t num_threads, QObject* obj = nullptr);
    ~ThreadPool();

    template <typename Func, typename ...Args>
    void addTask(int index, const Func& task_func, Args&&... args) {
        last_idx_ = index;

        std::lock_guard<std::mutex> q_lock(mtx_);
        queue_.emplace_back(std::async(std::launch::deferred, task_func, args...), index);
        q_cv_.notify_one();
    }

    void deleteTask(int index) {
        for (int i = 0; i < queue_.size(); ++i) {
            if (queue_[i].second == index) {
                queue_.erase(queue_.begin() + i);
                return;
            }
        }
    }

    std::string wait(int64_t task_id);

    void waitAll();

    bool calculated(int64_t task_id);

    std::unordered_map<int64_t, std::string>& getComplitedTasks();

signals:
    void functionReleased(int index, std::string result);
    void functionInProgress(int index);

private:
    void run();

    std::deque<std::pair<std::future<std::string>, int64_t>> queue_;
    std::mutex mtx_;
    std::condition_variable q_cv_;

    std::unordered_map<int64_t, std::string> completed_tasks_;

    std::condition_variable completed_task_ids_cv_;
    std::mutex completed_task_ids_mtx_;

    std::vector<std::thread> threads_;

    std::atomic<bool> quite_{ false };
    std::atomic<int64_t> last_idx_ = 0;
};





