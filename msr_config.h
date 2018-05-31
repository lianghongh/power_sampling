//
// Created by root on 18-3-29.
//

#ifndef POWER_SAMPLING_MSR_CONFIG_H
#define POWER_SAMPLING_MSR_CONFIG_H
#include <stdint.h>

#define ERROR 0xffff
#define EVENT_COUNT 51
#define MSR_COUNT 27
#define CPU_COUNT 8
#define PMC_COUNT 4
#define FIXED_COUNT 3

typedef struct msr_event
{
    int index;
    const char *name;
    uint32_t event_code;
    uint32_t umask;
}msr_event;

typedef struct msr{
    const char *name;
    uint32_t reg;
}msr;

extern msr_event events[];
extern msr msrs[];

//Find MSRs address, event_code and umask
uint32_t find_msr_address(const char* name);
uint32_t find_event_code(const char* name);
uint32_t find_umask(const char* name);

//Read and write MSRs API
uint64_t rdmsr_on_cpu(uint32_t reg,int cpu);
void wrmsr_on_cpu(uint32_t reg, int cpu,uint64_t config_code);

//Config performance event and MSRs
void config_pmc_event(const char* pmc_name,int cpu,uint32_t event_code,uint32_t umask);
void reset_msr(int cpu);
void config_fix_msr(int cpu);
void config_global(int cpu);

char *utob(uint64_t code,uint32_t width);
uint64_t read_msr_bit(uint64_t msr,uint32_t high,uint32_t low);
void display_all(int i);


#endif
