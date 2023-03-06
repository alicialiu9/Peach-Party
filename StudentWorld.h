#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>
#include <sstream>

class GameObject;
class Actor;
class Player; 

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual int init(); // never call
  virtual int move(); // never call
  virtual void cleanUp();
  ~StudentWorld(); //only place in written code that can call above 3 functions (cleanup)
    //Setgamestat
    std::string setGameStatTextHelper();
    void add_actor(Actor *actor);
    
     // Determine if there is a square at the specified location. Used to determine if a gameobject
     // like a player, enemy or vortex can move onto the specified location
     bool is_there_a_square_at_location(int dest_x, int dest_y) const;
     // Get a pointer to the square at the specified location
     Actor* get_square_at_location(int x, int y);
    //return baddie that overlaps with vortex
    Actor* get_impacted_baddie(int x, int y)const;
    void add_dropping_square_at_location(int x, int y);
     int get_bank_coins() const;
     void deposit_bank_coins(int coins);
     void reset_bank_coins();
     // Get a random location on the board that contains a square
     void get_random_square_location(int &x, int &y) const;
     Player* Winner();
    Player* get_other_player(Player* p) const;
    Player* getPeach() const;
    Player* getYoshi() const;

private:
    //container of pointers to objects
    std::vector<Actor*> all_actors;
    Player* peach;
    Player* yoshi; 
    int bank_balance;
    Board* bd; 
};

#endif // STUDENTWORLD_H_




