#pragma once

#include <settings/GeneralSettings.h>
#include <utility/MultiThreading.h>

#include <vector>
#include <functional>

namespace container {
    /**
     * @brief A simple wrapper around T to keep track of the thread-local instances of T.
     *        This allows access to all the thread-local data from any single thread.
     *        Note that it is assumed that all threads have a longer lifetime than this class.
     */
    template <typename T>
    class ThreadLocalWrapper {
        public:
            /**
             * @brief Create a wrapper around T, and create a thread-local instance of T for each thread using the given arguments.
             */
            template <typename... Args>
            ThreadLocalWrapper(Args&&... args) {
                auto ids = utility::multi_threading::get_global_pool()->get_thread_ids();
                for (auto& id : ids) {
                    data.emplace(id, T(args...));
                }
                data.emplace(std::this_thread::get_id(), T(args...));
            }

            /**
             * @brief Get the thread-local instance of the wrapped type.
             */
            T& get() {return data.at(std::this_thread::get_id());}

            /**
             * @brief Get the thread-local instances of the wrapped type for all threads.
             */
            std::vector<std::reference_wrapper<T>> get_all() {
                std::vector<std::reference_wrapper<T>> result; result.reserve(data.size());
                for (auto& [id, t] : data) {result.emplace_back(t);}
                return result;
            }

            /**
             * @brief Get the number of thread-local instances of the wrapped type.
             */
            std::size_t size() const {return data.size();}

            /**
             * @brief Merge all thread-local instances of the wrapped type into a single instance.
             */
            T merge() const {
                T result;
                if constexpr (std::ranges::range<T>) {
                    for (const auto& [id, t] : data) {
                        std::transform(t.begin(), t.end(), result.begin(), result.begin(), std::plus<>());
                    }
                    return result;
                } else {
                    for (const auto& [id, t] : data) {
                        result += t;
                    }
                    return result;
                }
            }

        private:
            std::unordered_map<std::thread::id, T> data;
    };
}