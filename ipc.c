/*
* Filename  : ipc.c
* copyright : (C) 2006 by zhonghonglie
* Function  : 一组建立 IPC 机制的函数
*/
#include "ipc.h"
/*
* get_ipc_id() 从/proc/sysvipc/文件系统中获取 IPC 的 id 号
* pfile: 对应/proc/sysvipc/目录中的 IPC 文件分别为
* msg-消息队列,sem-信号量,shm-共享内存
* key:  对应要获取的 IPC 的 id 号的键值
*/
int get_ipc_id(char *proc_file, key_t key) {
    FILE *pf; int i, j;
    char line[BUFSZ], colum[BUFSZ];

    if ((pf = fopen(proc_file, "r")) == NULL) {
        perror("Proc file not open");
        exit(EXIT_FAILURE);
    }
    fgets(line, BUFSZ, pf);
    while (!feof(pf)) {
        i = j = 0;
        fgets(line, BUFSZ, pf);
        while (line[i] == ' ') i++;
        while (line[i] != ' ') colum[j++] = line[i++];
        colum[j] = '\0';
        if (atoi(colum) != key) continue;
        j = 0;
        while (line[i] == ' ') i++;
        while (line[i] != ' ') colum[j++] = line[i++];
        colum[j] = '\0';
        i = atoi(colum);
        fclose(pf);
        return i;
    }
    fclose(pf);
    return -1;
}

/*
* 信号灯上的down/up 操作
* semid:信号灯数组标识符
* semnum:信号灯数组下标
* buf:操作信号灯的结构
*/
int down(int sem_id) {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_num = 0;
    buf.sem_flg =0;
    // buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("down error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int up(int sem_id) {
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_num = 0;
    buf.sem_flg = 0;
    // buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("up error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

/*
* set_sem 函数建立一个具有 n 个信号灯的信号量
* 如果建立成功，返回 一个信号灯数组的标识符 sem_id
* 输入参数：
* sem_key 信号灯数组的键值
* sem_val 信号灯数组中信号灯的个数
* sem_flag 信号等数组的存取权限
*/
int set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
    Sem_uns sem_arg;
    sem_arg.val = sem_val;
    //测试由 sem_key 标识的信号灯数组是否已经建立
    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0 ) {
        //semget 新建一个信号灯,其标号返回到 sem_id
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }

        //设置信号灯的初值sem_arg.val = sem_val;
        if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
            perror("semaphore set error");
            exit(EXIT_FAILURE);
        }
    }
    return sem_id;
}

/*
* set_shm 函数建立一个具有 n 个字节 的共享内存区
* 如果建立成功，返回 一个指向该内存区首地址的指针 shm_buf
* 输入参数：
* shm_key 共享内存的键值
* shm_val 共享内存字节的长度
* shm_flag 共享内存的存取权限
*/
char *set_shm(key_t shm_key, int shm_num, int shm_flg) {
    int i, shm_id;
    char *shm_buf;
    //测试由 shm_key 标识的共享内存区是否已经建立
    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0 ) {
        //shmget 新建 一个长度为 shm_num 字节的共享内存,其标号返回shm_id
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
            perror("shareMemory set error"); exit(EXIT_FAILURE);
        }
        //shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
            perror("get shareMemory error"); exit(EXIT_FAILURE);
        }
        for (i = 0; i < shm_num; i++) shm_buf[i] = 0; //初始为 0
    }
    //shm_key 标识的共享内存区已经建立,将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }

    return shm_buf;
}

/*
* set_msq 函数建立一个消息队列
* 如果建立成功，返回 一个消息队列的标识符 msq_id
* 输入参数：
* msq_key 消息队列的键值
* msq_flag 消息队列的存取权限
*/
int set_msq(key_t msq_key, int msq_flg) {
    int msq_id;
    //测试由 msq_key 标识的消息队列是否已经建立
    if ((msq_id = get_ipc_id("/proc/sysvipc/msg", msq_key)) < 0 ) {
        //msgget 新建一个消息队列,其标号返回到 msq_id
        if ((msq_id = msgget(msq_key, msq_flg)) < 0) {
            perror("messageQueue set error"); exit(EXIT_FAILURE);
        }
    }
    return msq_id;
}


void init_sem_shm()
{
    consumer_cnt_mux_key = 100;
    sofa_key = 200;
    cutting_cnt_key = 300;
    call_barber_key = 400;
    call_consumer_key= 410;
    for(int i=0;i<3;i++)
        wait_cut_finish_key[i] = 500 + 10*i;
    bill_mux_key = 600;
    wait_pay_key = 700;
    bill_cnt_key = 800;
    cut_trans_mux_key = 900;

    wait_pay_cnt_shm_key = 2000;
    consumer_id_shm_key = 2100;
    barber_id_shm_key = 2200;
    consumer_cnt_shm_key = 2300;

    //初始化信号量和共享内存

    int sem_flg = IPC_CREAT|0644;
    int sem_val = 1;
    consumer_cnt_mux_id = set_sem(consumer_cnt_mux_key,sem_val,sem_flg);
    bill_mux_id = set_sem(bill_mux_key,sem_val,sem_flg);
    bill_cnt_id = set_sem(bill_cnt_key,sem_val,sem_flg);
    cut_trans_mux_id = set_sem(cut_trans_mux_key,sem_val,sem_flg);

    sem_val = 0;
    wait_pay_id = set_sem(wait_pay_key,sem_val,sem_flg);
    for(int i=0;i<3;i++)
        wait_cut_finish_id[i] = set_sem(wait_cut_finish_key[i],sem_val,sem_flg);
    call_barber_id=set_sem(call_barber_key,sem_val,sem_flg);
    call_consumer_id = set_sem(call_consumer_key,sem_val,sem_flg);

    sem_val = 4;
    sofa_id = set_sem(sofa_key,sem_val,sem_flg);

    sem_val = 3;
    cutting_cnt_id = set_sem(cutting_cnt_key,sem_val,sem_flg);

    int shm_flg = IPC_CREAT|0644;
    int shm_num = 4;
    wait_pay_cnt_shm_content = (int*) set_shm(wait_pay_cnt_shm_key,shm_num,shm_flg);
    consumer_id_shm_content = (int*) set_shm(consumer_id_shm_key,shm_num,shm_flg);
    barber_id_shm_content = (int*) set_shm(barber_id_shm_key,shm_num,shm_flg);
    consumer_cnt_shm_content = (int*) set_shm(consumer_cnt_shm_key,shm_num,shm_flg);
    srand(time(0));
}