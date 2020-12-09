#include "bridge.h"
using namespace std;
Sema::Sema(int id) {
    sem_id = id;
}

Sema::~Sema() { }

/*
    * 信号灯上的 down/up 操作
    * semid:信号灯数组标识符
    * semnum:信号灯数组下标
    * buf:操作信号灯的结构
*/
int Sema::down() {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_num = 0;
    // buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("down error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int Sema::up() {
    Sem_uns arg;
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_num = 0;
    // buf.sem_flg = SEM_UNDO;
    if ((semop(sem_id, &buf, 1)) < 0) {
        perror("up error ");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int Sema::get_sem_value()
{
    Sem_uns sem_arg;
    int value = semctl(sem_id,0,GETVAL,sem_arg);
    return value;
}

/* * 用于哲学家管程的互斥执行 */
Lock::Lock(Sema *s) {
    sema = s;
}
Lock::~Lock() {}

//上锁
void Lock::close_lock() {
    sema->down();
}
//开锁
void Lock::open_lock() {
    sema->up();
}

Condition::Condition(Sema *sm)
{
    sema = sm;
}

Condition::~Condition()
{}

void Condition::Wait(Lock *lock)
{
    cout<<"in wait\n";
    lock->open_lock();
    std::cout<<"condition wait open lock\n";
    sema->down();
    // std::cout<<"condition wait\n";
    lock->close_lock();
    std::cout<<"condition wait get lock\n";
}

void Condition::Signal()
{

    // std::cout<<"val: "<<sema->get_sem_value()<<std::endl;
    // if(sema->get_sem_value()<0)
    // {
        // std::cout<<"signal\n";
        cout<<"condition up\n";
        sema->up();
    // }
}

/*
int control::get_ipc_id(char *proc_file, key_t key) {
		#define BUFSZ 256
    FILE *pf;
    int i, j;
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
        if (atoi(colum) != key)
            continue;
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


int control::set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
  	Sem_uns sem_arg;
    //测试由 sem_key 标识的信号量是否已经建立
    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0 ) {
        //semget 新建一个信号灯,其标号返回到sem_id
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }
    }
    //设置信号量的初值
    sem_arg.val = sem_val;
    if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
        perror("semaphore set error");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}


char *control::set_shm(key_t shm_key, int shm_num, int shm_flg) {

    int i, shm_id;
    char *shm_buf;

    //测试由 shm_key 标识的共享内存区是否已经建立
    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0) {
        //shmget 新建 一个长度为 shm_num 字节的共享内存
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
        //shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < shm_num; i++)
            shm_buf[i] = 0; //初始为 0
    }
    //共享内存区已经建立,将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}

void control::start(int i,int cur)// 方向和当前的编号
{
    lock->close_lock();
    if(cur_direction == i)
        *run_count[i] ++ ;
    else
    {
        *wait_count[i] ++ ;
        std::cout<<"car "<<cur<<" waiting"<<std::endl;
        wait_queue[i]->Wait(lock);
        wait_count[i] -- ;
        run_count[i] ++ ;
    }
    lock->open_lock();
}

void control::finish(int i,int cur)
{
    lock->close_lock();
    cur_direction = i;
    if(*run_count[i]>1)
        run_queue[i]->Wait(lock);
    sleep(2);
    *run_count[i] -- ;
    if(*run_count[i]>0)
        run_queue[i]->Signal();
    for(int i =0;i<*wait_count[i];i++)
        wait_queue[i]->Signal();

    if (*run_count[i] == 0&&*wait_count[i] == 0)
    {
        cur_direction = -1;
        wait_queue[(i+1)%2]->Signal();
    }
    
     lock->open_lock();
}

control::control()
{
    int ipc_flg = IPC_CREAT | 0644;
    int rcnt_key = 200;
    int wcnt_key = 300;
    
    int rcnt_shm_key = 400;
    int wcnt_shm_key = 500;

    int lock_key = 600;
    Sema *sema;
    int sem_id;
    sem_id = set_sem(lock_key,0,ipc_flg);
    sema = new Sema(sem_id);
    lock = new Lock(sema);


}
*/

int fcfs::get_ipc_id(char *proc_file, key_t key) {
		#define BUFSZ 256
    FILE *pf;
    int i, j;
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
        if (atoi(colum) != key)
            continue;
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

int fcfs::set_sem(key_t sem_key, int sem_val, int sem_flg) {
    int sem_id;
  	Sem_uns sem_arg;
    //测试由 sem_key 标识的信号量是否已经建立
    if ((sem_id = get_ipc_id("/proc/sysvipc/sem", sem_key)) < 0 ) {
        //semget 新建一个信号灯,其标号返回到sem_id
        if ((sem_id = semget(sem_key, 1, sem_flg)) < 0) {
            perror("semaphore create error");
            exit(EXIT_FAILURE);
        }
    }
    //设置信号量的初值
    sem_arg.val = sem_val;
    if (semctl(sem_id, 0, SETVAL, sem_arg) < 0) {
        perror("semaphore set error");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}


char *fcfs::set_shm(key_t shm_key, int shm_num, int shm_flg) {

    int i, shm_id;
    char *shm_buf;

    //测试由 shm_key 标识的共享内存区是否已经建立
    if ((shm_id = get_ipc_id("/proc/sysvipc/shm", shm_key)) < 0) {
        //shmget 新建 一个长度为 shm_num 字节的共享内存
        if ((shm_id = shmget(shm_key, shm_num, shm_flg)) < 0) {
            perror("shareMemory set error");
            exit(EXIT_FAILURE);
        }
        //shmat 将由 shm_id 标识的共享内存附加给指针 shm_buf
        if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
            perror("get shareMemory error");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < shm_num; i++)
            shm_buf[i] = 0; //初始为 0
    }
    //共享内存区已经建立,将由 shm_id 标识的共享内存附加给指针 shm_buf
    if ((shm_buf = (char *)shmat(shm_id, 0, 0)) < (char *)0) {
        perror("get shareMemory error");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}

void fcfs::start(int i,int cur)
{
    lock->close_lock();
    cout<<"car "<<cur<<" in start locked\n";
    cout<<"rcnt: "<<*rcnt<<" wcnt: "<<*wcnt<<endl;
    if(*rcnt != 0 || *wcnt!=0)
    {
        *wcnt++;
        std::cout<<"car "<<cur<<" wait for road"<<std::endl;
        wait_queue->Wait(lock);
        *wcnt--;
    }
    *rcnt = 1;
    std::cout<<"car "<<cur<<" run in direction "<<i<<std::endl;
    lock->open_lock();
}

void fcfs::finish(int i,int cur)
{
    lock->close_lock();
    sleep(2);
    std::cout<<"cat "<<cur<<" finished"<<std::endl;
    if(*wcnt >0 )
        wait_queue->Signal();
    *rcnt = 0;
    lock->open_lock();
}

fcfs::fcfs()
{
    int ipc_flg = IPC_CREAT | 0644;
    int lock_key = 200;
    int wait_queue_key = 300;
    int rcnt_shm_key = 400;
    int wcnt_shm_key = 500;
    Sema *sema;
    int sem_id ;

    sem_id = set_sem(lock_key,1,ipc_flg);
    sema = new Sema(sem_id);
    lock = new Lock(sema);

    sem_id = set_sem(wait_queue_key,0,ipc_flg);
    sema = new Sema(sem_id);
    wait_queue = new Condition(sema);

    rcnt = (int *)set_shm(rcnt_shm_key,4,ipc_flg);
    wcnt = (int *)set_shm(wcnt_shm_key,4,ipc_flg);

}



 int main()
 {
    fcfs* cont;
    int pid = -1 ;
    cont= new fcfs();
    srand(time(0));

    for(int i=0;i<5;i++)
    {
        pid = fork();
        int rand_time = rand()%10;
        int direction = rand()%2;
        if(pid == 0)
        {
            while(1)
            {
                cont->start(direction,i);
                cont->finish(direction,i);
                sleep(rand_time);
                std::cout<<"one \n";
            }
        }
    }
    int status;
    for(int i=0;i<5;i++)
        wait(&status);
    return 0;
 }