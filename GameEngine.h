#include <SFML/Graphics.hpp>
#include <vector>
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
using namespace sf;
using namespace std;

enum MODE
{
    START_MENU,
    IN_GAME,
    GAME_OVER,
    WIN,
    IDLE
};

MODE mode = START_MENU;
MODE changeMode = IDLE;

Font arial;
int sizeTitle = 60;
int sizeText = 40;
// GAME PARAMETERS
int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 700;
int DESKTOP_WIDTH = 1919;
int DESKTOP_HEIGHT = 1079;

int TIME_PER_GAME = -1; // PUT THIS BACK POSITIVE
// CAR MOVEMENT
double angle = 90;
int speed = 150;
double rotationAmt = .0085;
double leftRotateMultiplier = 1, rightRotateMultiplier = 1;

Time elapsed, lastSecond;
int REDRAW_TIME = 200;
Clock gameClock, clockDraw, timeClock;
bool gasDown = false, rotateRight = false, rotateLeft = false, reverseDown = false;

// Game Objects
sf::RenderWindow *window;
// CAR
Sprite car;
Texture textureCar, tCoin;
RectangleShape walls, parkingSpot;
Vector2f velocity;
vector<Drawable *> drawObjects;
vector<Drawable *> coins;
int secs = 0;
// Gameplay text
sf::Text txtScore, txtTime;
// Win Text

void newGame();
void gameOver();
void setupText(Text &t, String msg, int size, int x, int y, Color c);
void drawGameObjects();
void parseUSBCommand(string msg);
bool checkCrash();
bool checkWin();
void startMenu();

void parseUSBCommand(string msg)
{
    printf("USB CMD: %s\n", msg.c_str());

    try
    {
        if (msg == "U")
        {

            rotateRight = false;
            rotateLeft = false;
        }
        else if (msg == "L")
        {
            rotateRight = false; // probably dont need this
            rotateLeft = true;
            // leftRotateMultiplier = val;
        }
        else if (msg == "R")
        {                       // turning right
            rotateRight = true; // probably dont need this
            rotateLeft = false;
            // rightRotateMultiplier = val;
        }
        else if (msg == "I")
        {
            gasDown = false;
            reverseDown = false;
        }
        else if (msg == "F")
        {                   // negative is forward
            gasDown = true; // probably dont need this
            reverseDown = false;
        }
        else if (msg == "B")
        {                    // backward
            gasDown = false; // probably dont need this
            reverseDown = true;
        }
        else if (msg == "b")
        {
            if (mode == GAME_OVER || mode == WIN)
            {
                changeMode = START_MENU;
                // cant change the screen in this thread
                // startMenu();
            }
            else if (mode == START_MENU)
            {
                changeMode = IN_GAME;
                // newGame();
            }
        }
    }
    catch (std::invalid_argument e)
    {
        printf("Could not parse USB msg: %s\n", e.what());
    }
}

