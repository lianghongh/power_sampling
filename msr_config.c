//
// Created by root on 18-3-29.
//

#include "msr_config.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>


msr_event events[] = {
        // Architectural Performance Events
        {1,  "UnHalted_Core_Cycle",                         0x3c, 0x00},  //pass          //Counts core clock whenever logical processor is in C0 state
        {2,  "UnHalted_Reference_Cycles",                   0x3c, 0x01},            //Counts at a fixed frequency whenever logical processor is in C0 state
        {3,  "Instruction_Retired",                         0xc0, 0x00},            //Counts when the last uop of an instruction retires
        {4,  "LLC_Reference",                               0x2e, 0x4f},            //Accesses to LLC
        {5,  "LLC_Misses",                                  0x2e, 0x41},  //pass          //LLC misses
        {6,  "Branch_Instruction_Retired",                  0xc4, 0x00},            //Counts when the last uop of branch instruction retires
        {7,  "Branch_Misses_Retired",                       0xc5, 0x00},            //Counts branch misses
        //Non Architectural Performance Events
        {8,  "LD_BLOCKS.STORE_FORWARD",                     0x03, 0x02},            //Loads blocked by overlapping with store buffer that cannot be forward
        {9,  "LD_BLOCKS_PARTIAL.ADDRESS_ALIAS",             0x07, 0x01},
        {10, "DTLB_LOAD_MISSES.MISS_CAUSE_A_WALK",          0x08, 0x01},  //pass          //Load misses in all TLB levels that cause a page walk of any page size
        {11, "DTLB_LOAD_MISSES.WALK_COMPLETED",             0x08, 0x0e},
        {12, "DTLB_LOAD_MISSES.STLB_HIT",                   0x08, 0x20},
        {13, "INT_MISC.RECOVERY_CYCLES",                    0x0d, 0x01},
        {14, "INT_MISC.CLEAR_RESTEER_CYCLES",               0x0d, 0x80},
        {15, "UOPS_ISSUED.ANY",                             0x0e, 0x01},            //The number of uops issued by the RAT to RS
        {16, "UOPS_ISSUED.VECTOR_WIDTH_MISMATCH",           0x0e, 0x02},
        {17, "UOPS_ISSUED.SLOW_LEA",                        0x0e, 0x20},
        {18, "ARITH.FPU_DIVIDER_ACTIVE",                    0x14, 0x01},  //pass
        {19, "L2_RQSTS.DEMAND_DATA_RDMISS",                 0x24, 0x21},
        {20, "L2_RQSTS.RFO_MISS",                           0x24, 0x22},
        {21, "L2_RQSTS.CODE_RD_MISS",                       0x24, 0x24},  //pass          //L2 cache misses when fetching instruction
        {22, "L2_RQSTS.ALL_DEMAND_MISS",                    0x24, 0x27},            //Demand requests that missed L2
        {23, "L2_RQSTS.PF_MISS",                            0x24, 0x38},
        {24, "L2_RQSTS.MISS",                               0x24, 0x3f},
        {25, "L1D_PEND_MISS.PENDING",                       0x48, 0x01},            //Increments the number of outstanding L1D misses every cycle
        {26, "DTLB_STORE_MISSES.MISS_CAUSES_A_WALK",        0x49, 0x01},   //pass          //Store misses in all TLB levels that cause page walks
        {27, "L1D.REPLACEMENT",                             0x51, 0x01},            //Counts the number of lines brought into L1d cache
        {28, "OFFCORE_REQUESTS_OUTSTANDING.DEMAND_DATA_RD", 0x60, 0x01},            //Increment each cycle of number of offcore out...
        {29, "LOCK_CYCLE.CACHE_LOCK_DURATION",              0x63, 0x02},             //Cycles in which the L1D is locked
        {30, "ICACHE_16B.IFDATA_STALL",                     0x80, 0x04},            //Cycles where a code fetch is stalled due to L1 instruction cache tag miss
        {31, "ICACHE_64B.IFTAG_MISS",                       0x83, 0x02},            //Instruction fetch tag lookups that miss in the instruction cache
        {32, "ITLB_MISSES.MISS_CAUSE_A_WALK",               0x85, 0x01},            //Misses at all ITLB levels that cause page walks
        {33, "IDQ_UOPS_NOT_DELIVERED.CORE",                 0x9c, 0x01},            //Count issue pipline slots where no uop was delivered from the front end to the back end when where there is no back-end stall
        {34, "RESOURCE_STALLS.ANY",                         0xa2, 0x01},            //resource-related stall cycle
        {35, "OFFCORE_REQUESTS.DEMAND_DATA_RD",             0xb0, 0x01},   //pass         //Demand data read requests sent to uncore
        {36, "UOPS_EXECUTED.THREAD",                        0xb1, 0x01},            //Counts the number of uops that begin execution across all ports
        {37, "UOPS_EXECUTED.CORE",                          0xb1, 0x02},
        {38, "TLB_FLUSH.DTLB_THREAD",                       0xbd, 0x01},            //DTLB flush attempts of the thread-specific entries
        {39, "BR_INST_RETIRED.ALL_BRANCHES",                0xc4, 0x04},            //Counts the number of branch instructions retired PS
        {40, "HW_INTERRUPTS_RECEIVED",                      0xcb, 0x01},            //Number of hardware interrupts received by the processor
        {41, "L2_TRANS,L2_WB",                              0xf0, 0x40},            //L2 writebacks that access L2 cache
        {42, "MEM_INST_RETIRED.STLB_MISS_LOADS",            0xd0, 0x11},  //pass
        {43, "MEM_INST_RETIRED.STLB_MISS_STORES",           0xd0, 0x12},  //pass
        {44, "MEM_LOAD_RETIRED.L1_HIT",                     0xd1, 0x01},
        {45, "MEM_LOAD_RETIRED.L2_HIT",                     0xd1, 0x02},
        {46, "MEM_LOAD_RETIRED.L3_HIT",                     0xd1, 0x04},
        {47, "MEM_LOAD_RETIRED.L1_MISS",                    0xd1, 0x08},
        {48, "MEM_LOAD_RETIRED.L2_MISS",                    0xd1, 0x10},
        {49, "MEM_LOAD_RETIRED.L3_MISS",                    0xd1, 0x20},
        {50, "DTLB_MISSES.ANY",                             0x49, 0x01}, //pass           //Counts the number of misses in the STLB which cause a page walk
        {51, "L1I.MISSES",                                  0x80, 0x02}            //Counts all instruction fetches that miss the L1I cache
};

