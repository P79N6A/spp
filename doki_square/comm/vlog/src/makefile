CXX=g++
Cc=gcc
CFLAGS=-fPIC -g

INC = -I ../include/

OBJS = VLog.o
output = libVLog.a

all: $(output)

.cpp.o:
	$(CXX) $(CFLAGS) $(INC) -c $(filter %.cpp,$^)

$(output): $(OBJS)
	ar -rs $@ $^
	cp $(output)  ../lib/ -rf

clean:
	rm *.o *.~ $(output) -rf

