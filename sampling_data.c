//
// Created by root on 18-4-3.
//
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "sampling_data.h"

uint64_t pmc_data[CPU_COUNT][PMC_COUNT][MAX];
uint64_t fixed_data[CPU_COUNT][FIXED_COUNT][MAX];
uint64_t power_data[3][MAX];
uint32_t event_code,umask,reg[PMC_COUNT],power_unit,pkg_energy,pp0_energy,fix[FIXED_COUNT],sample_count,counter;

int c_cpu;

void sampling_task_list(int signal)
{
    if(signal==SIGALRM)
    {
        for(int i=0;i<CPU_COUNT;i++)
        {
            pmc_data[i][0][sample_count]=rdmsr_on_cpu(reg[0],i);
            pmc_data[i][1][sample_count]=rdmsr_on_cpu(reg[1],i);
            pmc_data[i][2][sample_count] = rdmsr_on_cpu(reg[2], i);
            pmc_data[i][3][sample_count] = rdmsr_on_cpu(reg[3], i);
            fixed_data[i][0][sample_count] = rdmsr_on_cpu(fix[0], i);
            fixed_data[i][1][sample_count] = rdmsr_on_cpu(fix[1], i);
            fixed_data[i][2][sample_count] = rdmsr_on_cpu(fix[2], i);
            power_data[0][sample_count] = rdmsr_on_cpu(pkg_energy, i);
            power_data[1][sample_count] = rdmsr_on_cpu(pp0_energy, i);
            power_data[2][sample_count] = rdmsr_on_cpu(power_unit, i);
        }
        sample_count++;
        counter++;
    }
}
void sampling_task_on_cpu(int signal)
{
    if(signal==SIGALRM)
    {
        pmc_data[c_cpu][0][sample_count]=rdmsr_on_cpu(reg[0],c_cpu);
        pmc_data[c_cpu][1][sample_count]=rdmsr_on_cpu(reg[1],c_cpu);
        pmc_data[c_cpu][2][sample_count] = rdmsr_on_cpu(reg[2], c_cpu);
        pmc_data[c_cpu][3][sample_count] = rdmsr_on_cpu(reg[3], c_cpu);
        fixed_data[c_cpu][0][sample_count] = rdmsr_on_cpu(fix[0], c_cpu);
        fixed_data[c_cpu][1][sample_count] = rdmsr_on_cpu(fix[1], c_cpu);
        fixed_data[c_cpu][2][sample_count] = rdmsr_on_cpu(fix[2], c_cpu);
        power_data[0][sample_count] = rdmsr_on_cpu(pkg_energy, c_cpu);
        power_data[1][sample_count] = rdmsr_on_cpu(pp0_energy, c_cpu);
        power_data[2][sample_count] = rdmsr_on_cpu(power_unit, c_cpu);
        sample_count++;
        counter++;
    }
}


void set_time(uint32_t latency_sec, uint32_t latency_usec, uint32_t interval_sec, uint32_t interval_usec)
{
    struct itimerval val;
    val.it_value.tv_sec=latency_sec;
    val.it_value.tv_usec=latency_usec;
    val.it_interval.tv_sec=interval_sec;
    val.it_interval.tv_usec=interval_usec;
    setitimer(ITIMER_REAL, &val, NULL);
}

void sampling_cpu(int cpu,uint32_t interval_sec, uint32_t interval_msec,uint32_t sample_per_event)
{
    printf("CPU %d Sampling Staring!\nTime Interval: %d ms\nSample Count: %d\n",cpu,interval_msec,sample_per_event);
    config_global(cpu);
    config_fix_msr(cpu);
    pkg_energy = find_msr_address("MSR_PKG_ENERGY_STATUS");
    pp0_energy = find_msr_address("MSR_PP0_ENERGY_STATUS");
    power_unit = find_msr_address("MSR_RAPL_POWER_UNIT");
    fix[0] = find_msr_address("IA32_FIXED_CTR0");
    fix[1] = find_msr_address("IA32_FIXED_CTR1");
    fix[2] = find_msr_address("IA32_FIXED_CTR2");
    sample_count=0;
    c_cpu=cpu;
    signal(SIGALRM,sampling_task_on_cpu);
    int index;
    char name[66];
    for(index=0;index<EVENT_COUNT;index+=PMC_COUNT)
    {
        counter=0;
        for(int j=0;j<PMC_COUNT;j++)
        {
            event_code=events[index+j].event_code;
            umask = events[index+j].umask;
            sprintf(name,"IA32_PMC%d",j);
            reg[j] = find_msr_address(name);
            sprintf(name,"IA32_PERFEVTSEL%d",j);
            config_pmc_event(name,cpu,event_code,umask);
        }
        set_time(0,1,interval_sec, interval_msec*1000);
        while (counter<sample_per_event)
            pause();
        printf("Sample batch %d completed!\n",1+(index>>2));
    }
    if(index>EVENT_COUNT)
    {
        int rest_event=EVENT_COUNT-index+PMC_COUNT;
        counter=0;
        for(int i=0;i<rest_event;i++)
        {
            event_code=events[index-PMC_COUNT+i].event_code;
            umask=events[index-PMC_COUNT+i].umask;
            sprintf(name,"IA32_PMC%d",i);
            reg[i] = find_msr_address(name);
            sprintf(name,"IA32_PERFEVTSEL%d",i);
            config_pmc_event(name,cpu,event_code,umask);
        }
        set_time(0,1,interval_sec, interval_msec*1000);
        while (counter<sample_per_event)
            pause();
        printf("Sample batch %d completed!\n",1+(index>>2));
    }
    reset_msr(cpu);
    printf("\nCPU %d sampling completed!\n", cpu);
}

