#include <algorithm>
#include <atomic>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "quicksort_parlay.h"


namespace test {

static int g_failed = 0;

void fail(const std::string& where, const std::string& msg) {
  ++g_failed;
  std::cerr << "[FAIL] " << where << ": " << msg << "\n";
}

void pass(const std::string& where) {
  std::cout << "[ OK ] " << where << "\n";
}

#define TEST_ASSERT(cond, msg) \
  do { \
    if (!(cond)) { \
      ::test::fail(__func__, (msg)); \
      return; \
    } \
  } while (0)


struct ComparisonLimitExceeded : std::exception {
  const char* what() const noexcept override {
    return "comparison limit exceeded (likely O(n^2) behavior)";
  }
};

template <typename T>
struct CountingLess {
  std::shared_ptr<std::atomic<std::uint64_t>> counter;
  std::uint64_t limit = std::numeric_limits<std::uint64_t>::max();
  bool hard_limit = false;

  CountingLess(std::shared_ptr<std::atomic<std::uint64_t>> c,
               std::uint64_t lim,
               bool hard)
      : counter(std::move(c)), limit(lim), hard_limit(hard) {}

  bool operator()(const T& a, const T& b) const {
    auto cur = counter->fetch_add(1, std::memory_order_relaxed) + 1;
    if (hard_limit && cur > limit) {
      throw ComparisonLimitExceeded{};
    }
    return a < b;
  }
};

static std::uint64_t approx_log2(std::uint64_t n) {
  std::uint64_t lg = 0;
  while (n > 1) {
    n >>= 1;
    ++lg;
  }
  return lg;
}

template <typename SortFn>
void check_not_quadratic_on_many_equal_seq(const std::string& name, SortFn sort_fn) {
  const std::size_t n = 250'000;
  std::vector<std::int32_t> v(n, 42);

  auto counter = std::make_shared<std::atomic<std::uint64_t>>(0);
  const std::uint64_t lg = approx_log2(static_cast<std::uint64_t>(n));
  const std::uint64_t max_cmp = 200ull * static_cast<std::uint64_t>(n) * (lg + 1) + 10ull * static_cast<std::uint64_t>(n);
  CountingLess<std::int32_t> cmp(counter, max_cmp, /*hard_limit=*/true);

  try {
    sort_fn(v.begin(), v.end(), cmp);
  } catch (const ComparisonLimitExceeded&) {
    fail(name, "слишком много сравнений на массиве из одинаковых элементов: похоже на O(n^2)");
    return;
  }

  TEST_ASSERT(std::is_sorted(v.begin(), v.end()), "массив не отсортирован");
  const auto used = counter->load(std::memory_order_relaxed);
  TEST_ASSERT(used <= max_cmp,
              "слишком много сравнений: used=" + std::to_string(used) +
                  " limit=" + std::to_string(max_cmp));
  pass(name);
}

template <typename SortFn>
void check_not_quadratic_on_few_unique_seq(const std::string& name, SortFn sort_fn) {
  const std::size_t n = 400'000;
  std::vector<std::int32_t> v(n);
  for (std::size_t i = 0; i < n; ++i) v[i] = static_cast<std::int32_t>(i & 7);

  auto counter = std::make_shared<std::atomic<std::uint64_t>>(0);
  const std::uint64_t lg = approx_log2(static_cast<std::uint64_t>(n));
  const std::uint64_t max_cmp = 250ull * static_cast<std::uint64_t>(n) * (lg + 1) + 10ull * static_cast<std::uint64_t>(n);
  CountingLess<std::int32_t> cmp(counter, max_cmp, /*hard_limit=*/true);

  try {
    sort_fn(v.begin(), v.end(), cmp);
  } catch (const ComparisonLimitExceeded&) {
    fail(name, "слишком много сравнений на массиве с большим числом дубликатов: похоже на O(n^2)");
    return;
  }

  TEST_ASSERT(std::is_sorted(v.begin(), v.end()), "массив не отсортирован");
  pass(name);
}

void test_correctness_small() {
  std::mt19937_64 gen(123);
  std::uniform_int_distribution<std::int32_t> dist(-1000, 1000);

  for (int it = 0; it < 50; ++it) {
    const std::size_t n = 1 + (it * 37) % 5000;
    std::vector<std::int32_t> a(n);
    for (auto& x : a) x = dist(gen);

    auto b = a;
    auto ref = a;
    std::sort(ref.begin(), ref.end());

    qs_parlay::quicksort_seq(a.begin(), a.end());
    qs_parlay::quicksort_par(b.begin(), b.end());

    TEST_ASSERT(a == ref, "seq: результат не совпадает со std::sort");
    TEST_ASSERT(b == ref, "par: результат не совпадает со std::sort");
  }

  pass(__func__);
}

void test_no_quadratic_equal_seq() {
  check_not_quadratic_on_many_equal_seq(
      __func__,
      [](auto first, auto last, auto comp) {
        qs_parlay::quicksort_seq(first, last, comp);
      });
}

void test_no_quadratic_few_unique_seq() {
  check_not_quadratic_on_few_unique_seq(
      __func__,
      [](auto first, auto last, auto comp) {
        qs_parlay::quicksort_seq(first, last, comp);
      });
}

void test_many_equal_correctness_par() {
  const std::size_t n = 300'000;
  std::vector<std::int32_t> v(n, 7);
  qs_parlay::quicksort_par(v.begin(), v.end());
  TEST_ASSERT(std::is_sorted(v.begin(), v.end()), "par: массив не отсортирован");
  pass(__func__);
}

}

int main() {
  using namespace test;
  std::cout << "Running quicksort tests...\n";

  test_correctness_small();
  test_no_quadratic_equal_seq();
  test_no_quadratic_few_unique_seq();
  test_many_equal_correctness_par();

  if (g_failed == 0) {
    std::cout << "All tests passed.\n";
    return 0;
  }
  std::cerr << g_failed << " test(s) failed.\n";
  return 1;
}
