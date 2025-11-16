#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <cstddef>

#include <parlay/parallel.h>

static constexpr std::size_t QSORT_INSERTION_THRESHOLD = 32;
static constexpr std::size_t QSORT_PAR_THRESHOLD = 1u << 15;

namespace qs_parlay {

// === ins sort ===

template <typename RandomIt, typename Compare>
void insertion_sort(RandomIt first, RandomIt last, Compare comp) {
    for (RandomIt i = first + (first == last ? 0 : 1); i < last; ++i) {
        auto key = std::move(*i);
        RandomIt j = i;
        while (j > first && comp(key, *(j - 1))) {
            *j = std::move(*(j - 1));
            --j;
        }
        *j = std::move(key);
    }
}

// === median of three ===

template <typename RandomIt, typename Compare>
typename std::iterator_traits<RandomIt>::value_type
median_of_three(RandomIt a, RandomIt b, RandomIt c, Compare comp) {
    auto &x = *a;
    auto &y = *b;
    auto &z = *c;
    if (comp(x, y)) {
        if (comp(y, z)) return y;
        return comp(x, z) ? z : x;
    } else {
        if (comp(x, z)) return x;
        return comp(y, z) ? z : y;
    }
}

// === hoare partition ===

template <typename RandomIt, typename Compare>
RandomIt partition_hoare(RandomIt first, RandomIt last, Compare comp) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    if (last - first <= 1) return first;

    RandomIt mid_it = first + (last - first) / 2;
    RandomIt last_it = last - 1;
    T pivot = median_of_three(first, mid_it, last_it, comp);

    RandomIt i = first;
    RandomIt j = last_it;
    while (true) {
        while (comp(*i, pivot)) ++i;
        while (comp(pivot, *j)) --j;
        if (i >= j) return j;
        std::iter_swap(i, j);
        ++i;
        --j;
    }
}

// === seq quicksort ===

template <typename RandomIt, typename Compare>
void quicksort_seq_impl(RandomIt first, RandomIt last, Compare comp) {
    while (last - first > static_cast<std::ptrdiff_t>(QSORT_INSERTION_THRESHOLD)) {
        RandomIt mid = partition_hoare(first, last, comp);
        RandomIt left_first = first;
        RandomIt left_last = mid + 1;
        RandomIt right_first = mid + 1;
        RandomIt right_last = last;

        if (left_last - left_first < right_last - right_first) {
            if (left_first < left_last)
                quicksort_seq_impl(left_first, left_last, comp);
            first = right_first;
        } else {
            if (right_first < right_last)
                quicksort_seq_impl(right_first, right_last, comp);
            last = left_last;
        }
    }

    if (last - first > 1)
        insertion_sort(first, last, comp);
}

template <typename RandomIt, typename Compare>
void quicksort_seq(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;
    quicksort_seq_impl(first, last, comp);
}

template <typename RandomIt>
void quicksort_seq(RandomIt first, RandomIt last) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    quicksort_seq(first, last, std::less<T>{});
}

// === paral quicksort ===

template <typename RandomIt, typename Compare>
void quicksort_par_impl(RandomIt first, RandomIt last, Compare comp, std::size_t depth) {
    std::size_t n = static_cast<std::size_t>(last - first);
    if (n <= QSORT_PAR_THRESHOLD || depth == 0) {
        quicksort_seq_impl(first, last, comp);
        return;
    }

    RandomIt mid = partition_hoare(first, last, comp);
    RandomIt left_first = first;
    RandomIt left_last = mid + 1;
    RandomIt right_first = mid + 1;
    RandomIt right_last = last;

    if (left_first >= left_last || right_first >= right_last) {
        quicksort_seq_impl(first, last, comp);
        return;
    }

    parlay::par_do(
        [&]() { quicksort_par_impl(left_first, left_last, comp, depth - 1); },
        [&]() { quicksort_par_impl(right_first, right_last, comp, depth - 1); }
    );
}

template <typename RandomIt, typename Compare>
void quicksort_par(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) return;
    std::size_t n = static_cast<std::size_t>(last - first);

    std::size_t depth = 0;
    while ((std::size_t(1) << depth) < n) ++depth;
    depth *= 2;

    quicksort_par_impl(first, last, comp, depth);
}

template <typename RandomIt>
void quicksort_par(RandomIt first, RandomIt last) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    quicksort_par(first, last, std::less<T>{});
}

}