void write_data_cpu(int cpu,char *events_path,char *power_path,int sample_per_count)
{
    FILE *power_file,*events_file;
    power_file=fopen(power_path,"w");
    events_file=fopen(events_path,"w");
    if(!power_file||!events_file)
    {
        printf("FILE OPEN ERROR!\n");
        exit(1);
    }
    int ss=EVENT_COUNT/PMC_COUNT;
    if(EVENT_COUNT%PMC_COUNT!=0)
        ss++;
    ss*=sample_per_count;
    for(int i=0;i<ss;i++)
    {
        fprintf(events_file, "%15lu %15lu %15lu %15lu %15lu %15lu %15lu\n", pmc_data[cpu][0][i], pmc_data[cpu][1][i], pmc_data[cpu][2][i],
                    pmc_data[cpu][3][i], fixed_data[cpu][0][i], fixed_data[cpu][1][i], fixed_data[cpu][2][i]);
        fprintf(power_file, "%15lu %15lu %15lu %15lu %15lu\n", power_data[0][i],
                power_data[1][i], read_msr_bit(power_data[2][i], 3, 0),
                read_msr_bit(power_data[2][i], 12, 8), read_msr_bit(power_data[2][i], 19, 16));
    }
    fclose(events_file);
    fclose(power_file);
    printf("\nCPU %d write data completed!\n",cpu);
}

void sampling_event(char *event_list[],int event_num,uint32_t interval_sec, uint32_t interval_msec,uint32_t sample_per_event)
{
    printf("Sampling Staring!\nTime Interval: %d ms\nSample Count: %d\n",interval_msec,sample_per_event);
    for(int i=0;i<CPU_COUNT;i++)
    {
        config_global(i);
        config_fix_msr(i);
    }
    pkg_energy = find_msr_address("MSR_PKG_ENERGY_STATUS");
    pp0_energy = find_msr_address("MSR_PP0_ENERGY_STATUS");
    power_unit = find_msr_address("MSR_RAPL_POWER_UNIT");
    fix[0] = find_msr_address("IA32_FIXED_CTR0");
    fix[1] = find_msr_address("IA32_FIXED_CTR1");
    fix[2] = find_msr_address("IA32_FIXED_CTR2");
    sample_count=0;
    signal(SIGALRM,sampling_task_list);
    char name[66];
    int tt_count=0;
    while (tt_count<sample_per_event)
    {
        int index=0;
        for(index=0;index<event_num;index+=PMC_COUNT)
        {
            counter=0;
            for(int j=0;j<PMC_COUNT;j++)
            {
                event_code = find_event_code(event_list[index + j]);
                umask = find_umask(event_list[index + j]);
                sprintf(name,"IA32_PMC%d",j);
                reg[j] = find_msr_address(name);
                sprintf(name,"IA32_PERFEVTSEL%d",j);
                for(int k=0;k<CPU_COUNT;k++)
                    config_pmc_event(name,k,event_code,umask);
            }
            set_time(0,1,interval_sec, interval_msec*1000);
            while (counter<2)
                pause();
        }
        tt_count++;
    }
    for(int i=0;i<CPU_COUNT;i++)
        reset_msr(i);
    printf("\nSampling data completed!\n");
}


void write_event(char *data_path,int event_num,int sample_per_count)
{
    FILE *power_file,*events_file[CPU_COUNT];
    char file_name[66];
    char cam[100];
    // create data dir
    sprintf(cam,"mkdir -p %s/data",data_path);
    system(cam);
    sprintf(cam,"chmod 777 %s/data",data_path);
    system(cam);
    printf("Create %s/data!\n",data_path);

    for(int i=0;i<CPU_COUNT;i++)
    {
        sprintf(file_name,"%s/data/cpu_%d",data_path,i);
        events_file[i]=fopen(file_name,"w");
        if(!events_file[i]){
            printf("OPEN %s failed!\n",file_name);
            exit(1);
        }
    }
    sprintf(file_name,"%s/data/power",data_path);
    power_file=fopen(file_name,"w");
    if(!power_file)
    {
        printf("OPEN %s failed!\n",file_name);
        exit(1);
    }
    int ss=event_num/PMC_COUNT;
    if(event_num%PMC_COUNT!=0)
        ss++;
    ss*=sample_per_count*2;
    for(int i=0;i<ss;i++)
    {
        for(int j=0;j<CPU_COUNT;j++)
        {
            fprintf(events_file[j], "%15lu %15lu %15lu %15lu %15lu %15lu %15lu\n", pmc_data[j][0][i], pmc_data[j][1][i], pmc_data[j][2][i],
                    pmc_data[j][3][i], fixed_data[j][0][i], fixed_data[j][1][i], fixed_data[j][2][i]);
        }
        fprintf(power_file, "%15lu %15lu %15lu %15lu %15lu\n", power_data[0][i],
                power_data[1][i], read_msr_bit(power_data[2][i], 3, 0),
                read_msr_bit(power_data[2][i], 12, 8), read_msr_bit(power_data[2][i], 19, 16));
    }
    for(int i=0;i<CPU_COUNT;i++)
        fclose(events_file[i]);
    fclose(power_file);
    printf("\nWrite data completed!\n");
}