msr msrs[] = {
        //Set Global settings
        {"IA32_PERF_GLOBAL_CTRL",     0x38f},
        {"IA32_PERF_GLOBAL_STATUS",   0x38e},
        {"IA32_PERF_GLOBAL_OVF_CTRL", 0x390},
        //Set fixed-function Performance Counters
        {"IA32_FIXED_CTR_CTRL",       0x38d},
        //Set PMCx MSRs
        {"IA32_PERFEVTSEL0",          0x186},
        {"IA32_PERFEVTSEL1",          0x187},
        {"IA32_PERFEVTSEL2",          0x188},
        {"IA32_PERFEVTSEL3",          0x189},
        //Fixed-Function Performance counters
        {"IA32_FIXED_CTR0",           0x309},
        {"IA32_FIXED_CTR1",           0x30a},
        {"IA32_FIXED_CTR2",           0x30b},
        //PMCx
        {"IA32_PMC0",                 0xc1},
        {"IA32_PMC1",                 0xc2},
        {"IA32_PMC2",                 0xc3},
        {"IA32_PMC3",                 0xc4},
        {"IA32_PMC4",                 0xc5},
        {"IA32_PMC5",                 0xc6},
        {"IA32_PMC6",                 0xc7},
        {"IA32_PMC7",                 0xc8},
        //RAPL Interface MSRs
        {"MSR_RAPL_POWER_UNIT",       0x606},
        {"MSR_PKG_POWER_LIMIT",       0x610},
        {"MSR_PKG_ENERGY_STATUS",     0x611},
        {"MSR_PKG_POWER_INFO",        0x614},
        {"MSR_PKG_PERF_STATUS",       0x613},
        {"MSR_PP0_POWER_LIMIT",       0x638},
        {"MSR_PP0_ENERGY_STATUS",     0x639},
        {"MSR_PP0_POLICY",            0x63a}
};

