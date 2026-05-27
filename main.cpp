#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Angle.hpp>
#include <iostream>
#include <string>
#include <random>


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "Tappa-09";
const unsigned window_width = 1200;
const unsigned window_height = 680;
const float max_frame_rate = 60;

// Room
sf::Vector2f roomPos;

// Player
const float player_speed = {400};

// Bullet
const float bullet_speed = {500};

// Life
const sf::Vector2f edge = {10.f, 10.f};
const int numLife = 3;

// Enemy
const float enemy_speed = {200};

struct Room{
    sf::Texture texture;
    sf::Sprite sprite;

    Room ();
    void draw (sf::RenderWindow& window);
};

struct Life{
    sf::Texture texture;
    sf::Vector2f pos;
    sf::Vector2f size;
    int life;

    Life ();
    void draw (sf::RenderWindow& window);
    void update ();
};

struct Round{
    const sf::Font font{"resources/font/LowresPixel-Regular.otf"};
    sf::Text text;
    sf::Vector2f pos;
    sf::Vector2f size;
    int num;

    Round ();
    void draw (sf::RenderWindow& window);
    void update ();
};

struct Hub{
    Life life;
    Round round;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f size;
    sf::Vector2f pos;

    Hub ();
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
    bool hit;

    Player ();
    void draw (sf::RenderWindow& window);
    void setDirection (int direction);
    void move_player_left (float elapsed);
    void move_player_right (float elapsed);
    void move_player_up (float elapsed);
    void move_player_down (float elapsed);
    void hitted (){ hit = true; }
};

struct Enemy{
    sf::RectangleShape shape;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture textureSpider;
    float speed;
    bool hitted;
    
    bool isWaiting;          
    float waitTimer;
    float WAIT_DURATION;         

    Enemy ();
    void draw (sf::RenderWindow& window);
    sf::Vector2f randomSpawnPoint ();
    bool move (float elapsed, Player& player);
    void hit(){ hitted = true; };
    void startWaiting();
};

struct Bullet{
    sf::RectangleShape shape;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture texture;
    int direction;
    float speed;
    bool inAction;

    Bullet (int direction, sf::Vector2f posPlayer);
    void draw (sf::RenderWindow& window);
    void move_bullet (float elapsed);
    bool hit (Enemy& enemy);
};

struct Attack{
    std::vector<Bullet> bullets;

    Attack (){};
    void draw (sf::RenderWindow& window);
    void attack (int direction, sf::Vector2f posPlayer);
    void update (float elapsed);
    void hit (Enemy& enemy);
    void collision (Hub Hub);
};

struct State{
    Room room;
    Hub hub;
    Player player;
    Attack attack;
    Enemy enemy;
    bool focus;
    bool pause;

    bool move_player_left;
    bool move_player_right;
    bool move_player_up;
    bool move_player_down;
    
    int num_bullet_left;
    int num_bullet_right;
    int num_bullet_up;
    int num_bullet_down;

    bool pressed_bullet_left;
    bool pressed_bullet_right;
    bool pressed_bullet_up;
    bool pressed_bullet_down;


    State() : room(),
              focus(false), 
              pause(true), 
              move_player_left(false), 
              move_player_right(false), 
              move_player_up(false), 
              move_player_down(false),
              num_bullet_left(0), 
              num_bullet_right(0), 
              num_bullet_up(0), 
              num_bullet_down(0),
              pressed_bullet_left(false),
              pressed_bullet_right(false),
              pressed_bullet_up(false),
              pressed_bullet_down(false) {}
    void draw (sf::RenderWindow& window);
    void update (float elapsed);
    void collision ();
};

/////////////////
// Constructor //
/////////////////

Room::Room () : sprite(texture){
    texture = sf::Texture("texture/floor/floorBlock01.png");
    texture.setRepeated(true);
    sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(window_width), static_cast<int>(window_height)}));
}

