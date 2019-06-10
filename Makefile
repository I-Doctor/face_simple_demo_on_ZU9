CP = g++
CC = gcc
TARGET = process judge run
OBJS = process.o judge.o

DLIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui

all: $(TARGET) 

process: process.o
	$(CP) -o $@ $^ $(DLIBS)  

process.o: process.cpp
	$(CP) -o $@ -c $<

judge: judge.o
	$(CP) -o $@ $^ $(DLIBS)  

judge.o: judge.cpp
	$(CP) -o $@ -c $<

run: main.c
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)
