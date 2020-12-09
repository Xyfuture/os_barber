head = bridge.h
srcs = bridge.cpp
objs = bridge.o 
opts = -w -g -c 
all: bridge rmipcs
bridge: $(objs)
	g++ $(objs) -o bridge
bridge.o: $(srcs) $(head)
	g++ $(opts) $(srcs) 
clean:
	rm bridge *.o
rmipcs:
	bash rmipcs.sh