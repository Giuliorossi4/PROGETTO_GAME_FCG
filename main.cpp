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

// Font
const sf::Font font{"resources/font/LowresPixel-Regular.otf"};

// window
const char* window_title = "Tappa-12";
const unsigned window_width = 1200;
const unsigned window_height = 680;
const float max_frame_rate = 60;

// Menu
const float outLine = 5.f;

// Room
sf::Vector2f roomPos;
enum class Direction { Left, Right, Up, Down };

// Player
const float player_speed = 400;

// Bullet
const float bullet_speed = 600;

// Life
const sf::Vector2f edge = {10.f, 10.f};
const int numLife = 3;

// Enemy
const float enemy_speed = 100;
const int numEnemy = 10;

struct Room{
    sf::Texture texture;
    sf::Sprite sprite;

    Room ();
    void draw (sf::RenderWindow& window);
};

struct Menu {
    sf::RectangleShape box;
    sf::RectangleShape button;
    sf::Text titleText;
    sf::Text buttonText;
    sf::Texture texture;
    sf::Vector2f boxSize;
    sf::Vector2f buttonSize;

    Menu();
    void update (bool isGameOver);
    void draw (sf::RenderWindow& window);
};

struct Life{
    sf::Texture texture;
    sf::Texture texture2;
    sf::Vector2f pos;
    sf::Vector2f size;
    int life;

    Life ();
    void draw (sf::RenderWindow& window);

    void reset ();
};

struct Round{
    sf::Text text;
    sf::Vector2f pos;
    sf::Vector2f size;
    int num;

    Round ();
    void draw (sf::RenderWindow& window);
};

struct Hub{
    Life life;
    Round round;
    sf::Texture texture;
    sf::RectangleShape shape;
    sf::Vector2f size;
    sf::Vector2f pos;

    Hub ();
    void draw (sf::RenderWindow& window);
    void nextRound ();
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
    void setDirection (Direction direction);
    void move_player_left (float elapsed);
    void move_player_right (float elapsed);
    void move_player_up (float elapsed);
    void move_player_down (float elapsed);
    void hitted (){ hit = true; }
    
    void reset (int numRound);
};

struct Enemy{
    sf::RectangleShape shape;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture texture;
    float myspeed;
    int enemylife;
    
    bool isWaiting;
    float waitTimer;
    float WAIT_DURATION;

    Enemy (float speed, int life, sf::Vector2f posSpawn, sf::Texture& textureSelect);
    void draw (sf::RenderWindow& window);
    bool move (float elapsed, Player& player);
    void hit (){ enemylife --; };
    bool dead (){ return enemylife <= 0; };
    void startWaiting ();
};

struct Bullet{
    sf::RectangleShape shape;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Texture texture;
    Direction direction;
    float speed;
    bool inAction;

    Bullet (Direction direction, sf::Vector2f posPlayer);
    void draw (sf::RenderWindow& window);
    void move_bullet (float elapsed);
    bool hit (Enemy& enemy);
};

struct Attack{
    std::vector<Bullet> bullets;

    Attack (){};
    void draw (sf::RenderWindow& window);
    void attack (Direction direction, sf::Vector2f posPlayer);
    void update (float elapsed);
    void hit (Enemy& enemy);
    void collision (Hub Hub);
    void cleanUp ();

    void reset();
};

struct Wave {
    std::vector<Enemy> enemies;
    sf::Texture textureSpider;
    sf::Texture textureSnake;
    float spawnTimer;
    int totalEnemy;
    int enemyAlive;
    int spawnedCount;
    float speed;

    Wave ();
    void draw (sf::RenderWindow& window);
    void collision (Hub hub);
    sf::Texture& randomTexture ();
    sf::Vector2f randomSpawnPoint (sf::Texture texture);
    void cleanUp ();
    void update (float elapsed);

    void reset (int numRound);
};