uint32_t find_msr_address(const char *name) {
    for (int i = 0; i < MSR_COUNT; i++) {
        if (!strcmp(msrs[i].name, name))
            return msrs[i].reg;
    }
    printf("Can't find %s address\n",name);
    return ERROR;
}


uint32_t find_event_code(const char *name) {
    for (int i = 0; i < EVENT_COUNT; i++) {
        if (!strcmp(events[i].name, name))
            return events[i].event_code;
    }
    printf("Can't find %s event_code\n",name);
    return ERROR;
}

uint32_t find_umask(const char *name) {
    for (int i = 0; i < EVENT_COUNT; i++) {
        if (!strcmp(events[i].name, name))
            return events[i].umask;
    }
    printf("Can't find %s umask\n",name);
    return ERROR;
}


uint64_t rdmsr_on_cpu(uint32_t reg, int cpu) {
    uint64_t data;
    int fd;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_RDONLY);
    if (fd < 0) {
        if (errno == ENXIO) {
            printf("rdmsr: No CPU %d found\n", cpu);
            exit(2);
        } else if (errno == EIO) {
            printf("rdmsr: CPU %d doesn't support MSRs\n", cpu);
            exit(3);
        } else {
            perror("rdmsr: open");
            exit(127);
        }
    }
    if (pread(fd, &data, sizeof(data), reg) != sizeof(data)) {
        if (errno == EIO) {
            printf("rdmsr: CPU %d cannot read MSR 0x%08"PRIx32"\n", cpu, reg);
            exit(4);
        } else {
            perror("rdmsr: pread");
            exit(127);
        }
    }
    close(fd);
    return data;
}

void wrmsr_on_cpu(uint32_t reg, int cpu, uint64_t config_code) {
    uint64_t data;
    int fd;
    char msr_file_name[64];
    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_WRONLY);
    if (fd < 0) {
        if (errno == ENXIO) {
            printf("wrmsr: No CPU %d found!\n", cpu);
            exit(2);
        } else if (errno == EIO) {
            printf("wrmsr: CPU %d doesn't support MSRs\n", cpu);
            exit(3);
        } else {
            perror("wrmsr: open");
            exit(127);
        }
    }

    data = config_code;
    if (pwrite(fd, &data, sizeof(data), reg) != sizeof(data)) {
        if (errno == EIO) {
            printf("wrmsr: CPU %d cannot set MSR ""0x%08"PRIx32" to 0x%016"PRIx64"\n", cpu, reg, data);
            exit(4);
        } else {
            perror("wrmsr: pwrite");
            exit(127);
        }
    }
    close(fd);
}

void config_pmc_event(const char *pmc_name, int cpu, uint32_t event_code, uint32_t umask) {
    uint32_t address = find_msr_address(pmc_name);
    wrmsr_on_cpu(address,cpu,0);
    if(!strcmp(pmc_name,"IA32_PERFEVTSEL0"))
        wrmsr_on_cpu(find_msr_address("IA32_PMC0"),cpu,0);
    else if(!strcmp(pmc_name,"IA32_PERFEVTSEL1"))
        wrmsr_on_cpu(find_msr_address("IA32_PMC1"),cpu,0);
    else if(!strcmp(pmc_name,"IA32_PERFEVTSEL2"))
        wrmsr_on_cpu(find_msr_address("IA32_PMC2"),cpu,0);
    else
        wrmsr_on_cpu(find_msr_address("IA32_PMC3"),cpu,0);

    if (address != ERROR) {
        uint64_t config_code = 0x43;
        config_code = (config_code << 16) + (umask << 8) + event_code;
        wrmsr_on_cpu(address, cpu, config_code);
    }
    else
    {
        printf("config_pmc_event error!\n");
        exit(1);
    }
}

void config_fix_msr(int cpu) {
    uint32_t address = find_msr_address("IA32_FIXED_CTR_CTRL");
    if (address != ERROR) {
        uint64_t config_code = 0xbbb;
        wrmsr_on_cpu(address, cpu, config_code);
    }
    else
    {
        printf("config_fix_msr error!\n");
        exit(1);
    }
}

