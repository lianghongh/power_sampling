#include "temp.h"
#include "sampling_data.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if(argc!=4)
    {
        printf("Usage: ./power_sample <program> <time_intervel(ms)> <sample_count>\n");
        printf("\nWrong Parameter!\n");
        exit(1);
    }

    char *events[]={
            "Instruction_Retired",
            "RESOURCE_STALLS.ANY",
            "UOPS_ISSUED.ANY",
            "Branch_Misses_Retired",
            "UOPS_EXECUTED.THREAD",
            "L2_TRANS,L2_WB",
            "Branch_Instruction_Retired",
            "L2_RQSTS.RFO_MISS"
    };


    sampling_event(events,8,0,atoi(argv[2]),atoi(argv[3]));
    char output_path[100];
    sprintf(output_path,"/home/lianghong/data/%s",argv[1]);
    write_event(output_path,8,atoi(argv[3]));

//    display_all(0);
    return 0;
}