struct State{
    Room room;
    Menu menu;
    Hub hub;
    Player player;
    Attack attack;
    Wave wave;
    bool focus;
    bool pause;
    bool game_over;

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
              game_over(false),
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
    void restart (bool nextRound);
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

Menu::Menu() : box(boxSize), button(buttonSize), titleText(font, "", 10), buttonText(font,"", 10) {
    texture = sf::Texture("texture/floor/floorBlock02.png");
    texture.setRepeated(true);
    
    boxSize = {window_width / 1.5f, window_height / 1.5f};
    box.setSize(boxSize);
    box.setTexture(&texture);
    box.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(boxSize.x), static_cast<int>(boxSize.y)}));
    box.setPosition({(window_width - boxSize.x) / 2.f, (window_height - boxSize.y) / 2.f});
    box.setOutlineThickness(5.f);
    box.setOutlineColor(sf::Color::Black);

    buttonSize = {200.f, 60.f};
    button.setSize(buttonSize);
    button.setFillColor(sf::Color(50, 50, 50));
    button.setOutlineThickness(3.f);
    button.setOutlineColor(sf::Color::White);
    button.setPosition({
        box.getPosition().x + (boxSize.x - buttonSize.x) / 2.f,
        box.getPosition().y + (boxSize.y * 0.65f)
    });

    titleText.setFont(font);
    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color::White);

    buttonText.setFont(font);
    buttonText.setCharacterSize(25);
    buttonText.setFillColor(sf::Color::White);
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

