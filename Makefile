hdrs = ipc.h
opts = -g -c
c_src = consumer.c ipc.c 
c_obj = consumer.o ipc.o 
p_src = barber.c ipc.c 
p_obj = barber.o ipc.o
all: barber consumer rmipc
consumer:$(c_obj)
	gcc $(c_obj) -o consumer
consumer.o:	$(c_src) $(hdrs)
	gcc $(opts) $(c_src)
barber: $(p_obj)
	gcc $(p_obj) -o barber 
barber.o:	$(p_src) $(hdrs)
	gcc $(opts) $(p_src)
clean:
	rm consumer barber *.o
rmipc:
	bash rmipcs.sh