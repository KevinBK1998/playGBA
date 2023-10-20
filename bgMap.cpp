#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

uint8_t palette[0x400];

const int SCREEN_WIDTH_TILES = 40;

int TILE_WIDTH = 8;
int ZOOM = 2;
int SCREEN_WIDTH = 9*ZOOM*SCREEN_WIDTH_TILES;
int SCREEN_HEIGHT = 9*ZOOM*SCREEN_WIDTH_TILES;

void storeHalfWord(int address, uint16_t* var, uint8_t data){
    if (address){
        *(var) &= 0xFF;
        *(var) |= (data<<8);
        cout <<hex<<showbase<< "BGCNT: "<< *var  <<endl;
    }
    else {
        *(var) &= 0xFF00;
        *(var) |= data;
    }
}

sf::Color getFromPalette(int paletteNum){
    uint16_t value = palette[paletteNum*2];
    value |= palette[paletteNum*2 + 1] << 8;
    uint8_t red = (value & 0x1F) << 3; red|=7;
    uint8_t green = (value & 0x3E0) >> 2;green|=7;
    uint8_t blue = (value & 0x7C00) >> 7;blue|=7;
    return sf::Color(red, green, blue, 0xFF);
}

int main()
{
    ifstream bgCntIn("bgControl.bin");
    if (!bgCntIn)
        exit(-1);
    uint16_t bgControl[4];
    char c;
    for (int i = 0; i < 8; i++)
    {
        bgCntIn.get(c);
        storeHalfWord(i%2, &bgControl[i/2], c);
    }

    uint32_t tileBase = ((bgControl[3]>>2) & 0b11) * 0x4000;
    bool use256color = ((bgControl[3]>>7) & 1);
    // use256color = true;

    ifstream tileIn("vram.bin");
    if (!tileIn)
        exit(-1);

    //Tile data
    uint8_t* tile = new uint8_t[0x18000];
    for (int i = 0; i < 0x18000; i++)
    {
        tileIn.get(c);
        tile[i] = c;
    }

    ifstream fin("palRam.bin");
    if (!fin)
        exit(-1);

    //Palette data
    for (int i = 0; i < 0x400; i++)
    {
        fin.get(c);
        palette[i] = c;
    }
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "BG Map");
    window.setPosition(sf::Vector2i(0,0));

    sf::VertexArray screen(sf::Triangles, SCREEN_WIDTH*SCREEN_HEIGHT*6);

    int tileEnd = 767;
    if(use256color)
        tileEnd *= 2;

    for (int tileNum = 0; tileNum < tileEnd; tileNum++)
    {
        for (int y = 0; y < TILE_WIDTH; y++)
        {
            for (int x = 0; x < TILE_WIDTH; x++)
            {
                int dot = TILE_WIDTH*TILE_WIDTH*tileNum + TILE_WIDTH*y + x;
                uint8_t value = tile[tileBase + dot/2];
                if (dot & 1 != 0)
                    value &= 0xFF;
                if (dot & 1)
                    value >>= 4;
                if (use256color){
                    value = tile[tileBase + dot];
                }
                int xoff=(tileNum%SCREEN_WIDTH_TILES)*9,yoff=(tileNum/SCREEN_WIDTH_TILES)*9;
                // cout <<dec<< "tN: " << tileNum << ", x: " << x << ", y: " << y << ", dot: " << dot << ", v: " <<hex<< unsigned(value) << endl;
                // cout <<dec<< "xoff: " << xoff << ", yoff: " << yoff << endl;

                sf::Vertex* triangles = &screen[(dot) * 6];

                int startX = (xoff+x)*ZOOM;
                int startY = (yoff+y)*ZOOM;
                int endX = (xoff+x+1)*ZOOM;
                int endY = (yoff+y+1)*ZOOM;

                // define the position of the triangle's points
                triangles[0].position = sf::Vector2f(startX, startY);
                triangles[1].position = sf::Vector2f(endX, startY);
                triangles[2].position = sf::Vector2f(startX, endY);
                triangles[3].position = sf::Vector2f(startX, endY);
                triangles[4].position = sf::Vector2f(endX, startY);
                triangles[5].position = sf::Vector2f(endX, endY);

                // define the color of the triangle's points
                sf::Color color = getFromPalette(value);
                triangles[0].color = color;
                triangles[1].color = color;
                triangles[2].color = color;
                triangles[3].color = color;
                triangles[4].color = color;
                triangles[5].color = color;
            }
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

        window.clear(sf::Color::Red);
        window.draw(screen);
        window.display();
    }

    return 0;
}