Player::Player (){
    hit = false;
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

Bullet::Bullet (int direction, sf::Vector2f playerCenter) : shape(size){
    this->direction = direction;
    inAction = true;
    
    texture = sf::Texture ("texture/bullet/bulletUpdated.png");
    size = (sf::Vector2f)texture.getSize();

    shape.setTexture(&texture);
    pos = {playerCenter.x, playerCenter.y};
    if(direction == 0){
        shape.setScale({-1.f,1.f});
        shape.setOrigin({0.f, size.y/2.f});
    }
    else if(direction == 1){
        shape.setOrigin({0.f,size.y/2.f});
    }
    else if(direction == 2){
        shape.rotate(sf::degrees(90.f));
        shape.setScale({-1.f,1.f});
        shape.setOrigin({0.f, size.y/2.f});
        
    }
    else if(direction == 3){
        shape.rotate(sf::degrees(90.f));
        shape.setScale({1.f,1.f});
        shape.setOrigin({0.f, size.y/2.f});
    }

    shape.setSize(size);
    shape.setPosition(pos);

    speed = bullet_speed;
}

Life::Life (){
    life = numLife;
    texture = sf::Texture("texture/life/heart.png");
    pos = edge;
    size = {(texture.getSize().x * life) + (edge.x * (life - 1)), (float)texture.getSize().y};
}

Round::Round () : text(font,"ROUND: ", 30){
    
    text.setPosition({(window_width / 2.f) - 100, edge.y});
    text.setFillColor(sf::Color::White);
    num = 1;
}

Hub::Hub () : sprite(texture){
    texture = sf::Texture("texture/floor/floorBlock02.png");
    texture.setRepeated(true);
    pos = {0,0};
    size = {window_width, life.texture.getSize().y + (edge.y*2.f)};
    sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, (sf::Vector2i)size));
    roomPos = {0, size.y};
}

Enemy::Enemy () : shape(size){
    hitted = false;
    isWaiting = false;
    waitTimer = 0.f;
    WAIT_DURATION = 0.5f;
    textureSpider = sf::Texture ("texture/enemy/spider.png");
    size = {textureSpider.getSize().x * 1.5f, textureSpider.getSize().y * 1.5f};
    pos = randomSpawnPoint();
    shape.setTexture(&textureSpider);
    shape.setPosition(pos);
    shape.setSize(size);
    speed = enemy_speed;
}

//////////
// Draw //
//////////

void Room::draw (sf::RenderWindow& window){
    window.draw(sprite);
}

void Hub::draw (sf::RenderWindow& window){
    window.draw(sprite);
    life.draw(window);
    round.draw(window);
}

void Player::draw (sf::RenderWindow& window){
    size = (sf::Vector2f)lastTexture.getSize();
    sf::RectangleShape shape(size);
    shape.setPosition(pos);
    shape.setTexture(&lastTexture);
    window.draw(shape);

    hit = false;
}

void Bullet::draw (sf::RenderWindow& window){
    if(inAction){
        shape.setTexture(&texture);
        shape.setPosition(pos);
        window.draw(shape);
    }
}

void Attack::draw (sf::RenderWindow& window){
    for(auto& bullet : bullets){
        bullet.draw(window);
    }
}

void Life::draw (sf::RenderWindow& window){
    sf::Vector2f posHeart = pos;
    sf::Sprite sprite(texture);
    for(int i = 0; i < life; i++){
        sprite.setPosition(posHeart);
        window.draw(sprite);
        posHeart.x += texture.getSize().x + (edge.x / 2.f);
    }
}


void Round::draw (sf::RenderWindow& window){
    sf::Text finalText = text;
    finalText.setString(text.getString() + "\t" + std::to_string(num));
    window.draw(finalText);
}

void Enemy::draw (sf::RenderWindow& window){
    if(!hitted){
        shape.setPosition(pos);
        window.draw(shape);
    }
}

