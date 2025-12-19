Результаты бенчмарков

Машина: WSL2, g++ 13.3.0, `-O3 -march=native`, ParlayLib, `PARLAY_NUM_THREADS=4`.

Команда запуска:

PARLAY_NUM_THREADS=4 ./quicksort_bench 100000000 5


--------------------------------------------------

denis@DESKTOP-NS1UQHF:/mnt/c/Users/Denis/vs-code/big_hw1_paral/build$ PARLAY_NUM_THREADS=4 ./quicksort_bench 100000000 5 
parallel-quicksort with ParlayLib
n = 100000000, rounds = 5

Running correctness tests
  random n = 0 OK
  random n = 1 OK
  random n = 2 OK
  random n = 10 OK
  random n = 100 OK
  random n = 1000 OK
  random n = 100000 OK
  already sorted OK
    reverse sorted OK
    all equal OK
  Correctness tests passed.

---Benchmarking on n = 100000000---
  Round 1from5:
    seq: 7.99903 sec
    par: 2.67696 sec

  Round 2from5:
    seq: 7.8313 sec
    par: 2.70568 sec

  Round 3from5:
    seq: 7.83502 sec
    par: 2.63242 sec

  Round 4from5:
    seq: 7.81093 sec
    par: 2.00161 sec

  Round 5from5:
    seq: 7.82129 sec
    par: 2.64515 sec

-----------------------
Avg seq time: 7.85952 s
Avg par time: 2.53236 s
Speedup: 3.10363x
-----------------------








denis@denis-MS-7C52:~/Downloads/parallel-quicksort-with-ParlayLib-main_with_tests_autoparlay/parallel-quicksort-with-ParlayLib-main$ ctest --test-dir build --output-on-failure
Internal ctest changing into directory: /home/denis/Downloads/parallel-quicksort-with-ParlayLib-main_with_tests_autoparlay/parallel-quicksort-with-ParlayLib-main/build
Test project /home/denis/Downloads/parallel-quicksort-with-ParlayLib-main_with_tests_autoparlay/parallel-quicksort-with-ParlayLib-main/build
    Start 1: quicksort_tests
1/1 Test #1: quicksort_tests ..................   Passed    0.09 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.10 sec
denis@denis-MS-7C52:~/Downloads/parallel-quicksort-with-ParlayLib-main_with_tests_autoparlay/parallel-quicksort-with-ParlayLib-main$ PARLAY_NUM_THREADS=4 ./build/quicksort_bench 100000000 5
parallel-quicksort with ParlayLib
n = 100000000, rounds = 5

Running correctness tests
  random n = 0 OK
  random n = 1 OK
  random n = 2 OK
  random n = 10 OK
  random n = 100 OK
  random n = 1000 OK
  random n = 100000 OK
  already sorted OK
    reverse sorted OK
    all equal OK
  Correctness tests passed.

---Benchmarking on n = 100000000---
  Round 1from5:
    seq: 8.17186 sec
    par: 2.32992 sec

  Round 2from5:
    seq: 8.48249 sec
    par: 2.5346 sec

  Round 3from5:
    seq: 8.46647 sec
    par: 2.32917 sec

  Round 4from5:
    seq: 8.18589 sec
    par: 2.22109 sec

  Round 5from5:
    seq: 8.30756 sec
    par: 2.23668 sec

-----------------------
Avg seq time: 8.32285 s
Avg par time: 2.33029 s
Speedup: 3.57159x
-----------------------
