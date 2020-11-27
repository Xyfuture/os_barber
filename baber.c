#include "ipc.h"


void init_sem_shm()
{
    consumer_cnt_mux_key = 100;
    sofa_key = 200;
    cutting_cnt_key = 300;
    call_barber_key = 400;
    wait_cut_finish_key = 500;
    bill_mux_key = 600;
    wait_pay_key = 700;
    bill_cnt_key = 800;
    trans_key = 900;

    pay_shm_id = 2000;
    cut_shm_id = 2100;

    //初始化信号量和共享内存

    int sem_flg = IPC_CREAT|0644;
    int sem_val = 1;
    consumer_cnt_mux_id = set_sem(consumer_cnt_mux_key,sem_val,sem_flg);
    bill_mux_id = set_sem(bill_mux_key,sem_val,sem_flg);
    wait_pay_id = set_sem(wait_pay_key,sem_val,sem_flg);

    sem_val = 4;
    sofa_id = set_sem(sofa_key,sem_val,sem_flg);
    // sem_val = 


}