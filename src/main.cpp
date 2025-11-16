#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <limits>
#include <algorithm>

#include <parlay/parallel.h>
#include "quicksort_parlay.h"

using Clock = std::chrono::high_resolution_clock;

struct Timer {
    Clock::time_point start;
    Timer() : start(Clock::now()) {}
    double elapsed() const {
        auto end = Clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
};

void test_with_vector(const std::vector<int>& base) {
    std::vector<int> a = base;
    std::vector<int> b = base;
    std::vector<int> ref = base;

    std::sort(ref.begin(), ref.end());

    qs_parlay::quicksort_seq(a.begin(), a.end());
    qs_parlay::quicksort_par(b.begin(), b.end());

    assert(std::is_sorted(a.begin(), a.end()));
    assert(std::is_sorted(b.begin(), b.end()));
    assert(a == ref);
    assert(b == ref);
}

void run_correctness_tests() {
    std::cout << "Running correctness tests\n";

    std::mt19937_64 gen(123456);
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    );

    auto make_random = [&](std::size_t n) {
        std::vector<int> v(n);
        for (std::size_t i = 0; i < n; ++i) v[i] = dist(gen);
        return v;
    };

    std::vector<std::size_t> sizes = {0, 1, 2, 10, 100, 1000, 100000};

    for (std::size_t n : sizes) {
        auto v = make_random(n);
        test_with_vector(v);
        std::cout << "  random n = " << n << " OK\n";
    }

    {
        std::vector<int> sorted(100000);
        for (int i = 0; i < 100000; ++i) sorted[i] = i;
        test_with_vector(sorted);
        std::cout << "  already sorted OK\n";
    }

    {
        std::vector<int> rev(100000);
        for (int i = 0; i < 100000; ++i) rev[i] = 100000 - i;
        test_with_vector(rev);
        std::cout << "    reverse sorted OK\n";
    }

    {
        std::vector<int> equal(100000, 42);
        test_with_vector(equal);
        std::cout << "    all equal OK\n";
    }

    std::cout << "  Correctness tests passed.\n\n";
}


int main(int argc, char** argv) {
    std::size_t n = 100000000;
    int rounds = 5;

    if (argc > 1) {
        n = static_cast<std::size_t>(std::stoull(argv[1]));
    }
    if (argc > 2) {
        rounds = std::stoi(argv[2]);
    }

    std::cout << "parallel-quicksort with ParlayLib\n";
    std::cout << "n = " << n << ", rounds = " << rounds << "\n\n";

    run_correctness_tests();

    std::mt19937_64 gen(42);
    std::uniform_int_distribution<int> dist(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    );

    auto make_random = [&](std::size_t n_local, std::uint64_t seed) {
        std::mt19937_64 g(seed);
        std::vector<int> v(n_local);
        for (std::size_t i = 0; i < n_local; ++i) v[i] = dist(g);
        return v;
    };

    double total_seq = 0.0;
    double total_par = 0.0;

    std::cout << "---Benchmarking on n = " << n << "---\n";

    for (int r = 0; r < rounds; ++r) {
        std::uint64_t seed_base = 1000 + r * 17;

        std::cout << "  Round " << (r + 1) << "from" << rounds << ":\n";

        {
            auto data = make_random(n, seed_base);
            Timer t;
            qs_parlay::quicksort_seq(data.begin(), data.end());
            double t_seq = t.elapsed();
            total_seq += t_seq;
            std::cout << "    seq: " << t_seq << " sec\n";
        }

        {
            auto data = make_random(n, seed_base + 123456789ull);
            Timer t;
            qs_parlay::quicksort_par(data.begin(), data.end());
            double t_par = t.elapsed();
            total_par += t_par;
            std::cout << "    par: " << t_par << " sec\n";
        }

        std::cout << std::endl;
    }

    double avg_seq = total_seq / rounds;
    double avg_par = total_par / rounds;
    double speedup = avg_seq / avg_par;

    std::cout << "-----------------------\n";
    std::cout << "Avg seq time: " << avg_seq << " s\n";
    std::cout << "Avg par time: " << avg_par << " s\n";
    std::cout << "Speedup: " << speedup << "x\n";
    std::cout << "-----------------------\n";

    return 0;
}