void State::draw (sf::RenderWindow& window){
    room.draw(window);
    if(!pause){
        attack.draw(window);
    }
    player.draw(window);
    enemy.draw(window);
    hub.draw(window);
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

bool Bullet::hit (Enemy& enemy) {
    if (inAction && !enemy.hitted) {
        sf::FloatRect bulletBounds = this->shape.getGlobalBounds();
        sf::FloatRect enemyBounds = enemy.shape.getGlobalBounds();

        if (bulletBounds.findIntersection(enemyBounds)) {
            return true;
        }
    }
    return false;
}

void Attack::attack (int direction, sf::Vector2f posPlayer){
    bullets.push_back(Bullet(direction, posPlayer));
}

void Attack::update (float elapsed){
    for(auto& bullet : bullets){
        if(bullet.inAction){
            bullet.move_bullet(elapsed);
        }
    }
}

void Attack::hit (Enemy& enemy){
    for(auto& bullet : bullets){
        if(bullet.inAction){
            if(bullet.hit(enemy)){
                bullet.inAction = false;
                enemy.hit();
            }
        }
    }
}

void Attack::collision (Hub hub){
    for(auto & bullet : bullets){
        if(bullet.inAction){
            if(bullet.pos.x + bullet.size.x < 0.f){
                bullet.inAction = false;
            }
            if(bullet.pos.y + bullet.size.y < hub.size.y){
                bullet.inAction = false;
            }
            if(bullet.pos.x > window_width){
                bullet.inAction = false;
            }
            if(bullet.pos.y > window_height){
                bullet.inAction = false;
            }
        }
    }
}

sf::Vector2f Enemy::randomSpawnPoint (){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distrX(1,2);
    std::uniform_real_distribution<> distrY(roomPos.y, window_height - size.y);
    
    float X = 0.f;
    if(distrX(gen) == 1){
        X = 0.f;
    }
    else{
        X = window_width - size.x;
        shape.setScale({-1.f, 1.f});
        shape.setOrigin({size.x,0.f});
    }
    float Y = distrY(gen);

    return {X,Y};
}

void Enemy::startWaiting() {
    isWaiting = true;
    waitTimer = 0.f;
}

bool Enemy::move (float elapsed, Player& player){
    sf::Vector2f enemyCenter = {pos.x + (size.x / 2.f), pos.y + (size.y / 2.f)};
    sf::Vector2f playerCenter = {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)};

    sf::Vector2f distance = {enemyCenter.x - playerCenter.x, enemyCenter.y - playerCenter.y};
    float movement = (speed * elapsed);

    if (isWaiting) {
        waitTimer += elapsed;
        
        if (waitTimer >= WAIT_DURATION) {
            isWaiting = false;
        }
        
        return false;
    }

    if(std::abs(distance.x) > movement){
        if(enemyCenter.x < playerCenter.x){
            shape.setScale({1.f, 1.f});
            shape.setOrigin({0.f,0.f});
            pos.x += movement;
        }
        else if(enemyCenter.x > playerCenter.x){
            shape.setScale({-1.f, 1.f});
            shape.setOrigin({size.x,0.f});
            pos.x -= movement;
        }
    }

    if(std::abs(distance.y) > movement){
        if(enemyCenter.y < playerCenter.y){
            pos.y += movement;
        }
        else if(enemyCenter.y > playerCenter.y){
            pos.y -= movement;
        }
    }

    sf::FloatRect bulletBounds = shape.getGlobalBounds();
    sf::RectangleShape playerShape(player.size);
    playerShape.setPosition(player.pos);
    sf::FloatRect enemyBounds = playerShape.getGlobalBounds();

    if (bulletBounds.findIntersection(enemyBounds)) {
        player.hitted();
        return true;
    }

    return false;
}

void State::collision (){
    if(player.pos.x < 0.f){
        player.pos.x = 0.f;
    }
    if(player.pos.y < hub.size.y){
        player.pos.y = hub.size.y;
    }
    if(player.pos.x + player.size.x > window_width){
        player.pos.x = window_width - player.size.x;
    }
    if(player.pos.y + player.size.y > window_height){
        player.pos.y = window_height - player.size.y;
    }

    attack.hit(enemy);
    attack.collision(hub);
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
        for(; num_bullet_left > 0; num_bullet_left--){
            attack.attack(0, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_right > 0; num_bullet_right--){
            attack.attack(1, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_up > 0; num_bullet_up--){
            attack.attack(2, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_down > 0; num_bullet_down--){
            attack.attack(3, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        
        attack.update(elapsed);

        if(enemy.move(elapsed, player)){
            hub.life.life -= 1;
            enemy.startWaiting();
        }

        collision();
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
        if(!state.pressed_bullet_left){
            state.num_bullet_left ++;
            state.pressed_bullet_left = true;
        }
        return;
    case sf::Keyboard::Scancode::Right:
        if(!state.pressed_bullet_right){
            state.num_bullet_right ++;
            state.pressed_bullet_right = true;
        }
        return;
    case sf::Keyboard::Scancode::Up:
        if(!state.pressed_bullet_up){
            state.num_bullet_up ++;
            state.pressed_bullet_up = true;
        }
        return;
    case sf::Keyboard::Scancode::Down:
        if(!state.pressed_bullet_down){
            state.num_bullet_down ++;
            state.pressed_bullet_down = true;
        }
        return;
    
    case sf::Keyboard::Scan::C:
        state.enemy.hit();
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
    
    // Player attack (bullet movement)
    case sf::Keyboard::Scancode::Left:
        state.pressed_bullet_left = false;
        return;
    case sf::Keyboard::Scancode::Right:
        state.pressed_bullet_right = false;
        return;
    case sf::Keyboard::Scancode::Up:
        state.pressed_bullet_up = false;
        return;
    case sf::Keyboard::Scancode::Down:
        state.pressed_bullet_down = false;
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