#include "ipc.h"

int bid;
int sleep_time;

void barber()
{
    down(call_barber_id);
    int cid = *consumer_id_shm_content;
    printf("barber %d have consumer %d\n",bid,cid);
    sleep(sleep_time);
    *barber_id_shm_content = bid;
    up(call_consumer_id);
    printf("barber %d is cutting\n",bid);
    sleep(sleep_time);
    up(wait_cut_finish_id[bid]);
    printf("barber %d finishes cutting\n",bid);
    down(bill_cnt_id);
    *wait_pay_cnt_shm_content +=1;
    up(bill_cnt_id);
    down(bill_cnt_id);
    while(*wait_pay_cnt_shm_content >0 )
    {
        printf("barber %d receive money\n",bid);
        up(wait_pay_id);
        *wait_pay_cnt_shm_content -=1;
        sleep(sleep_time);
    }
    up(bill_cnt_id);
}

int main(int argc,char** argv)
{
    if (argv[1]!=NULL)
        bid = atoi(argv[1]);
    else
        bid = 1;
    if(argc == 3)
        sleep_time = atoi(argv[2]);
    else
        sleep_time = 4;
    init_sem_shm();
    barber();
    return 0;
}