void config_global(int cpu) {
    uint32_t address = find_msr_address("IA32_PERF_GLOBAL_CTRL");
    if (address != ERROR) {
        uint64_t config_code = 0x70000000f;
        wrmsr_on_cpu(address, cpu, config_code);
    }
    else
    {
        printf("config_global error!\n");
        exit(1);
    }
}

void reset_msr(int cpu) {
    char name[66];
    uint32_t reg = find_msr_address("IA32_PERF_GLOBAL_CTRL");
    if (reg != ERROR)
        wrmsr_on_cpu(reg, cpu, 0x70000000f);
    reg = find_msr_address("IA32_FIXED_CTR_CTRL");
    if (reg != ERROR)
        wrmsr_on_cpu(reg, cpu, 0);
    for (int i = 0; i < PMC_COUNT; i++) {
        sprintf(name, "IA32_PERFEVTSEL%d", i);
        reg = find_msr_address(name);
        if (reg != ERROR)
            wrmsr_on_cpu(reg, cpu, 0);
        sprintf(name, "IA32_PMC%d", i);
        reg = find_msr_address(name);
        if (reg != ERROR)
            wrmsr_on_cpu(reg, cpu, 0);
    }
    for (int i = 0; i < FIXED_COUNT; i++) {
        sprintf(name, "IA32_FIXED_CTR%d", i);
        reg = find_msr_address(name);
        if (reg != ERROR)
            wrmsr_on_cpu(reg, cpu, 0);
    }
}


char *utob(uint64_t code, uint32_t width) {
    char *r = (char *) malloc(66 * sizeof(char));
    if (r != NULL) {
        int i;
        for (i = 0; code; i++) {
            r[i] = code % 2 + '0';
            code >>= 1;
        }
        if (i > 0)
            r[i] = '\0';
        else {
            r[0] = '0';
            r[1] = '\0';
        }
        int length = strlen(r);
        for (i = 0; i < length / 2; i++) {
            char t = r[i];
            r[i] = r[length - i - 1];
            r[length - i - 1] = t;
        }
        if (width > length) {
            r[width] = '\0';
            for (i = length - 1; i >= 0; --i)
                r[i + width - length] = r[i];
            for (i = 0; i < width - length; i++)
                r[i] = '0';
        }
    }
    return r;
}

uint64_t read_msr_bit(uint64_t msr,uint32_t high,uint32_t low)
{
    msr>>=low;
    return msr&(1L<<high-low+1)-1;
}


void display_all(int i) {
    uint64_t result;
    char name[66];
    printf("\nCPU %d\n", i);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    sprintf(name, "IA32_PERF_GLOBAL_CTRL");
    result = rdmsr_on_cpu(find_msr_address("IA32_PERF_GLOBAL_CTRL"), i);
    printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    result = rdmsr_on_cpu(find_msr_address("IA32_FIXED_CTR_CTRL"), i);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    sprintf(name, "IA32_FIXED_CTR_CTRL");
    printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    for (int j = 0; j < FIXED_COUNT; j++) {
        sprintf(name, "IA32_FIXED_CTR%d", j);
        result = rdmsr_on_cpu(find_msr_address(name), i);
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    }
    for (int j = 0; j < PMC_COUNT; j++) {
        sprintf(name, "IA32_PERFEVTSEL%d", j);
        result = rdmsr_on_cpu(find_msr_address(name), i);
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
        sprintf(name, "IA32_PMC%d", j);
        result = rdmsr_on_cpu(find_msr_address(name), i);
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    }
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    sprintf(name, "MSR_RAPL_POWER_UNIT");
    result = rdmsr_on_cpu(find_msr_address(name), i);
    printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    sprintf(name, "MSR_PKG_ENERGY_STATUS");
    result = rdmsr_on_cpu(find_msr_address(name), i);
    printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    sprintf(name, "MSR_PP0_ENERGY_STATUS");
    result = rdmsr_on_cpu(find_msr_address(name), i);
    printf("|%25s|%s|%12lu|%#12lx|\n", name, utob(result, 64), result, result);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
}

