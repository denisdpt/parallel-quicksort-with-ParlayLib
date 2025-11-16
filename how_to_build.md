git clone https://github.com/denisdpt/parallel-quicksort-with-ParlayLib.git
cd parallel-quicksort-with-ParlayLib

git submodule update --init --recursive

mkdir build
cd build
cmake ..
cmake --build . -j$(nproc)

PARLAY_NUM_THREADS=4 ./quicksort_bench 100000000 5