void newGame()
{
    drawObjects.clear();
    // Init Car
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
    angle = 90;
    car.setRotation(0);
    printf("Car placed at (%d,%d)\n", carX, carY);

    // center the car in the screen
    /* Vector2f pos = car.getPosition();
     pos.x += (DESKTOP_WIDTH - SCREEN_WIDTH) / 2;
     pos.y += (DESKTOP_HEIGHT - SCREEN_HEIGHT) / 2;
     car.setPosition(pos);*/

    // put the outside walls on
    walls.setSize(Vector2f(SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40));
    walls.setPosition((DESKTOP_WIDTH / 2) - (SCREEN_WIDTH / 2), (DESKTOP_HEIGHT / 2) - (SCREEN_HEIGHT / 2));
    walls.setFillColor(Color(0, 0, 0));
    walls.setOutlineThickness(20);
    walls.setOutlineColor(Color(255, 0, 0));
    // prepend so it is the first thing drawn

    // put the parking space in
    parkingSpot.setSize(Vector2f(car.getGlobalBounds().width + 30, car.getGlobalBounds().height + 30));

    // pick a random spot within the walls
    FloatRect wallsBounds = walls.getGlobalBounds();
    int randX = (rand() % (int)(wallsBounds.width - 50)) + wallsBounds.left;
    int randY = (rand() % (int)(wallsBounds.height - 50)) + wallsBounds.top;
    // shift into the center of the screen;
    // randX += (DESKTOP_WIDTH / 2) - (SCREEN_WIDTH / 2);
    // randY += (DESKTOP_HEIGHT / 2) - (SCREEN_HEIGHT / 2);
    printf("Parking spot placed at (%d, %d)\n", randX, randY);
    parkingSpot.setPosition(randX, randY);
    parkingSpot.setFillColor(Color(0, 255, 0));
    parkingSpot.setOutlineColor(Color(255, 255, 255));
    parkingSpot.setOutlineThickness(2);
    parkingSpot.setRotation(rand() % 180);

    // detect if the parking spot is touching the red
    FloatRect pB = parkingSpot.getGlobalBounds();
    Vector2f pPos = parkingSpot.getPosition();
    printf("Parking Bounds top:%.0f left:%.0f bottom:%.0f right:%.0f\n", pB.top, pB.left, pB.top + pB.height, pB.left + pB.width);
    // check left side
    if (pB.left < wallsBounds.left)
    {
        pPos.x += pB.width + 60;
        printf("shifted right..\n");
    }
    if (pB.top < wallsBounds.top)
    {
        pPos.y += pB.height;
        printf("shifted down..\n");
    }
    if (pB.left + pB.width > wallsBounds.left + wallsBounds.width)
    {
        pPos.x -= pB.width - 60;
        printf("shifted left..\n");
    }
    if (pB.top + pB.height > wallsBounds.top + wallsBounds.height)
    {
        pPos.y -= pB.height - 60;
        printf("shifted up..\n");
    }
    parkingSpot.setPosition(pPos);

    // add in the order they should be drawn
    drawObjects.push_back(&walls);
    drawObjects.push_back(&parkingSpot);
    drawObjects.push_back(&car);

    // put the score on

    secs = TIME_PER_GAME;
    setupText(txtTime, "Time: " + to_string(secs), sizeText, -1, (int)(DESKTOP_HEIGHT * 0.1), Color::White);
    txtTime.setStyle(sf::Text::Bold);
    drawObjects.push_back((&txtTime));

    gameClock.restart();
    timeClock.restart();

    rotateLeft = false;
    rotateRight = false;
    gasDown = false;
    reverseDown = false;

    printf("...New Game...\n");
}

void startMenu()
{
    window->clear();

    Text title;
    setupText(title, "Park The Car", sizeTitle, -1, (int)(DESKTOP_HEIGHT * 0.25), Color::White);
    window->draw(title);

    Text description;
    int y = DESKTOP_HEIGHT * 0.5;
    setupText(description, "Drive the car until it fits inside the green box", sizeText, -1, y, Color::White);
    window->draw(description);

    Text descr;
    setupText(descr, "Press [RED BUTTON] to play", sizeText, -1, (int)(DESKTOP_HEIGHT * 0.6), Color::White);

    window->draw(descr);
    window->display();
    printf("DREW START MENU\n");
}

void win()
{
    window->clear();

    Texture textReward;
    textReward.loadFromFile("trophy.png");
    Sprite trophy;
    trophy.setTexture(textReward);
    trophy.setScale(0.5, 0.5);
    // by default this image will rotate around the top-left corner
    // fix it to be the center
    trophy.setOrigin(trophy.getTextureRect().width / 2, trophy.getTextureRect().height / 2);
    trophy.setPosition(Vector2f(DESKTOP_WIDTH / 2, DESKTOP_HEIGHT / 3));

    Text congrats;
    setupText(congrats, "You won!", sizeTitle, -1, (int)(DESKTOP_HEIGHT * 0.5), Color::White);
    window->draw(congrats);

    Text descr;
    setupText(descr, "Press [RED BUTTON] to replay", sizeText, -1, (int)(DESKTOP_HEIGHT * 0.6), Color::White);

    window->draw(trophy);
    window->draw(descr);
    window->display();
}

void winEvents(Event event)
{
    switch (event.type)
    {
    // window closed
    case sf::Event::Closed:
        window->close();
        break;
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::A)
        {
            mode = START_MENU;
            startMenu();
        }
        break;
    }
}

