#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

int OSCREEN_WIDTH = 128;
int OSCREEN_HEIGHT = 8;
int ZOOM = 10;
int SCREEN_WIDTH = ZOOM*OSCREEN_WIDTH;
int SCREEN_HEIGHT = ZOOM*OSCREEN_HEIGHT;

int main()
{
    ifstream fin("palRam.bin");
    if (!fin)
        exit(-1);

    //Tile data
    uint8_t tile[0x400];
    char c;
    for (int i = 0; i < 0x400; i++)
    {
        fin.get(c);
        tile[i] = c;
    }
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Palette Map");
    window.setPosition(sf::Vector2i(0,0));

    sf::VertexArray screen(sf::Triangles, SCREEN_WIDTH*SCREEN_HEIGHT*6);
    for (int i = 0; i < OSCREEN_WIDTH; i++)
    {
        for (int j = 0; j < OSCREEN_HEIGHT; j++)
        {
            sf::Vertex* triangles = &screen[(i + j * OSCREEN_WIDTH) * 6];

            int startX = i*ZOOM;
            int startY = j*ZOOM;
            int endX = (i+1)*ZOOM;
            int endY = (j+1)*ZOOM;

            // define the position of the triangle's points
            triangles[0].position = sf::Vector2f(startX, startY);
            triangles[1].position = sf::Vector2f(endX, startY);
            triangles[2].position = sf::Vector2f(startX, endY);
            triangles[3].position = sf::Vector2f(startX, endY);
            triangles[4].position = sf::Vector2f(endX, startY);
            triangles[5].position = sf::Vector2f(endX, endY);

            uint16_t value = tile[2*j + i*OSCREEN_HEIGHT];
            value |= tile[2*j + i*OSCREEN_HEIGHT + 1] << 8;
            uint8_t red = (value & 0x1F) << 3; red|=7;
            uint8_t green = (value & 0x3E0) >> 2;green|=7;
            uint8_t blue = (value & 0x7C00) >> 7;blue|=7;

            // define the color of the triangle's points
            sf::Color color = sf::Color(red, green, blue, 0xFF);
            triangles[0].color = color;
            triangles[1].color = color;
            triangles[2].color = color;
            triangles[3].color = color;
            triangles[4].color = color;
            triangles[5].color = color;
        }
        
    }
    


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(screen);
        window.display();
    }

    return 0;
}
