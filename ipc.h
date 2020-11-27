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



#define BUFSZ	256
//建立或获取 ipc 的一组函数的原型说明
int get_ipc_id(char *proc_file,key_t key);

char *set_shm(key_t shm_key,int shm_num,int shm_flag); 
int set_msq(key_t msq_key,int msq_flag);
int set_sem(key_t sem_key,int sem_val,int sem_flag); 
int down(int sem_id);
int up(int sem_id);

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

int cutting_cnt_key;
int cutting_cnt_id;

int call_barber_key;
int call_barber_id;

int wait_cut_finish_key;
int wait_cnt_finish_id;

int bill_mux_key;//使用这个实现一次贡献内存的保护
int bill_mux_id;

int bill_cnt_key;
int bill_cnt_id;

int wait_pay_key;
int wait_pay_id;

int trans_key;
int trans_id;



int pay_shm_id;
int* pay_shm_content;

int cut_shm_id;
int* cut_shm_content;


