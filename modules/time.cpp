#pragma once

#include <chrono>
#include <string>

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

std::string timestamp() {
  std::time_t time = std::chrono::system_clock::to_time_t(
    std::chrono::system_clock::now()
  );
  char timestamp[20];
  strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", std::localtime(&time));
  return std::string(timestamp);
};