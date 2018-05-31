#include "temp.h"
#include "sampling_data.h"

int main(int argc, char *argv[]) {
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

    sampling_event(events,8,0,100,2000);
    write_event("/home/lianghong/data/result/fluidanimate/data/",8,2000);

//    sampling_cpu(0,0,100,300);
//    write_data_cpu(0,"/home/lianghong/data/events_data","/home/lianghong/data/power_data",300);
    return 0;
}

