#include <SFML/Graphics.hpp>
#include <iostream>


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "Tappa-03";
const unsigned window_width = 1200;
const unsigned window_height = 680;
const float max_frame_rate = 60;

// Player
const float player_speed = {400};

struct Room{
    sf::Texture texture;
    sf::Sprite sprite;

    Room ();
    void draw (sf::RenderWindow& window);
};

struct Player{
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture lastTexture;
    sf::Texture textureFront;
    sf::Texture textureBack;
    sf::Texture textureLeft;
    sf::Texture textureRight;
    float speed;

    Player ();
    void draw (int direction, sf::RenderWindow& window);
    void move_left (float elapsed);
    void move_right (float elapsed);
    void move_up (float elapsed);
    void move_down (float elapsed);
};


struct State{
    Room room;
    Player player;
    bool focus;
    bool pause;
    bool move_player_left;
    bool move_player_right;
    bool move_player_up;
    bool move_player_down;

    State() : room(), 
              focus(false), 
              pause(true), 
              move_player_left(false), 
              move_player_right(false), 
              move_player_up(false), 
              move_player_down(false) {}
    void draw (sf::RenderWindow& window);
    void update(float elapsed);
    void collision();
};


Room::Room() : sprite(texture){
    texture = sf::Texture("texture/floor/floorBlock01.png");
    texture.setRepeated(true);
    sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(window_width), static_cast<int>(window_height)}));
}

Player::Player ()
{
    textureFront = sf::Texture ("texture/front/front.png");
    textureBack = sf::Texture ("texture/back/back.png");
    textureLeft = sf::Texture ("texture/left/left.png");
    textureRight = sf::Texture ("texture/right/right.png");
    lastTexture = textureFront;

    size = (sf::Vector2f)lastTexture.getSize();
    float player_px = ((float) window_width / 2.0) - (size.x / 2.0);
    float player_py = ((float) window_height/2.0) - size.y;
    pos = {player_px, player_py};
    
    speed = player_speed;
}

//////////
// Draw //
//////////

void Room::draw (sf::RenderWindow& window){
    window.draw(sprite);
}

void Player::draw (int direction, sf::RenderWindow& window){
    if(direction == 0){
        lastTexture = textureLeft;
    }
    else if(direction == 1){
        lastTexture =  textureRight;
    }
    else if(direction == 2){
        lastTexture = textureBack;
    }
    else if(direction == 3){
        lastTexture = textureFront;
    }
    size = (sf::Vector2f)lastTexture.getSize();
    sf::RectangleShape p (size);
    p.setTexture(&lastTexture);
    p.setPosition(pos);
    window.draw(p);
}


void State::draw (sf::RenderWindow& window){
    room.draw(window);
    if(!pause){
        if(move_player_left)
            player.draw(0, window);
        else if(move_player_right)
            player.draw(1, window);
        else if(move_player_up)
            player.draw(2, window);
        else if(move_player_down)
            player.draw(3, window);
    }
        player.draw(-1, window);
}

////////////
// Update //
////////////
void Player::move_left (float elapsed){
    pos.x -= speed * elapsed;
}

void Player::move_right (float elapsed){
    pos.x += speed * elapsed;
}

void Player::move_up (float elapsed){
    pos.y -= speed * elapsed;
}

void Player::move_down (float elapsed){
    pos.y += speed * elapsed;
}

void State::update (float elapsed){
    if(!pause){
        if(move_player_left)
            player.move_left(elapsed);
        if(move_player_right)
            player.move_right(elapsed);
        if(move_player_up)
            player.move_up(elapsed);
        if(move_player_down)
            player.move_down(elapsed);
        
        collision();
    }
}

void State::collision(){
    if(player.pos.x < 0.f){
        player.pos.x = 0.f;
    }
    if(player.pos.y < 0.f){
        player.pos.y = 0.f;
    }
    if(player.pos.x + player.size.x > window_width){
        player.pos.x = window_width - player.size.x;
    }
    if(player.pos.y + player.size.y > window_height){
        player.pos.y = window_height - player.size.y;
    }
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


template <typename T>
void handle (T& event, State& state) {}

void handle (const sf::Event::KeyPressed& key, State& state)
{
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scan::Enter:
        state.pause = !state.pause;
        return;
    case sf::Keyboard::Scan::A:
        state.move_player_left = true;
        return;
    case sf::Keyboard::Scan::D:
        state.move_player_right = true;
        return;
    case sf::Keyboard::Scan::W:
        state.move_player_up = true;
        return;
    case sf::Keyboard::Scan::S:
        state.move_player_down = true;
        return;
    default:
        return;
    }
}

void handle (const sf::Event::KeyReleased& key, State& state)
{
    if (!state.focus)
        return;

    switch (key.scancode) {
    case sf::Keyboard::Scan::A:
        state.move_player_left = false;
        return;
    case sf::Keyboard::Scan::D:
        state.move_player_right = false;
        return;
    case sf::Keyboard::Scan::W:
        state.move_player_up = false;
        return;
    case sf::Keyboard::Scan::S:
        state.move_player_down = false;
        return;
    default:
        return;
    }
}

void handle (const sf::Event::FocusGained&, State& state)
{
    state.focus = true;
}

void handle (const sf::Event::FocusLost&, State& state)
{
    state.pause = true;
    state.focus = false;
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
    sf::Clock clock;

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
                             [&window](const sf::Event::Resized& event) { handle_resize (event, window); },
                             [&state] (const auto& event) { handle (event, state); }
        );

        // update
        state.update (clock.restart().asSeconds());

        // display
        window.clear (sf::Color::Black);
        state.draw(window);
        window.display ();
    }
}