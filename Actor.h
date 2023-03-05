#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameWorld.h"

class StudentWorld;
class Board; 

const int DEAD = 0;
const int ALIVE = 1;

const int WAITING_TO_ROLL = 0;
const int WALKING = 1;

const int E_WALKING = 0;
const int E_PAUSED = 1;

// Students: Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
    public:
        Actor(Board* b,StudentWorld* world, int imageID, double startX, double startY, int startDirection, int depth = 0, double size = 1.0): GraphObject(imageID, startX, startY, startDirection, depth,size), m_world(world), status(1), m_b(b) {};
        virtual void doSomething() = 0;
        virtual bool is_a_square() const = 0;
        virtual bool can_be_hit_by_vortex() const = 0;
        void changeStatus(int state) { status = state; }
        int getStatus();
        StudentWorld* getWorld() const { return m_world; }
        Board* getBoard() const { return m_b; }
        bool available_dir(int dir);
        bool is_at_fork(int dir);
        virtual void hit_by_vortex();
    private:
        StudentWorld* m_world;
        int status;
        Board* m_b;
};


class Player : public Actor
{
    public:
    // CHANGE LATER
        Player(Board* b,StudentWorld* world, int imageID, int x, int y, int player_n ): Actor(b, world, imageID, x, y, right, 0, 1.0), n_stars(0), n_coins(0), player_num(player_n), ticks_to_move(0), state(WAITING_TO_ROLL), walking_direction(0), die_roll(0), has_Vortex(false), at_dir_square(false){};
        virtual void doSomething();
        int get_pState() const;
        int getStars() const;
        int getCoins() const;
        int get_dice() const; 
        void updateCoins(int amt);
        void updateStars(int amt);
        void resetCoins();
        void resetStars();
        int getWalkingDirection() const ;
        void setWalkingDirection(int direction);
        void move();
        void roll_dice();
        void teleport_me_to_random_sq();
        void swap_positions(Player* other);  // with other player
        void swap_stars();  // with other player
        void swap_coins();  // with other player
        void equip_with_vortex_projectile();
        bool if_have_vortex()const;
        void give_vortex(); 
        void force_walk_direction(int dir, int angle);
        void set_at_dir_square();
        bool get_at_dir_square() const;
    //  REDO :
        bool isWaiting();
        bool isWalking();
        virtual bool is_a_square() const;
        virtual bool can_be_hit_by_vortex() const;
    
    private:
        int player_num;
        int state;
        int n_stars;
        int n_coins;
        int ticks_to_move;
        int walking_direction;
        int die_roll;
        bool has_Vortex;
        bool at_dir_square;
};

class Peach : public Player
{
    public:
        Peach(Board* b,StudentWorld* world, int x, int y) : Player (b, world, IID_PEACH, x, y, 1){};
};

class Yoshi : public Player
{
    public:
    Yoshi(Board* b, StudentWorld* world, int x, int y) : Player (b,world, IID_YOSHI, x, y, 2){};
    
};

class ActivatingObject : public Actor
{
    public:
        ActivatingObject(Board* b, StudentWorld* world,int imageID, int x, int y, int direction, int depth, double size): Actor(b,world,imageID, x, y, direction) {};
        virtual void doSomething() = 0;
    private:
};

class ActivateOnPlayer : public ActivatingObject
{
    public:
        ActivateOnPlayer(Board* b, StudentWorld* world,int imageID, int x, int y, int direction, int depth, double size): ActivatingObject(b,world,imageID, x, y, direction, depth, size),activated_p1(false), activated_p2(false) {};
        bool overlap(Player* player, Actor* actor);
        Player* r_Peach() const;
        Player* r_Yoshi() const;
        bool new_activation(Player* player);
        bool new_player_landed(Player* player);
        bool new_player_moved_over(Player* player);
        void deactivate(Player* player);
        void setActivated(Player* player, bool value);
        bool getActivated(Player* player);
    private:
        bool activated_p1;
        bool activated_p2;
        bool isActivated(Player* player) const;
    
};

class Square :public ActivateOnPlayer
{
    public:
        Square(Board* b, StudentWorld* world,int imageID, int x, int y, int direction): ActivateOnPlayer(b,world,imageID, x, y, direction, 1, 1.0) {};
        bool is_a_square() const;
        virtual bool can_be_hit_by_vortex() const;
    private:
};

