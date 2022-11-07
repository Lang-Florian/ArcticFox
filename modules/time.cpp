#pragma once

#include <chrono>

unsigned long long nanoseconds() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
};

unsigned long long microseconds() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
};

unsigned long long milliseconds() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
};

unsigned long long seconds() {
  return std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
};

unsigned long long minutes() {
  return std::chrono::duration_cast<std::chrono::minutes>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
};