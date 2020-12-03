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
        printf("consumer enter waiting room\n");
        down(sofa_id); // 尝试从等待室进入沙发
        printf("consumer enter sofa room\n");
        down(cutting_cnt_id); // 看看能不能开始理发
        //开始理发,能开始理发,首先将沙发释放
        up(sofa_id);
        printf("consumer ready to get hair cut\n");
        down(cut_trans_mux_id);// 相互之间互斥传输数据
        *consumer_id_shm_content = cid;//告诉理发师自己的id
        up(call_barber_id);//唤醒一个理发师
        down(call_consumer_id);//等待理发师传输理发师id
        int bid = *barber_id_shm_content;
        printf("consumer %d find barber %d\n",cid,bid);
        down(wait_cut_finish_id[bid]);//等待理发师结束理发
        //理发结束,开始付款
        printf("cutting finish,wait pay for money\n");
        down(bill_mux_id);//同一时间只能有一个人付款,开始互斥
        down(wait_pay_id); // 等待理发师收款
        printf("finish payment\n");
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
        up(consumer_cnt_mux_id);
    }
}

int main(int argc,char** argv)
{
    if(argv[1]!=NULL)
        cid = atoi(argv[1]);
    else
        cid = 1;
    if(argc == 3)
        sleep_time = atoi(argv[2]);
    else
        sleep_time = 1;
    
    consumer();
}