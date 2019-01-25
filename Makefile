all: simu_source/main.cpp
	g++ -std=c++14 -O3 simu_source/main.cpp -o simu

clean:
	rm -f simu
