## 逻辑顺序
### 消费者
```cpp
P(consumer_cnt)
//计算数量,如果不足20人就进入
V(consumer_cnt)
//此时就算进入了等待室
P(sofa) // 看看能不能进入sofa
//别人释放一个,就成功进入了sofa
P(cutting_cnt) //看看能不能进入真正的理发区
//进入了理发区
V(sofa) // 释放一个sofa空位
P(cut_trans_mux)
// 放入消费者自己的id
V(call_barber) // 进入理发区后通知理发师
P(call_consumer)//等待理发师相应
//读取理发师id
V(cut_trans_mux)//关闭互斥
P(wait_cut_finish_key[])//等待理发结束
//付款阶段
P(bill_mux)//同一时间只能有一个人付钱
P(wait_pay)//等待理发师收款
//付款结束
V(bill_mux)
//这个阶段已经结束了理发
V(cutting_cnt)//释放一个理发区
P(consumer_cnt)
//总人数-1
V(consumer_cnt)
```
### 理发师
```cpp
P(call_barber) //没有call时等待,有call时唤醒
//读取消费者的id
//将自己的id写入到缓冲区
V(call_consumer)//唤醒消费者
//sleep() 理发中
V(wait_cut_finish[])//唤醒消费者,提示理发结束
//等待收钱
P(bill_cnt)
//+1 没付钱的多一个
V(bill_cnt)

P(bill_cnt)
    while(cnt > 0)
        V(wait_pay)
        cnt-1;
V(bill_cnt)
```





