CC = g++
CFlAGS = -g -I. -c -Wall
LFLAGS = -g

SRCS = main.cpp http_get.cpp
OBJS = main.o http_get.o
EXE = 21127077_21127409
BUILD_DIR = .

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BUILD_DIR)/$(EXE)

main.o: main.cpp
	$(CC) $(CFlAGS) main.cpp -o main.o

http_get.o: http_get.cpp
	$(CC) $(CFlAGS) http_get.cpp -o http_get.o

run: all
	$(BUILD_DIR)/$(EXE) "www.google.com.vn/" "google.html"

clean:
	rm -f $(OBJS) $(BUILD_DIR)/$(EXE)