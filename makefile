preparedate : preparedata.o main.o
	g++ main.o  preparedata.o -o preparedata `pkg-config opencv --libs --cflags opencv` 
preparedata.o :src/preparedata.cpp inc/preparedata.h
	g++ -c src/preparedata.cpp -H inc/preparedata.h
main.o :main.cpp 
	g++ -c main.cpp
