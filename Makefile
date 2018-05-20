CC = g++
CXXFLAGS = -g -Wall
CCLINK = $(CC)
LFLAGS = -g -pthread
OBJS = main.o account.o ATM.o bank.o
RM = rm -f

all: Bank
Bank: $(OBJS)
	$(CCLINK) $(LFLAGS) -o Bank $(OBJS)

account.o: account.cpp account.h
ATM.o: ATM.cpp ATM.h account.h bank.h
bank.o: bank.cpp bank.h account.h ATM.h
main.o: main.cpp bank.h ATM.h

clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*
