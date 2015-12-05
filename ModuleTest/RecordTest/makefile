debug:main.o table.o RecordManager.o
	g++ -o debug main.o table.o RecordManager.o
main.o:main.cpp Table.h RecordManager.h
	g++ -c main.cpp
table.o:table.cpp Table.h
	g++ -c table.cpp
RecordManager.o:RecordManager.cpp Table.h RecordManager.h
	g++ -c RecordManager.cpp
.PHONY:clean
clean:
	rm -f *.o
