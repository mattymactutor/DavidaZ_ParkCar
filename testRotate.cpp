#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;

Texture textureCar;
sf::RenderWindow *window;
Sprite car;
double rotationAmt = .0085;
Time elapsed;
Clock gameClock;

int main(int argc, char **argv)
{

    // window = new RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Car Game", sf::Style::Fullscreen);
    VideoMode desktop = VideoMode::getDesktopMode();
    int DESKTOP_HEIGHT = desktop.height; // / 2;
    int DESKTOP_WIDTH = desktop.width;   // / 2;
    printf("Desktop Width: %d   Height: %d\n", DESKTOP_WIDTH, DESKTOP_HEIGHT);
    window = new RenderWindow(sf::VideoMode(DESKTOP_WIDTH, DESKTOP_HEIGHT), "Car Game");
    window->setPosition(Vector2i(0, 0));

    textureCar.loadFromFile("car.png");
    // apply the png image to the sprite
    car.setTexture(textureCar);
    // it's too big make it half size
    car.setScale(0.2, 0.2);
    // by default this image will rotate around the top-left corner
    // fix it to be the center
    car.setOrigin(car.getTextureRect().width / 2, car.getTextureRect().height / 2);
    int carX = DESKTOP_WIDTH / 2;
    int carY = DESKTOP_HEIGHT / 2;
    car.setPosition(Vector2f(carX, carY));
    float angle = 90;
    car.setRotation(0);
    printf("Car placed at (%d,%d)\n", carX, carY);

    while (window->isOpen())
    {
        elapsed = gameClock.getElapsedTime();
        gameClock.restart();
        sf::Event event;
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            // window closed
            case sf::Event::Closed:
                window->close();
                break;
            }
        }

        car.setRotation(angle);
        angle += 0.01;

        window->clear();
        window->draw(car);
        window->display();
    }
}