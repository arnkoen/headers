// Benchmark: array.h vs std::vector, hashmap.h vs std::unordered_map
// Compile with: cl /EHsc /DNDEBUG /O2 vsstd.cpp /std:c++17
// Or: g++ -O2 vsstd.cpp -std=c++17
// As with all benchmarks, take this with a grain of salt.
// The hashmap and resizable array implementations in this repo contain way less functionality than std::unordered_map and std::vector.
// Also the results may vary depending on compiler, optimization flags, system architexture and the size of N.

#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "../array.h"
#include "../hashmap.h"

constexpr size_t N = 1000000;

static void bench_array() {
    array_t(int) arr = {0};
    array_reserve(&arr, N);
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        array_push(&arr, i);
    }
    int sum = 0;
    for (size_t i = 0; i < arr.len; ++i) {
        sum += arr.data[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "array.h: sum=" << sum << ", time=" << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
    array_free(&arr);
}

static void bench_vector() {
    std::vector<int> v;
    v.reserve(N);
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i) {
        v.push_back((int)i);
    }
    int sum = 0;
    for (size_t i = 0; i < v.size(); ++i) {
        sum += v[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "std::vector: sum=" << sum << ", time=" << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
}

static void bench_hashmap() {
    hashmap map = {0};
    hashmap_init(&map, sizeof(int), sizeof(int), N * 2, 0, 0);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < (int)N; ++i) {
        hashmap_insert(&map, &i, &i);
    }
    int sum = 0;
    for (int i = 0; i < (int)N; ++i) {
        int* v = (int*)hashmap_find(&map, &i);
        if (v) sum += *v;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "hashmap.h: sum=" << sum << ", time=" << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
    hashmap_free(&map);
}

static void bench_unordered_map() {
    std::unordered_map<int, int> m;
    m.reserve(N*2);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < (int)N; ++i) {
        m[i] = i;
    }
    int sum = 0;
    for (int i = 0; i < (int)N; ++i) {
        sum += m[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "std::unordered_map: sum=" << sum << ", time=" << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
}

int main() {
    bench_array();
    bench_vector();
    bench_hashmap();
    bench_unordered_map();
    return 0;
}