void initGame()
{

    // window = new RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Car Game", sf::Style::Fullscreen);
    VideoMode desktop = VideoMode::getDesktopMode();
    DESKTOP_HEIGHT = desktop.height;// / 2;
    DESKTOP_WIDTH = desktop.width;// / 2;
    printf("Desktop Width: %d   Height: %d\n", DESKTOP_WIDTH, DESKTOP_HEIGHT);
    window = new RenderWindow(sf::VideoMode(DESKTOP_WIDTH, DESKTOP_HEIGHT), "Car Game", Style::None);
    window->setPosition(Vector2i(0, 0));
    // load font
    if (!arial.loadFromFile("arial.ttf"))
    {
        printf("COULD NOT LOAD FONT\n");
        exit(1);
    }

    mode = START_MENU;
    startMenu();
}

void gamePlayEvents(Event event)
{
    /*switch (event.type)
    {
    // window closed
    case sf::Event::Closed:
        window->close();
        break;

    // key pressed
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Left)
        {
            rotateLeft = true;
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            rotateRight = true;
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            reverseDown = true;
        }
        else if (event.key.code == sf::Keyboard::Up)
        {
            gasDown = true;
        }
        else if (event.key.code == Keyboard::Escape)
        {
            window->close();
        }
        else if (event.key.code == Keyboard::N)
        {
            newGame();
        }
        break;
    case sf::Event::KeyReleased:
        if (event.key.code == sf::Keyboard::Up)
        {
            gasDown = false;
        }
        else if (event.key.code == sf::Keyboard::Right)
        {
            rotateRight = false;
        }
        else if (event.key.code == sf::Keyboard::Left)
        {
            rotateLeft = false;
        }
        else if (event.key.code == sf::Keyboard::Down)
        {
            reverseDown = false;
        }
        break;
    // we don't process other types of events
    default:
        break;
    }*/
}

bool gamePlay()
{

    //--- Update the timer ---
    lastSecond = timeClock.getElapsedTime();
    if (lastSecond.asMilliseconds() > 1000)
    {
        timeClock.restart();

        secs--;
        txtTime.setString("Time: " + to_string(secs));

        if (secs == 0)
        {
            gameOver();
            return false;
        }
    }

    // handle the key events
    if (gasDown)
    {
        // rocket.move(movement);
        velocity.x = sin((M_PI / 180) * angle) * speed * elapsed.asSeconds();
        velocity.y = cos((M_PI / 180) * angle) * speed * elapsed.asSeconds();

        car.setPosition(car.getPosition() + velocity);
    }
    else if (reverseDown)
    {
        // rocket.move(movement);
        velocity.x = sin((M_PI / 180) * angle) * speed * elapsed.asSeconds();
        velocity.y = cos((M_PI / 180) * angle) * speed * elapsed.asSeconds();

        car.setPosition(car.getPosition() - velocity);
    }

    if (rotateRight)
    {

        if (gasDown)
        {
            //the car faces right so the angle should be 90 but the actual image rotation is 0
            car.setRotation(-(angle-90));
            angle -= rotationAmt;
            //cout << angle << endl;
        }
        else if (reverseDown)
        {
            car.setRotation(-(angle-90));
            angle += rotationAmt;
           // cout << angle << endl;
        }
    }
     if (rotateLeft)
    {

        if (gasDown)
        {
            //the car faces right so the angle should be 90 but the actual image rotation is 0
            car.setRotation(-(angle-90));
            angle += rotationAmt;
            //cout << angle << endl;
        }
        else if (reverseDown)
        {
            car.setRotation(-(angle-90));
            angle -= rotationAmt;
            //cout << angle << endl;
        }
    }
  

    if (checkCrash())
    {
        gameOver();
        return false;
    }

    // check for a win being inside of the green box
    FloatRect parkingBounds = parkingSpot.getGlobalBounds();
    FloatRect carBounds = car.getGlobalBounds();
    int rightX = carBounds.left + carBounds.width;
    int bottomY = carBounds.top + carBounds.height;

    bool inBounds = true;

    // printf("Pts in: ");

    //  check top left of car with in
    if (!parkingBounds.contains(carBounds.left, carBounds.top))
    {
        // printf("Top Left");
        inBounds = false;
    }

    if (!parkingBounds.contains(carBounds.left, bottomY))
    {
        // printf(" Bottom Left");
        inBounds = false;
    }

    if (!parkingBounds.contains(rightX, carBounds.top))
    {
        // printf(" Top Right");
        inBounds = false;
    }

    if (!parkingBounds.contains(rightX, bottomY))
    {
        // printf(" Bottom right");
        inBounds = false;
    }

    if (inBounds)
    {
        mode = WIN;
        printf("WIN MODE\n");
    }

    // printf("\n");
    return true;
}

