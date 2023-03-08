FILE = parkCar
LIBS= -lsfml-graphics -lsfml-window -lsfml-system -lm -lserial -lpthread -lgbm

make:
	g++ -g -o $(FILE) $(FILE).cpp $(LIBS)
	./$(FILE) 30 130 4

test:
	g++ -g -o testRotate testRotate.cpp $(LIBS)