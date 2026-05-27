#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Angle.hpp>
#include <iostream>


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "Tappa-04";
const unsigned window_width = 1200;
const unsigned window_height = 680;
const float max_frame_rate = 60;

// Player
const float player_speed = {400};

// Bullet
const float bullet_speed = {500};

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
    void draw (sf::RenderWindow& window);
    void setDirection (int direction);
    void move_player_left (float elapsed);
    void move_player_right (float elapsed);
    void move_player_up (float elapsed);
    void move_player_down (float elapsed);
};

struct Bullet{
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture texture;
    int direction;
    float speed;
    bool inAction;

    Bullet(int direction, sf::Vector2f posPlayer);
    void draw (sf::RenderWindow& window);
    void move_bullet (float elapsed);
};

struct Attack{
    std::vector<Bullet> bullets;

    Attack(){};
    void draw (sf::RenderWindow& window);
    void attack(int direction, sf::Vector2f posPlayer);
    void update(float elapsed);
};

struct State{
    Room room;
    Player player;
    Attack attack;
    bool focus;
    bool pause;

    bool move_player_left;
    bool move_player_right;
    bool move_player_up;
    bool move_player_down;
    
    int move_bullet_left;
    int move_bullet_right;
    int move_bullet_up;
    int move_bullet_down;

    State() : room(), 
              focus(false), 
              pause(true), 
              move_player_left(false), 
              move_player_right(false), 
              move_player_up(false), 
              move_player_down(false),
              move_bullet_left(0), 
              move_bullet_right(0), 
              move_bullet_up(0), 
              move_bullet_down(0) {}
    void draw (sf::RenderWindow& window);
    void update(float elapsed);
    void collision();
};


Room::Room () : sprite(texture){
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

Bullet::Bullet (int direction, sf::Vector2f posPlayer)
{
    this->direction = direction;
    
    inAction = true;
    texture = sf::Texture ("texture/bullet/bullet.png");
    size = (sf::Vector2f)texture.getSize();
    if(direction == 0){
        pos = {posPlayer.x, posPlayer.y + (size.y)};
    }
    else if(direction == 1){
        pos = {posPlayer.x, posPlayer.y - (size.y/2.f)};
    }
    else if(direction == 2){
        pos = {posPlayer.x - (size.x/2.f), posPlayer.y};
    }
    else if(direction == 3){
        pos = {posPlayer.x + (size.x), posPlayer.y};
    }
    speed = bullet_speed;
}

//////////
// Draw //
//////////

void Room::draw (sf::RenderWindow& window){
    window.draw(sprite);
}

void Player::draw (sf::RenderWindow& window){
    size = (sf::Vector2f)lastTexture.getSize();
    sf::RectangleShape p (size);
    p.setTexture(&lastTexture);
    p.setPosition(pos);
    window.draw(p);
}

void Bullet::draw (sf::RenderWindow& window){
    if(!inAction){
        return;
    }

    sf::RectangleShape bullet (size);
    bullet.setTexture(&texture);
    bullet.setPosition(pos);
    
    if(direction == 0){
        bullet.setRotation(sf::Angle(sf::degrees(-135.f)));
    }
    else if(direction == 1){
        bullet.setRotation(sf::Angle(sf::degrees(45.f)));
    }
    else if(direction == 2){
        bullet.setRotation(sf::Angle(sf::degrees(-45.f)));
    }
    else if(direction == 3){
        bullet.setRotation(sf::Angle(sf::degrees(135.f)));
    }
    
    window.draw(bullet);
}

void Attack::draw (sf::RenderWindow& window){
    for(auto& bullet : bullets){
        bullet.draw(window);
    }
}

void State::draw (sf::RenderWindow& window){
    room.draw(window);
    if(!pause){
        attack.draw(window);
    }
        player.draw(window);
}

////////////
// Update //
////////////
void Player::setDirection(int direction){
    if(direction == 0) lastTexture = textureLeft;
    if(direction == 1) lastTexture = textureRight;
    if(direction == 2) lastTexture = textureBack;
    if(direction == 3) lastTexture = textureFront;
}

void Player::move_player_left (float elapsed){
    pos.x -= speed * elapsed;
}

void Player::move_player_right (float elapsed){
    pos.x += speed * elapsed;
}

void Player::move_player_up (float elapsed){
    pos.y -= speed * elapsed;
}

void Player::move_player_down (float elapsed){
    pos.y += speed * elapsed;
}

void Bullet::move_bullet (float elapsed){
    if(direction == 0){
        pos.x -= speed * elapsed;
    }
    else if(direction == 1){
        pos.x += speed * elapsed;
    }
    else if(direction == 2){
        pos.y -= speed * elapsed;
    }
    else if(direction == 3){
        pos.y += speed * elapsed;
    }
}

void Attack::attack (int direction, sf::Vector2f posPlayer){
    bullets.push_back(Bullet(direction, posPlayer));
}

void Attack::update (float elapsed){
    for(auto& bullet : bullets){
        bullet.move_bullet(elapsed);
    }
    
}

void State::update (float elapsed){
    if(!pause){
        if(move_player_left){
            player.move_player_left(elapsed);
            player.setDirection(0);
        }
        if(move_player_right){
            player.move_player_right(elapsed);
            player.setDirection(1);
        }
        if(move_player_up){
            player.move_player_up(elapsed);
            player.setDirection(2);
        }
        if(move_player_down){
            player.move_player_down(elapsed);
            player.setDirection(3);
        }
        
        // Player attact
        for(move_bullet_left; move_bullet_left > 0; move_bullet_left--){
            attack.attack(0, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(move_bullet_right; move_bullet_right > 0; move_bullet_right--){
            attack.attack(1, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(move_bullet_up; move_bullet_up > 0; move_bullet_up--){
            attack.attack(2, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(move_bullet_down; move_bullet_down > 0; move_bullet_down--){
            attack.attack(3, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        attack.update(elapsed);
        collision();
    }
}

void State::collision (){
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

    // Player movement
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
    
    // Player attack (bullet movement)
    case sf::Keyboard::Scancode::Left:
        state.move_bullet_left ++;
        return;
    case sf::Keyboard::Scancode::Right:
        state.move_bullet_right ++;
        return;
    case sf::Keyboard::Scancode::Up:
        state.move_bullet_up ++;
        return;
    case sf::Keyboard::Scancode::Down:
        state.move_bullet_down ++;
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
        
    // Player movement
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

int main ()
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