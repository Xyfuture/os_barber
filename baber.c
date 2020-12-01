#include "ipc.h"

int bid;

void barber()
{
    down(call_barber_id);
    int cid = *consumer_id_shm_content;
    printf("barber %d have consumer %d\n",bid,cid);
    *barber_id_shm_content = bid;
    up(call_consumer_id);
    printf("barber %d is cutting\n");
    sleep(rand());
    up(wait_cut_finish_id[bid]);
    printf("barber %d finishes cutting\n");
    down(bill_cnt_id);
    *wait_pay_cnt_shm_content +=1;
    up(bill_cnt_id);
    down(bill_cnt_id);
    while(*wait_pay_cnt_shm_content >0 )
    {
        printf("barber %d receive money\n");
        up(wait_pay_id);
        *wait_pay_cnt_shm_content -=1;
    }
    up(bill_cnt_id);
}

