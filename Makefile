FLAGS = -Wall
INCLUDES = ./include

all: main

clean:
	rm -f main

main: main.cpp
	g++ -I $(INCLUDES) -o main main.cpp $(FLAGS)
