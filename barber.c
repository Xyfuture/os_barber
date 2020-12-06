#include "ipc.h"

int bid;
int sleep_time;

void barber()
{
    down(call_barber_id);//有顾客唤醒自己,说明有人来,否则相当于没人,就等待,理发师睡眠
    int cid = *consumer_id_shm_content;//获取消费者id,这个缓冲区在消费者端进行了互斥,这里就不用了
    printf("barber %d have consumer %d\n",bid,cid);
    *barber_id_shm_content = bid;//传输自己的id
    up(call_consumer_id);//告诉消费者传输完毕,进行同步
    printf("barber %d is cutting\n",bid);
    sleep(sleep_time);
    up(wait_cut_finish_id[bid]);//告诉消费者理发完成
    printf("barber %d finishes cutting\n",bid);
    down(bill_cnt_id);//账本只能一个人操作,互斥进行
    *wait_pay_cnt_shm_content +=1;//理完发没付钱的人数增加
    up(bill_cnt_id);
    //收款阶段
    down(bill_cnt_id);//账本互斥操作
    while(*wait_pay_cnt_shm_content >0 )//收钱时,一个理发师将所有等待付钱的人的钱全部收掉
    {
        printf("barber %d receive money\n",bid);
        up(wait_pay_id);//收钱,允许一个理发师收多个人的钱
        *wait_pay_cnt_shm_content -=1;
        sleep(sleep_time);
    }
    up(bill_cnt_id);//操作完成,释放账本资源
}

int main(int argc,char** argv)
{
    // if (argv[1]!=NULL)
    //     bid = atoi(argv[1]);
    // else
    //     bid = 1;
    // if(argc == 3)
    //     sleep_time = atoi(argv[2]);
    // else
    //     sleep_time = 4;
    sleep_time = 4;
    int pid =0 ;
    int process_num = 3;

    init_sem_shm();
    for(int i=0;i<process_num;i++)
    {
        bid = i + 1;
        pid = fork();
        if(pid == 0)
            break;
    }

    if(pid != 0)
    {
        int status = 0;
        for(int i=0;i<process_num;i++)
            wait(&status);
        printf("all processs exit\n");
        return 0;
    }
    while (1)
        barber();
    return 0;
}
