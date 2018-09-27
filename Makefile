all: main.cpp
	g++ -std=c++14 main.cpp -o simu
clean:
	rm simu
