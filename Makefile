all: resol
	rm preproc.o
	rm tseitin.o
	rm sat.o

resol: main.cpp preproc.o tseitin.o sat.o
	g++ -o resol main.cpp preproc.o tseitin.o sat.o

preproc.o: preproc.cpp preproc.h
	g++ -c preproc.cpp

tseitin.o: tseitin.cpp tseitin.h
	g++ -c tseitin.cpp

sat.o: sat.cpp sat.h
	g++ -c sat.cpp
