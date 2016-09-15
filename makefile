preparedate : preparedata.o
	g++ preparedata.o -o preparedata `pkg-config opencv --libs --cflags opencv` 
preparedata.o :preparedata.cpp preparedata.h
	g++ -c preparedata.cpp -H preparedata.h preparedata.o 
	