class CoinSquare : public Square
{
    public:
        CoinSquare (Board* b, StudentWorld* world, int imageID, int x, int y) : Square(b,world, imageID, x, y, right){};
        virtual void doSomething();
        virtual void change_coins(Player* player) = 0 ;
    private:
};

class BlueCoinSquare: public CoinSquare
{
    public:
        BlueCoinSquare (Board* b, StudentWorld* world, int x, int y) : CoinSquare(b,world, IID_BLUE_COIN_SQUARE, x, y){};
        virtual void change_coins(Player* player);
    private:
};

class RedCoinSquare: public CoinSquare
{
    public:
        RedCoinSquare (Board* b, StudentWorld* world, int x, int y) : CoinSquare(b,world, IID_RED_COIN_SQUARE, x, y){};
        virtual void change_coins(Player* player);
        private:
};

class StarSquare : public Square
{
    public:
        StarSquare (Board* b, StudentWorld* world, int x, int y) : Square(b,world, IID_STAR_SQUARE, x, y, right){};
        virtual void doSomething();
        void giveStar(Player* player);
    private:
};


class BankSquare : public Square
{
    public:
        BankSquare (Board* b, StudentWorld* world, int x, int y) : Square(b, world,IID_BANK_SQUARE, x, y, right){};
        virtual void doSomething();
        void withdraw(Player* player);
        void deposit(Player* player);
    private:
};

class DirectionalSquare : public Square
{
    public:
        DirectionalSquare (Board* b, StudentWorld* world, int x, int y, int direction) : Square(b, world,IID_DIR_SQUARE, x, y, direction), d_start_dir(direction){};
        virtual void doSomething();
        int getForcingDir() const;
    private:
        int d_start_dir;
};

class DroppingSquare : public Square
{
    public:
        DroppingSquare (Board* b, StudentWorld* world, int x, int y) : Square(b, world,IID_DROPPING_SQUARE, x, y, right){};
        virtual void doSomething();
        void doActivity(Player* player);
    private:
};

class EventSquare : public Square
{
    public:
        EventSquare (Board* b, StudentWorld* world, int x, int y) : Square(b, world,IID_EVENT_SQUARE, x, y, right){};
        virtual void doSomething();
        void doActivity(Player* player, Player* other);
    private:
        void inform_teleport(Player* player);
        void inform_swap(Player* player, Player* other);
        void give_vortex(Player* player);
};

class Enemy: public ActivateOnPlayer
{
    public:
        Enemy(Board* b, StudentWorld* world,int imageID, int x, int y): ActivateOnPlayer(b,world,imageID, x, y, right, 0, 1.0), travel_distance(0), enemy_state(E_PAUSED), pause_counter(180), ticks_to_move(0), walking_direction(right){};
        virtual void doSomething();
        void doActivity(Player* player);
        virtual void specialPause(Player* player) = 0;
        virtual void specialWalk()= 0;
        void change_to_new_legal_dir();
        bool is_a_square() const;
        void setWalkingDirection(int dir);
        int getWalkingDirection() const;
        virtual int rand_squares_to_move() = 0;
        virtual bool can_be_hit_by_vortex() const;
        virtual void hit_by_vortex();
    private:
        int travel_distance;
        int enemy_state;
        int pause_counter;
        int ticks_to_move;
        int walking_direction;
};

class Bowser : public Enemy
{
    public:
        Bowser(Board* b, StudentWorld* world, int x, int y): Enemy(b,world,IID_BOWSER, x, y){};
        virtual void specialPause(Player* player);
        virtual void specialWalk();
        virtual int rand_squares_to_move();
    private:
      
};

class Boo : public Enemy
{
    public:
        Boo(Board* b, StudentWorld* world, int x, int y): Enemy(b,world,IID_BOO, x, y){};
        virtual void specialPause(Player* player);
        virtual void specialWalk();
        virtual int rand_squares_to_move();
    private:
};


class Vortex : public ActivatingObject
{
    public:
    Vortex(Board* b, StudentWorld* world,int x, int y, int fire_direction): ActivatingObject(b,world,IID_VORTEX, x, y, right, 0, 1.0), v_start_dir(fire_direction){};
    virtual void doSomething();
    bool overlap_with_Baddie(Actor* actor);
    bool is_a_square() const;
    virtual bool can_be_hit_by_vortex() const;
    private:
        int v_start_dir;
};


#endif // ACTOR_H_
