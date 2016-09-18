preparedata:  
	g++ -c src/preparedata.cpp -o preparedata.o
	g++ -c main.cpp -o main.o
	g++ -o preparedata main.o preparedata.o `pkg-config --cflags --libs opencv`   
clean:
	rm *.o
	rm preparedata
#preparedata.o :src/preparedata.cpp inc/preparedata.h
#	g++ -H  inc/preparedata.h -c src/preparedata.cpp 
#main.o : main.cpp 
#	g++ -c main.cpp -o main.o
