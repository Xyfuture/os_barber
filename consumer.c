#include "ipc.h"

int cid;
int sleep_time;


void consumer()
{
    sleep(sleep_time);
    down(consumer_cnt_mux_id);
    if(*consumer_cnt_shm_content <20)
    {
        //进入理发店
        (*consumer_cnt_shm_content) += 1;
        up(consumer_cnt_mux_id);
        printf("consumer %d enter waiting room\n",cid);
        down(sofa_id); // 尝试从等待室进入沙发
        sleep(sleep_time);
        printf("consumer %d enter sofa room\n",cid);
        down(cutting_cnt_id); // 看看能不能开始理发
        //开始理发,能开始理发,首先将沙发释放
        up(sofa_id);
        printf("consumer %d ready to get hair cut\n",cid);
        down(cut_trans_mux_id);// 相互之间互斥传输数据
        *consumer_id_shm_content = cid;//告诉理发师自己的id
        //sleep(sleep_time);
        up(call_barber_id);//唤醒一个理发师
        down(call_consumer_id);//等待理发师传输理发师id
        int bid = *barber_id_shm_content;
        up(cut_trans_mux_id);//传输结束
        printf("consumer %d find barber %d\n",cid,bid);
        down(wait_cut_finish_id[bid]);//等待理发师结束理发
        //理发结束,开始付款
        printf("consumer %d cutting finish,wait pay for money\n",cid);
        down(bill_mux_id);//同一时间只能有一个人付款,开始互斥
        sleep(sleep_time);
        down(wait_pay_id); // 等待理发师收款
        printf("consumer %d finish payment\n",cid);
        up(bill_mux_id);
        //此时理发结束,也付完钱了
        up(cutting_cnt_id);//释放一个理发资源
        down(consumer_cnt_mux_id);
        (*consumer_cnt_shm_content) -=1;
        up(consumer_cnt_mux_id);
        printf("consumer %d leaves barber shop\n",cid);
    }
    else
    {
        // 离开理发店
        printf("consumer %d leaves barber shop for lack of seats\n",cid);
        up(consumer_cnt_mux_id);
    }
}

int main(int argc,char** argv)
{
    // if(argv[1]!=NULL)
    //     cid = atoi(argv[1]);
    // else
    //     cid = 1;
    // if(argc == 3)
    //     sleep_time = atoi(argv[2]);
    // else
    //     sleep_time = 4;
    sleep_time = 4;
    int pid = 0;
    int process_num = 30;
    init_sem_shm();
    for(int i=0;i<process_num;i++)
    {
        cid = i+1;
        pid = fork();
        if(pid == 0)
            break;
    }
    // srand(time(0));
    // int t= rand()%10;
    // printf("rand time:%d\n",t);
    // sleep(t);
    if(pid != 0)
    {
        int status;
        for(int i=0;i<process_num;i++)
            wait(&status);
        printf("all process exit\n");
        return 0;
    }
    consumer();
    return 0 ;
}

