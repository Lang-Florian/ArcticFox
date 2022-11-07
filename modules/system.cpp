#pragma once

#include <sys/sysinfo.h>

unsigned long long free_ram() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  return memInfo.mem_unit * memInfo.freeram;
}

unsigned long long total_ram() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  return memInfo.mem_unit * memInfo.totalram;
}

unsigned long long used_ram() {
  return total_ram() - free_ram();
}

unsigned long long free_swap() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  return memInfo.mem_unit * memInfo.freeswap;
}

unsigned long long total_swap() {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  return memInfo.mem_unit * memInfo.totalswap;
}

unsigned long long used_swap() {
  return total_swap() - free_swap();
}

unsigned long long free_memory() {
  return free_ram() + free_swap();
}

unsigned long long total_memory() {
  return total_ram() + total_swap();
}

unsigned long long used_memory() {
  return used_ram() + used_swap();
}