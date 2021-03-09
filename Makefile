all:	b r
b:
		g++ main.cpp -ggdb3 -lOpenCL -o main
r:
		./main < test.in
