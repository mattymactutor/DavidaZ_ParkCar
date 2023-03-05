FILE = parkCar
LIBS= -lsfml-graphics -lsfml-window -lsfml-system -lm -lserial -lpthread

make:
	g++ -g -o $(FILE) $(FILE).cpp $(LIBS)
	./$(FILE) 30 150

arm:
	arm-linux-gnueabihf-g++ -o $(FILE)_arm $(FILE).cpp $(LIBS)