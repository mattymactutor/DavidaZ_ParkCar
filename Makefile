FILE = parkCar
LIBS= -lsfml-graphics -lsfml-window -lsfml-system -lm -lserial -lpthread

make:
	g++ -g -o $(FILE) $(FILE).cpp $(LIBS)
	./$(FILE) 30 150 3

test:
	g++ -g -o testRotate testRotate.cpp $(LIBS)