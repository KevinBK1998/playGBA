#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(240, 160), "SFML works!");

    sf::VertexArray screen(sf::Triangles, 240*160*6);
    for (int i = 0; i < 240; i++)
    {
        for (int j = 0; j < 160; j++)
        {
            sf::Vertex* triangles = &screen[(i + j * 240) * 6];
            // define the position of the triangle's points
            triangles[0].position = sf::Vector2f(i, j);
            triangles[1].position = sf::Vector2f(i+1, j);
            triangles[2].position = sf::Vector2f(i, j+1);
            triangles[3].position = sf::Vector2f(i, j+1);
            triangles[4].position = sf::Vector2f(i+1, j);
            triangles[5].position = sf::Vector2f(i+1, j+1);

            // define the color of the triangle's points
            auto clearColor = sf::Color::White;
            triangles[0].color = clearColor;
            triangles[1].color = clearColor;
            triangles[2].color = clearColor;
            triangles[3].color = clearColor;
            triangles[4].color = clearColor;
            triangles[5].color = clearColor;
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