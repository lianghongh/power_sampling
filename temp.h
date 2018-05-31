//
// Created by root on 18-4-11.
//

#ifndef POWER_SAMPLING_TEMP_H
#define POWER_SAMPLING_TEMP_H

#include <stdint.h>
#include <stdio.h>

#define TEMP_MAX_COUNT 10000

extern double temp_power[5][TEMP_MAX_COUNT];
extern uint64_t pkg_power[TEMP_MAX_COUNT],pp0_power[TEMP_MAX_COUNT];
extern char temp[50][20];
extern int temp_count;
extern char substr[10];
extern uint32_t pkg_reg,pp0_reg;

void log_cpu_temp(const char* path,uint32_t latency_sec, uint32_t latency_usec, uint32_t interval_sec, uint32_t interval_usec,uint32_t s_count);

#endif; //POWER_SAMPLING_TEMP_H
