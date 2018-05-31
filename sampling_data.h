//
// Created by root on 18-4-3.
//

#ifndef POWER_SAMPLING_SAMPLING_DATA_H
#define POWER_SAMPLING_SAMPLING_DATA_H

#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include "msr_config.h"

#define MAX 10000

extern int c_cpu;
extern uint32_t sample_count;
extern uint64_t pmc_data[CPU_COUNT][PMC_COUNT][MAX];
extern uint64_t fixed_data[CPU_COUNT][FIXED_COUNT][MAX];
extern uint64_t power_data[3][MAX];
extern uint32_t event_code,umask,reg[PMC_COUNT],power_unit,pkg_energy,pp0_energy,fix[FIXED_COUNT],counter;

void sampling_task_list(int signal);
void sampling_task_on_cpu(int signal);
void set_time(uint32_t latency_sec, uint32_t latency_usec, uint32_t interval_sec, uint32_t interval_msec);
void sampling_cpu(int cpu,uint32_t interval_sec, uint32_t interval_msec,uint32_t sample_per_event);
void write_data_cpu(int cpu,char *events_path,char *power_path,int sample_per_count);
void sampling_event(char *event_list[],int event_num,uint32_t interval_sec, uint32_t interval_msec,uint32_t sample_per_event);
void write_event(char *data_path,int event_num,int sample_per_count);

#endif //POWER_SAMPLING_SAMPLING_DATA_H
