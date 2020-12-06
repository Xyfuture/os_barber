#include "ipc.h"

int cid;
int sleep_time;


void consumer()
{
    sleep(sleep_time);
    down(consumer_cnt_mux_id);//检查理发店内人数,临界资源,需要互斥
    if(*consumer_cnt_shm_content <20)
    {
        //进入理发店
        (*consumer_cnt_shm_content) += 1;//人员增加
        up(consumer_cnt_mux_id);//使用结束,释放临界资源
        printf("consumer %d enter waiting room\n",cid);
        down(sofa_id); // 尝试从等待室进入沙发,如果等待说明沙发满,相当于在等待室等待
        sleep(sleep_time);
        printf("consumer %d enter sofa room\n",cid);
        down(cutting_cnt_id); //检查是否能够理发
        up(sofa_id);//开始理发,能开始理发,首先将沙发释放
        printf("consumer %d ready to get hair cut\n",cid);
        down(cut_trans_mux_id);//首先客户和理发师之间互相使用共享内存交换各自的id,这时访问临界资源,进行互斥
        *consumer_id_shm_content = cid;//告诉理发师自己的id
        up(call_barber_id);//唤醒一个理发师
        down(call_consumer_id);//等待理发师传输理发师id
        int bid = *barber_id_shm_content;//获取理发师id
        up(cut_trans_mux_id);//传输结束,释放临界资源
        printf("consumer %d find barber %d\n",cid,bid);
        down(wait_cut_finish_id[bid]);//等待理发师结束理发
        //理发结束,开始付款
        printf("consumer %d cutting finish,wait pay for money\n",cid);
        down(wait_pay_id); // 等待理发师收款
        printf("consumer %d finish payment\n",cid);

        //此时理发结束,也付完钱了
        up(cutting_cnt_id);//释放一个理发资源
        down(consumer_cnt_mux_id);//离开理发店,人数-1,互斥访问临界资源
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
    if (argc == 2)
        process_num = atoi(argv[1]);
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

