#pragma once

#include <random>

std::mt19937_64 _random_generator (std::random_device{}());

template <typename T>
T random() {
  return _random_generator();
};