Bullet::Bullet (Direction direction, sf::Vector2f playerCenter) : shape(size){
    this->direction = direction;
    inAction = true;
    
    texture = sf::Texture ("texture/bullet/bulletUpdated.png");
    size = (sf::Vector2f)texture.getSize();

    shape.setTexture(&texture);
    pos = {playerCenter.x, playerCenter.y};
    if(direction == Direction::Left){
        shape.setScale({-1.f,1.f});
        shape.setOrigin({0.f, size.y/2.f});
    }
    else if(direction == Direction::Right){
        shape.setOrigin({0.f,size.y/2.f});
    }
    else if(direction == Direction::Up){
        shape.rotate(sf::degrees(90.f));
        shape.setScale({-1.f,1.f});
        shape.setOrigin({0.f, size.y/2.f});
        
    }
    else if(direction == Direction::Down){
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
    texture2 = sf::Texture("texture/life/heart2.png");
    pos = edge;
    size = {(texture.getSize().x * life) + (edge.x * (life - 1)), (float)texture.getSize().y};
}

Round::Round () : text(font,"ROUND: ", 30){
    text.setPosition({(window_width / 2.f) - 100, edge.y});
    text.setFillColor(sf::Color::White);
    num = 1;
}

Hub::Hub () : shape(size){
    texture = sf::Texture("texture/floor/floorBlock02.png");
    texture.setRepeated(true);
    pos = {0,0};
    size = {window_width, life.texture.getSize().y + (edge.y*2.f)};
    shape.setSize(size);
    shape.setTexture(&texture);
    shape.setTextureRect(sf::IntRect({0, 0}, (sf::Vector2i)size));
    roomPos = {0, size.y + outLine};
    shape.setOutlineThickness(outLine);
    shape.setOutlineColor(sf::Color::Black);
    size.y += outLine;
}

Enemy::Enemy (float speed, int life, sf::Vector2f posSpawn, sf::Texture& textureSelect) : shape(size){
    enemylife = life;
    isWaiting = false;
    waitTimer = 0.f;
    WAIT_DURATION = 0.5f;
    texture = textureSelect;
    size =  (sf::Vector2f)texture.getSize();
    pos = posSpawn;
    shape.setPosition(pos);
    shape.setTexture(&texture);
    shape.setSize(size);
    if(pos.x != 0.f){
        shape.setScale({-1.f, 1.f});
        shape.setOrigin({size.x,0.f});
    }

    myspeed = speed;
}

Wave::Wave (){
    textureSnake = sf::Texture("texture/enemy/snake.png");
    textureSpider = sf::Texture("texture/enemy/spider.png");
    enemyAlive = numEnemy;
    spawnedCount = 0;
    totalEnemy = numEnemy;
    spawnTimer = 0.f;
    enemies.reserve(enemyAlive); 
    speed = enemy_speed;
}

//////////
// Draw //
//////////

void Room::draw (sf::RenderWindow& window){
    window.draw(sprite);
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(button);
    window.draw(titleText);
    window.draw(buttonText);
}

void Hub::draw (sf::RenderWindow& window){
    window.draw(shape);
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
    shape.setTexture(&texture);
    shape.setPosition(pos);
    window.draw(shape);
}

void Attack::draw (sf::RenderWindow& window){
    for(size_t i = 0; i < bullets.size(); i++){
        bullets.at(i).draw(window);
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
    sprite.setTexture(texture2);
    for(int i = life; i < numLife; i++){
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
    shape.setTexture(&texture);
    shape.setPosition(pos);
    window.draw(shape);
}

void Wave::draw (sf::RenderWindow& window){
    for(auto& enemy : enemies){
        enemy.draw(window);
    }
}

void State::draw (sf::RenderWindow& window){
    room.draw(window);
    if(!pause){
        attack.draw(window);
        player.draw(window);
        wave.draw(window);
    }
    else{
        if(hub.round.num == 1) menu.draw(window);
        else{
            player.draw(window);
            wave.draw(window);
        }
    }
    hub.draw(window);
}

////////////
// Update //
////////////

void Menu::update (bool isGameOver) {
    if (!isGameOver) {
        titleText.setString("DUNGEON CLEANSER");
        buttonText.setString("START");
    } else {
        titleText.setString("GAME OVER");
        buttonText.setString("RESTART");
    }

    buttonText.setPosition(sf::Vector2f(
        button.getPosition().x + (button.getSize().x - buttonText.getGlobalBounds().size.x) / 2.f,
        button.getPosition().y + (button.getSize().y - buttonText.getGlobalBounds().size.y) / 1.8f
    ));

    titleText.setPosition(sf::Vector2f(
        box.getPosition().x + (boxSize.x - titleText.getGlobalBounds().size.x) / 2.f,
        box.getPosition().y + (boxSize.y * 0.2f)
    ));
}

void Life::reset (){
    life = numLife;
}

void Hub::nextRound (){
    round.num ++;
}

void Player::setDirection(Direction direction){
    if(direction == Direction::Left) lastTexture = textureLeft;
    if(direction == Direction::Right) lastTexture = textureRight;
    if(direction == Direction::Up) lastTexture = textureBack;
    if(direction == Direction::Down) lastTexture = textureFront;
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

void Player::reset (int numRound){
    hit = false;
    lastTexture = textureFront;
    size = (sf::Vector2f)lastTexture.getSize();
    float player_px = ((float) window_width / 2.0) - (size.x / 2.0);
    float player_py = ((float) window_height/2.0) - size.y;
    pos = {player_px, player_py};
    speed = player_speed + (20 * numRound);
}

void Bullet::move_bullet (float elapsed){
    if(direction == Direction::Left){
        pos.x -= speed * elapsed;
    }
    else if(direction == Direction::Right){
        pos.x += speed * elapsed;
    }
    else if(direction == Direction::Up){
        pos.y -= speed * elapsed;
    }
    else if(direction == Direction::Down){
        pos.y += speed * elapsed;
    }
}

bool Bullet::hit (Enemy& enemy) {
    sf::FloatRect bulletBounds = this->shape.getGlobalBounds();
    sf::FloatRect enemyBounds = enemy.shape.getGlobalBounds();

    if (bulletBounds.findIntersection(enemyBounds)) {
        return true;
    }
    return false;
}

void Attack::attack (Direction direction, sf::Vector2f posPlayer){
    bullets.push_back(Bullet(direction, posPlayer));
}

void Attack::update (float elapsed){
    for(auto& bullet : bullets){
        bullet.move_bullet(elapsed);
    }
}

void Attack::cleanUp() {
    for(size_t i = 0; i < bullets.size();){
        if(!bullets.at(i).inAction){
            bullets.erase(bullets.begin() + i);
        }
        else i++;
    }
}

void Attack::hit (Enemy& enemy){
    for(auto& bullet : bullets){
        if(bullet.hit(enemy)){
            bullet.inAction = false;
            enemy.hit();
        }
    }
}

void Attack::collision (Hub hub){
    for(auto & bullet : bullets){
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

void Attack::reset (){
    bullets.clear();
}

void Enemy::startWaiting() {
    isWaiting = true;
    waitTimer = 0.f;
}

bool Enemy::move (float elapsed, Player& player){
    sf::Vector2f enemyCenter = {pos.x + (size.x / 2.f), pos.y + (size.y / 2.f)};
    sf::Vector2f playerCenter = {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)};

    sf::Vector2f distance = {enemyCenter.x - playerCenter.x, enemyCenter.y - playerCenter.y};
    float movement = (myspeed * elapsed);

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

sf::Vector2f Wave::randomSpawnPoint (sf::Texture texture){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distr(1, 2);
    float X = 0.f;
    float Y = 0.f;
    if(distr(gen) == 2){
        std::uniform_int_distribution<> distrX(0.f, window_width - texture.getSize().x);
        X = distrX(gen);
        if(X == 0.f  || X == window_width  - (texture.getSize().x)){
            std::uniform_real_distribution<> distrY(roomPos.y, window_height - texture.getSize().y);
            Y = distrY(gen);
        }
        else{
            std::uniform_real_distribution<> distrY(1,2);
            if(distrY(gen) == 1){
                Y = roomPos.y - texture.getSize().y;
            }
            else{
                Y = window_height;
            }
        }
    }
    else{
        
        std::uniform_real_distribution<> distrY(roomPos.y, window_height - texture.getSize().y);
        Y = distrY(gen);
        if(Y == roomPos.y || Y == window_height - texture.getSize().y){
            std::uniform_int_distribution<> distrX(0.f, window_width - texture.getSize().x);
            X = distrX(gen);
        }
        else{
            std::uniform_real_distribution<> distrX(1,2);
            if(distrX(gen) == 1){
                X = 0.f;
            }
            else{
                X = window_width - texture.getSize().x;
            }
        }
    }

    return {X,Y};
}

sf::Texture& Wave::randomTexture (){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distr (1,3);
    
    if(distr(gen) % 2 != 0){
        return textureSpider;
    }
    else{
        return textureSnake;
    }
}

void Wave::update (float elapsed) {
    spawnTimer += elapsed;
    if (spawnedCount < totalEnemy) {
        if (spawnTimer >= 1.0f) {
            sf::Texture& newEmenyTexture = randomTexture();
            sf::RectangleShape newEnemy((sf::Vector2f)newEmenyTexture.getSize());
            sf::Vector2f newEmenyPos = randomSpawnPoint(newEmenyTexture);
            newEnemy.setPosition(newEmenyPos);
            
            bool conflict = false;
            sf::FloatRect boundsA = newEnemy.getGlobalBounds();
            
            for(auto& enemy : enemies){
                sf::FloatRect boundsB = enemy.shape.getGlobalBounds();
                if(boundsA.findIntersection(boundsB)){
                    conflict = true;
                    break;
                }
            }
            
            if(!conflict){
                if(newEmenyTexture.getSize() == textureSpider.getSize())
                    enemies.emplace_back(speed, 2, newEmenyPos, newEmenyTexture);
                else
                    enemies.emplace_back(speed * 1.5f, 1, newEmenyPos, newEmenyTexture);
                spawnedCount++;
                spawnTimer = 0.f;
            }
        }
    }
}

void Wave::collision (Hub hub) {
    for (size_t i = 0; i < enemies.size(); ++i) {

        for (size_t j = 0; j < enemies.size(); ++j) {
            if (i == j) continue; 

            sf::FloatRect boundsA = enemies[i].shape.getGlobalBounds();
            sf::FloatRect boundsB = enemies[j].shape.getGlobalBounds();

            
            auto intersectionOpt = boundsA.findIntersection(boundsB);
            if (intersectionOpt) {
                sf::FloatRect inter = *intersectionOpt;

                if (inter.size.x < inter.size.y) {
                    if (boundsA.position.x < boundsB.position.x) {
                        enemies[i].pos.x -= inter.size.x / 2.f;
                    } else {
                        enemies[i].pos.x += inter.size.x / 2.f;
                    }
                } else {
                    if (boundsA.position.y < boundsB.position.y) {
                        enemies[i].pos.y -= inter.size.y / 2.f;
                    } else {
                        enemies[i].pos.y += inter.size.y / 2.f;
                    }
                }
            }
        }
    }

    for(auto & enemy : enemies){
        if(enemy.pos.x < 0.f){
            enemy.pos.x = 0.f;
        }
        if(enemy.pos.y < hub.size.y){
            enemy.pos.y = hub.size.y;
        }
        if(enemy.pos.x + enemy.size.x > window_width){
            enemy.pos.x = window_width - enemy.size.x;
        }
        if(enemy.pos.y + enemy.size.y > window_height){
            enemy.pos.y = window_height - enemy.size.y;
        }
    }
}

void Wave::cleanUp() {
    for(size_t i = 0; i < enemies.size(); ){
        if(enemies.at(i).dead()){
            enemies.erase(enemies.begin() + i);
            enemyAlive --;
        }
        else i++;
    }
}

void Wave::reset (int numRound){
    enemies.clear();
    enemyAlive = numEnemy + (1 * numRound);;
    spawnedCount = 0;
    totalEnemy = enemyAlive;
    spawnTimer = 0.f;
    enemies.reserve(enemyAlive); 
    speed = enemy_speed + (20 * numRound);
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

    for(auto& enemy : wave.enemies){
        attack.hit(enemy);
    }
    attack.collision(hub);
    wave.collision(hub);

    attack.cleanUp();
    wave.cleanUp();
}

void State::restart (bool nextRound)
{
    if(nextRound){
        player.reset(hub.round.num);
        wave.reset(hub.round.num);
        hub.nextRound();
    }
    else{
        player.reset(0);
        hub.life.reset();
        wave.reset(0);
        hub.round.num = 1;
        game_over = true;
    }
    attack.reset();
    pause = true;
    move_player_left = false;
    move_player_right = false;
    move_player_up = false;
    move_player_down = false;
    num_bullet_left = 0;
    num_bullet_right = 0;
    num_bullet_up = 0;
    num_bullet_down = 0;
    pressed_bullet_left = false;
    pressed_bullet_right = false;
    pressed_bullet_up = false;
    pressed_bullet_down = false;
}

void State::update (float elapsed){
    if(!pause){

        if(move_player_left){
            player.move_player_left(elapsed);
            player.setDirection(Direction::Left);
        }
        if(move_player_right){
            player.move_player_right(elapsed);
            player.setDirection(Direction::Right);
        }
        if(move_player_up){
            player.move_player_up(elapsed);
            player.setDirection(Direction::Up);
        }
        if(move_player_down){
            player.move_player_down(elapsed);
            player.setDirection(Direction::Down);
        }
        
        // Player attact
        for(; num_bullet_left > 0; num_bullet_left--){
            attack.attack(Direction::Left, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_right > 0; num_bullet_right--){
            attack.attack(Direction::Right, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_up > 0; num_bullet_up--){
            attack.attack(Direction::Up, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        for(; num_bullet_down > 0; num_bullet_down--){
            attack.attack(Direction::Down, {player.pos.x + (player.size.x / 2.f), player.pos.y + (player.size.y / 2.f)});
        }
        
        wave.update(elapsed);
        attack.update(elapsed);

        for(auto& enemy : wave.enemies){
            if(enemy.move(elapsed, player)){
                hub.life.life -= 1;
                enemy.startWaiting();
            }
        }

        collision();
        
        if(hub.life.life <= 0){
            restart(false);
        }

        if(wave.enemyAlive <= 0){
            restart(true);
        }
    }
    else {
        menu.update(game_over);
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
        state.game_over = false;
        return;

    case sf::Keyboard::Scan::Space:
        state.pause = !state.pause;
        state.game_over = false;
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

void handle (const sf::Event::MouseMoved& mouse, State& state)
{

    if (!state.pause) return;

    sf::Vector2f mousePos = sf::Vector2f(mouse.position);

    if (state.menu.button.getGlobalBounds().contains(mousePos)) {
        state.menu.button.setFillColor(sf::Color(100, 100, 100));
    } else {
        state.menu.button.setFillColor(sf::Color(50, 50, 50));
    }
}

void handle (const sf::Event::MouseButtonPressed& mouse, State& state)
{
    if (!state.pause) return;

    if (mouse.button == sf::Mouse::Button::Left) {
        sf::Vector2f mousePos = sf::Vector2f(mouse.position);

        if (state.menu.button.getGlobalBounds().contains(mousePos)) {
            if (state.game_over) {
                state.game_over = false;
            }
            state.pause = false;
        }
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