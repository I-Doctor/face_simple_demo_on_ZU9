CP = g++
CC = gcc
TARGET = process judge
TARGET2 = run
SRCS = process.cpp judeg.cpp
SRCS2 = main.c
OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.c=.o)

DLIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui

all: $(TARGET)$(TARGET2)

$(TARGET): %: %.o
	$(CP) -o $@ $^ $(DLIBS)  

$(OBJS): %.o: %.cpp
	$(CP) -o $@ -c $<

$(TARGET2): %: %.o
	$(CC) -o $@ $^   

$(OBJS2): %.o: %.c
	$(CC) -o $@ -c $<

clean:
	rm -rf $(TARGET) $(OBJS) $(TARGET2) $(OBJS2)
