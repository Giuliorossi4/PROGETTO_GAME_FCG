#include <SFML/Graphics.hpp>
#include <iostream>


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "Tappa-02";
const unsigned window_width = 1200;
const unsigned window_height = 680;
const float max_frame_rate = 60;

struct Room{
    sf::Texture texture;
    sf::Sprite sprite;

    Room ();
    void draw (sf::RenderWindow& window);
};

struct State{
    Room room;

    State() : room() {}
    void draw (sf::RenderWindow& window);
};

Room::Room() : sprite(texture){
    texture = sf::Texture("texture/floor/floorBlock01.png");
    texture.setRepeated(true);
    sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(window_width), static_cast<int>(window_height)}));
}

void Room::draw (sf::RenderWindow& window){
    window.draw(sprite);
}

void State::draw (sf::RenderWindow& window){
    room.draw(window);
}

////////////
// Events //
////////////

void handle_close (sf::RenderWindow& window)
{
    window.close();
}
void handle_resize (const sf::Event::Resized& resized, sf::RenderWindow& window)
{   // constrain aspect ratio and map always the same portion of the world
    sf::Vector2u ws = resized.size;
    window.setSize(ws);
}



//////////
// Loop //
//////////

int main()
{
    
    sf::RenderWindow window (sf::VideoMode({window_width, window_height}), window_title);
    window.setFramerateLimit (max_frame_rate);
    window.setMinimumSize(window.getSize());

    State state;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition({
        static_cast<int>((desktop.size.x - window.getSize().x) / 2),
        static_cast<int>((desktop.size.y - window.getSize().y) / 2)
    });

    while (window.isOpen())
    {
        // events
        window.handleEvents (
                             [&window](const sf::Event::Closed&) { handle_close (window); },
                             [&window](const sf::Event::Resized& event) { handle_resize (event, window); }
        );

        // display
        window.clear (sf::Color::Black);
        state.draw(window);
        window.display ();
    }
}