//
// Created by Nikita Koval on 2019-01-01.
//

#ifndef GALOIS_MULTIPRIORITYQUEUE_H
#define GALOIS_MULTIPRIORITYQUEUE_H

#include <vector>
#include <mutex>
#include <random>

using namespace std;

template <typename T, typename Cmp = std::less<T>, const size_t size = 40>
class MultiPriorityQueue : private boost::noncopyable {
    struct Heap {
        std::mutex lock;
        std::vector<T> data;

        Heap(): data() {}
    };

    Heap heaps[size];
    std::atomic<int> nonEmptyHeaps;
    Cmp cmp;

public:
    MultiPriorityQueue() : nonEmptyHeaps(0) {
        static_assert(size >= 2);
    }

    //! Pushes a value onto the queue
    void push(const T& val) {
        auto i = rand(0, static_cast<int>(size - 1));
        std::lock_guard<std::mutex> lock(heaps[i].lock);
        auto wasEmpty = heaps[i].data.empty();
        heaps[i].data.push_back(val);
        std::push_heap(heaps[i].data.begin(), heaps[i].data.end(), cmp);
        if (wasEmpty) { nonEmptyHeaps.fetch_add(1); }
    }

    //! Pushes a range onto the queue
    template <typename Iter>
    void push(Iter b, Iter e) {
        while (b != e)
            push(*b++);
    }

    /**
     * Pushes initial range onto the queue. Called with the same b and e on each
     * thread
     */
    template <typename RangeTy>
    void push_initial(const RangeTy&) {
        auto rp = range.local_pair();
        push(rp.first, rp.second);
    }

    //! Pops a value from the queue.
    galois::optional<T> pop() {
        while(true) {
            if (empty()) galois::optional<T>();
            auto i1 = rand(0, static_cast<int>(size - 2));
            auto i2 = rand(i1 + 1, static_cast<int>(size - 1));
            std::lock_guard<std::mutex> lock1(heaps[i1].lock);
            std::lock_guard<std::mutex> lock2(heaps[i2].lock);


            if (heaps[i1].data.empty() && heaps[i2].data.empty()) { continue; }
            if (heaps[i1].data.empty()) { return popInternal(i2); }
            if (heaps[i2].data.empty()) { return popInternal(i1); }

            auto res1 = heaps[i1].data.front(), res2 = heaps[i2].data.front();
            if (cmp(res2, res1)) {
                return popInternal(i1);
            } else {
                return popInternal(i2);
            }
        }
    }

    bool empty() {
        return nonEmptyHeaps.load() == 0;
    }

private:
    galois::optional<T> popInternal(int i) {
        std::pop_heap(heaps[i].data.begin(), heaps[i].data.end(), cmp);
        auto res = heaps[i].data.back();
        heaps[i].data.pop_back();
        if (heaps[i].data.empty()) { nonEmptyHeaps.fetch_add(-1); }
        return galois::optional<T>(res);
    }

    int rand(int low, int high) {
        thread_local static std::default_random_engine re {};
        using Dist = std::uniform_int_distribution<int>;
        static Dist uid {};
        return uid(re, Dist::param_type{low,high});
    }
};
GALOIS_WLCOMPILECHECK(MultiPriorityQueue)

#endif //GALOIS_MULTIPRIORITYQUEUE_H
