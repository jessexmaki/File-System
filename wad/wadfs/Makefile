CC=g++ 
TARGET=wadfs
SOURCES=wadfs.cpp
CFLAGS=-O3
LFLAGS=-lm
CXXFLAGS= -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -g -Wall -MMD -std=c++11 

OBJSC=$(SOURCES:.c=.o)
OBJS=$(OBJSC:.cpp=.o)

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(TARGET) -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -L "../libWad" -lfuse -lWad

purge: clean
	rm -f $(TARGET)

clean:
	rm -f *.o *.d wadfs
