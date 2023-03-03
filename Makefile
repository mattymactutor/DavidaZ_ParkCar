FILE = parkCar
LIBS= -lsfml-graphics -lsfml-window -lsfml-system -lm -lserial

make:
	g++ -g -o $(FILE) $(FILE).cpp $(LIBS)
	./$(FILE) 30 150