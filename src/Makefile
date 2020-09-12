SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.c=.o)

minidecaf: $(OBJS)
	@ g++ -o $@ $(OBJS)
	@ mv $@ ../bin/$@

clean:
	@ rm ../bin/minidecaf