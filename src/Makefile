CC = g++
CFlAGS = -g -I. -c -Wall
LFLAGS = -g -Wall

SRCS = main.cpp http_get.cpp
OBJS = main.o http_get.o
EXE = 21127077_21127409_21127199
BUILD_DIR = .

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BUILD_DIR)/$(EXE)

main.o: main.cpp
	$(CC) $(CFlAGS) main.cpp -o main.o

http_get.o: http_get.cpp 
	$(CC) $(CFlAGS) http_get.cpp -o http_get.o

run: 
	echo "Run with: www.example.com/ output/example.html"
	make run_1
	echo "Run with: http://courses.skule.ca/course/ECE361H1 output/skule.html"
	make run_2
	echo "Run with: http://exams.skule.ca/exams/ECE361H1_20191_661555510989ECE361_Lab3.pdf output/lab3.pdf"
	make run_3
	echo "Run with: http://anglesharp.azurewebsites.net/Chunked output/azurewebsites.html"
	make run_4
	echo "Run with: http://www.google.com.vn output/googlevn.html"
	make run_5
	echo "Run with: http://www.bing.com output/bing.html"
	make run_6

run_1: all
	$(BUILD_DIR)/$(EXE) "www.example.com/" "output/example.html"

run_2: all
	$(BUILD_DIR)/$(EXE) "http://courses.skule.ca/course/ECE361H1" "output/skule.html"

run_3: all
	$(BUILD_DIR)/$(EXE) "http://exams.skule.ca/exams/ECE361H1_20191_661555510989ECE361_Lab3.pdf" "output/lab3.pdf"

run_4: all
	$(BUILD_DIR)/$(EXE) "http://anglesharp.azurewebsites.net/Chunked" "output/azurewebsites.html" 

run_5: all
	$(BUILD_DIR)/$(EXE) "http://www.google.com.vn" "output/googlevn.html"

run_6: all
	$(BUILD_DIR)/$(EXE) "http://www.bing.com" "output/bing.html"

clean:
	rm -f $(OBJS) $(BUILD_DIR)/$(EXE)