bool checkCrash()
{
    // Check for off screen crash
    FloatRect carBounds = car.getGlobalBounds();
    FloatRect wallB = walls.getGlobalBounds();
    // account for it being centered on the screen
    int padding = 0;
    if (carBounds.top < wallB.top + padding)
    {
        printf("TOP SCREEN CRASH\n");
        // gameOver();
        return true;
    }
    if (carBounds.left < wallB.left + padding)
    {
        printf("LEFT SCREEN CRASH\n");
        // gameOver();
        return true;
    }
    if ((carBounds.top + carBounds.height) > (wallB.top + wallB.height) - padding)
    {
        printf("BOTTOM SCREEN CRASH\n");
        // gameOver();
        return true;
    }
    if ((carBounds.left + carBounds.width) > (wallB.left + wallB.width))
    {
        printf("RIGHT SCREEN CRASH\n");
        // gameOver();
        return true;
    }
    return false;
}

void startMenuEvents(Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == Keyboard::Escape)
        {
            window->close();
        }
        else if (event.key.code == Keyboard::A)
        {
            // start a new game
            mode = IN_GAME;
            newGame();
            printf("IN GAME MODE\n");
        }
    }
}

void gameOverEvents(Event event)
{
    switch (event.type)
    {
    // window closed
    case sf::Event::Closed:
        window->close();
        break;
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::A)
        {
            mode = START_MENU;
            startMenu();
        }
        else if (event.key.code == Keyboard::Escape)
        {
            window->close();
        }
        break;
    }
}

void setupText(Text &t, String msg, int size, int x, int y, Color c)
{
    t.setFont(arial); // font is a sf::Font
    // set the string to display
    t.setString(msg);
    // set the character size
    t.setCharacterSize(size); // in pixels, not points!
    // set the color
    t.setFillColor(c);
    // set the text style
    // t.setStyle(sf::Text::Bold | sf::Text::Underlined);
    int xCenter = (DESKTOP_WIDTH / 2) - (t.getGlobalBounds().width / 2);
    int yCenter = (DESKTOP_HEIGHT / 2) - (t.getGlobalBounds().height / 2);
    // printf("%f\n", t.getGlobalBounds().width);
    if (x == -1)
    {
        x = xCenter;
    }
    if (y == -1)
    {
        y = yCenter;
    }
    t.setPosition(x, y);
}

void gameOver()
{
    window->clear();

    sf::Text gameOverText;
    // set the text style
    gameOverText.setStyle(sf::Text::Bold | sf::Text::Underlined);

    setupText(gameOverText, "GAME OVER", sizeTitle, -1, (int)(DESKTOP_HEIGHT * 0.25), Color::White);

    Text descr;
    setupText(descr, "Press [RED BUTTON] to replay", sizeText, -1, (int)(DESKTOP_HEIGHT * 0.6), Color::White);

    window->draw(gameOverText);
    window->draw(descr);
    window->display();

    mode = GAME_OVER;
    printf("GAME OVER MODE\n");
}

void drawGameObjects()
{
    window->clear();
    // redraw every object
    for (int i = 0; i < drawObjects.size(); i++)
    {
        window->draw(*drawObjects[i]);
    }

    window->display();
}

void handleEvents(Event event)
{
    //esc key
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == Keyboard::Escape)
        {
            window->close();
        }
    }

    
    if (mode == START_MENU)
    {
        startMenuEvents(event);
    }
    else if (mode == IN_GAME)
    {
        gamePlayEvents(event);
    }
    else if (mode == GAME_OVER)
    {
        gameOverEvents(event);
    }
    else if (mode == WIN)
    {
        winEvents(event);
    }
}