#include "stdafx.h"
#include <iostream>
#include <intrin.h>
#include <time.h>
#include <utility>
#include <fstream>

#pragma intrinsic(__rdtsc)

#define DUP(x) x x

unsigned date(const unsigned i) {
  if (i < 32 * 1024)
    return 1024;
  else if (i < 256 * 1024)
    return 7168;
  else if (i < 6 * 1024 * 1024)
    return 98304;
  else
    return 49152;
}

int main() {
  std::ofstream fout_direct("direct.txt");
  std::ofstream fout_reverse("reverse.txt");
  std::ofstream fout_rand("rand.txt");
  // L1 = 32Kb; L2 = 256Kb; L3 = 6Mb
  double w = 2600000000;
  double nano = 1000;
  auto volatile a = 0;
  auto cache_line_size = 64;
  auto index = 0;

  //прямой проход
  for (auto i = 16384; i < 9437184; i += date(i)) {
    auto q = i / 4;
    auto volatile *array = new int[q];
    auto lines_count = q / cache_line_size;

    for (auto i = 0; i < lines_count; ++i)
      array[i * cache_line_size] = (i + 1) * cache_line_size;
    array[(lines_count - 1) * cache_line_size] = 0;

    index = 0;
    for (auto j = 0; j < lines_count; ++j) {
      index = array[index];
    }

    index = 0;
    unsigned __int64 start = __rdtsc();
    for (auto i = 0; i < 1000000; ++i) {
      DUP(DUP(DUP(DUP(DUP(DUP(index = array[index];))))))
    }
    unsigned __int64 end = __rdtsc();
    double result = 1.0 * (end - start) * nano / w;
    std::cout << "size : " << 1.0 * i / 1024 << " result : " << result
              << std::endl;
    fout_direct << 1.0 * i / 1024 << '\t' << result << std::endl;
    delete[] array;
  }
  //обратный проход
  for (auto i = 16384; i < 9437184; i += date(i)) {
    auto q = i / 4;
    auto volatile *array = new int[q];
    auto lines_count = q / cache_line_size;

    for (auto i = 1; i < lines_count; ++i)
      array[i * cache_line_size] = (i - 1) * cache_line_size;
    array[0] = (lines_count - 1) * cache_line_size;

    index = 0;
    for (auto j = 0; j < lines_count; ++j) {
      index = array[index];
    }

    index = 0;
    unsigned __int64 start = __rdtsc();
    for (auto i = 0; i < 1000000; ++i) {
      DUP(DUP(DUP(DUP(DUP(DUP(index = array[index];))))))
    }
    unsigned __int64 end = __rdtsc();
    double result = 1.0 * (end - start) * nano / w;
    std::cout << "size : " << 1.0 * i / 1024 << " result : " << result
              << std::endl;
    fout_reverse << 1.0 * i / 1024 << '\t' << result << std::endl;
    delete[] array;
  }
  //случайный обход
  for (auto i = 16384; i < 9437184; i += date(i)) {
    auto q = i / 4;
    auto volatile *array = new int[q];
    auto lines_count = q / cache_line_size;

    for (auto i = 0; i < lines_count; ++i)
      array[i * cache_line_size] = (i + 1) * cache_line_size;
    array[(lines_count - 1) * cache_line_size] = 0;

    index = 0;
    for (auto j = 0; j < lines_count; ++j) {
      index = array[index];
    }

    for (auto i = 0; i < lines_count * 100; ++i) {
      auto index1 = (rand() % lines_count) * cache_line_size;
      auto index2 = (rand() % lines_count) * cache_line_size;
      std::swap(array[index1], array[index2]);
    }

    index = 0;
    unsigned __int64 start = __rdtsc();
    for (auto i = 0; i < 1000000; ++i) {
      DUP(DUP(DUP(DUP(DUP(DUP(index = array[index];))))))
    }
    unsigned __int64 end = __rdtsc();
    double result = 1.0 * (end - start) * nano / w;
    std::cout << "size : " << 1.0 * i / 1024 << " result : " << result
              << std::endl;
    fout_rand << 1.0 * i / 1024 << '\t' << result << std::endl;
    delete[] array;
  }
  return 0;
}
