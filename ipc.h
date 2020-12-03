/*
*	Filename	:	ipc.h
*	copyright	: (C) 2006 by zhonghonglie
*	Function	: 声明 IPC 机制的函数原型和全局变量
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/sem.h> 
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <wait.h>

#define MAX_SLEEP_TIME 10;

#define BUFSZ	256
//建立或获取 ipc 的一组函数的原型说明
int get_ipc_id(char *proc_file,key_t key);

char *set_shm(key_t shm_key,int shm_num,int shm_flag); 
int set_msq(key_t msq_key,int msq_flag);
int set_sem(key_t sem_key,int sem_val,int sem_flag); 
int down(int sem_id);
int up(int sem_id);


void init_sem_shm();
/*信号灯控制用的共同体*/ 
typedef union semuns {
      int val;
} Sem_uns;

/* 消 息 结 构 体 */ 
typedef struct msgbuf {
      long mtype; 
      char mtext[1];
} Msg_buf;


int consumer_cnt_mux_key;
int consumer_cnt_mux_id;

int sofa_key;
int sofa_id;

int cutting_cnt_key;// 正在理发的情况
int cutting_cnt_id;

int call_barber_key;
int call_barber_id;

int call_consumer_key;
int call_consumer_id;

int wait_cut_finish_key[3];
int wait_cut_finish_id[3];

int bill_mux_key;//使用这个实现一次贡献内存的保护
int bill_mux_id;

int bill_cnt_key;// 理发师那一边计算有几个人理完发但是没付钱,这个用于互斥
int bill_cnt_id;

int wait_pay_key;
int wait_pay_id;

int cut_trans_mux_key;//传输id 互斥
int cut_trans_mux_id;

int wait_pay_cnt_shm_key ;
int* wait_pay_cnt_shm_content;//记录现在有几个人理完发了,但是还有没有支付

int consumer_id_shm_key;
int* consumer_id_shm_content;//将消费者的id传送给理发师

int barber_id_shm_key;
int* barber_id_shm_content;//将理发师的id传送给消费者

int consumer_cnt_shm_key;
int* consumer_cnt_shm_content;


