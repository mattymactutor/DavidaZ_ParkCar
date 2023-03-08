#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "stdio.h"
#include <math.h>
#include "Serial_Comm_SteeringWheel.h"
using namespace std;
using namespace sf;

#include "GameEngine.h"

// TODO
// I tried to make the steering turn it harder the further you turn but didn't work right away and I don't think it's necessary

// Documentation
// https://www.sfml-dev.org/tutorials/2.5/window-events.php

USB_Comm usb("/dev/ttyUSB0");

int main(int argc, char **argv)
{

    if (argc < 4)
    {
        cout << "No inputs, using default values" << endl;
        TIME_PER_GAME = 30;
        speed = 150;
        rotationAmt = 1;
    }
    else
    {

        TIME_PER_GAME = stoi(argv[1]);
        speed = stoi(argv[2]);
        rotationAmt = stoi(argv[3]);
    }
    cout << "TIME PER GAME: " << TIME_PER_GAME << endl;
    cout << "CAR SPEED: " << speed << endl;
    cout << "ROTATION: " << rotationAmt << endl;
    srand(time(NULL));

    usb.setParseFunc(parseUSBCommand);
    initGame();

    while (window->isOpen())
    {
        elapsed = gameClock.getElapsedTime();

        gameClock.restart();
        sf::Event event;
        while (window->pollEvent(event))
        {
            handleEvents(event);
            // capture clicks
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    printf(" ( %d, %d)\n", event.mouseButton.x, event.mouseButton.y);
                }
            }
        }

        // redraw every so often
        elapsed = clockDraw.getElapsedTime();
        if (elapsed.asMilliseconds() > 200)
        {
            clockDraw.restart();
            //printf("refresh!\n");

            if (mode == IN_GAME)
            {
                if (gamePlay())
                {
                    drawGameObjects();
                }
            }
            else if (mode == WIN)
            {
                win();
            }

            // change the screen on the main thread not in the thread for rec usb commands
            if (changeMode == START_MENU)
            {
                mode = START_MENU;
                // cant change the screen in this thread
                startMenu();
                changeMode = IDLE;
            }
            else if (changeMode == IN_GAME)
            {
                mode = IN_GAME;
                newGame();
                changeMode = IDLE;
            }
        }
    }

    return 0;
}
