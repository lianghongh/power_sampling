//
// Created by root on 18-4-11.
//

#include "temp.h"
#include "msr_config.h"
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

double temp_power[5][TEMP_MAX_COUNT];
uint64_t pkg_power[TEMP_MAX_COUNT],pp0_power[TEMP_MAX_COUNT];
char temp[50][20];
int temp_count;
char substr[10];
uint32_t pkg_reg,pp0_reg;

void cp(char *src, char *dest,int from ,int to)
{
    int i;
    for(i=from;i<=to;i++)
        dest[i-from]=src[i];
    dest[i-from]='\0';
}


void task(int signal)
{
    if(signal==SIGALRM)
    {
        FILE *fp=popen("sensors","r");
        if(!fp) {
            printf("Cannot open sensors!\n");
            exit(1);
        }
        for(int i=0;i<50;i++)
        {
            if(fscanf(fp,"%s",temp[i])==EOF)
                break;
        }
        cp(temp[7],substr,1,4);
        temp_power[0][temp_count]=strtod(substr,NULL);
        cp(temp[16], substr, 1, 4);
        temp_power[1][temp_count]=strtod(substr,NULL);
        cp(temp[25],substr,1,4);
        temp_power[2][temp_count] = strtod(substr, NULL);
        cp(temp[34], substr, 1, 4);
        temp_power[3][temp_count] = strtod(substr, NULL);
        cp(temp[43],substr,1,4);
        temp_power[4][temp_count]=strtod(substr,NULL);
        pkg_power[temp_count] = rdmsr_on_cpu(pkg_reg, 0);
        pp0_power[temp_count] = rdmsr_on_cpu(pp0_reg, 0);
        temp_count++;
//        printf("%d\n", temp_count);
        pclose(fp);
    }
}

void set_temp_time(uint32_t latency_sec, uint32_t latency_usec, uint32_t interval_sec, uint32_t interval_usec)
{
    struct itimerval val;
    val.it_value.tv_sec=latency_sec;
    val.it_value.tv_usec=latency_usec;
    val.it_interval.tv_sec=interval_sec;
    val.it_interval.tv_usec=interval_usec;
    setitimer(ITIMER_REAL, &val, NULL);
}


void log_cpu_temp(const char* path,uint32_t latency_sec, uint32_t latency_usec, uint32_t interval_sec, uint32_t interval_usec,uint32_t s_count)
{
    FILE *temp_log=fopen(path,"w");
    if(!temp_power)
    {
        printf("Cannot open %s!\n",path);
        exit(1);
    }
    temp_count=0;
    pkg_reg = find_msr_address("MSR_PKG_ENERGY_STATUS");
    pp0_reg=find_msr_address("MSR_PP0_ENERGY_STATUS");
    signal(SIGALRM,task);
    set_temp_time(latency_sec,latency_usec,interval_sec,interval_usec);
    while(temp_count<s_count)
        pause();

    for(int i=0;i<s_count;i++)
    {
        fprintf(temp_log, "%8.2f %8.2f %8.2f %8.2f %8.2f %15lu %15lu\n", temp_power[0][i], temp_power[1][i], temp_power[2][i],
                temp_power[3][i], temp_power[4][i], pkg_power[i], pp0_power[i]);
    }
    printf("sampling temperature data completed!\n");
    fclose(temp_log);
}