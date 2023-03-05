#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>

using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    all_actors.clear();
    peach = nullptr;
    bank_balance = 0;
}


int StudentWorld::init()
{
    // construct board
    int user_num = getBoardNumber();
    bd = new Board;
    string board_file = assetPath() + "board0" + to_string(user_num) + ".txt";
    Board::LoadResult result = bd->loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";

        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                Board::GridEntry ge = bd->getContentsOf(x, y);
                {
                    switch (ge) {
                        case Board::empty:
                            break;
                        case Board::player:
                        {
                            peach = new Peach(bd,this,x*SPRITE_WIDTH,y*SPRITE_HEIGHT);
                            yoshi = new Yoshi(bd,this,x*SPRITE_WIDTH,y*SPRITE_HEIGHT);
                            add_actor(new BlueCoinSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::blue_coin_square:
                        {
                            add_actor(new BlueCoinSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::red_coin_square:
                        {
                            add_actor(new RedCoinSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::star_square:
                        {
                            add_actor(new StarSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::bank_square:
                        {
                            add_actor(new BankSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::event_square:
                        {
                            add_actor(new EventSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::boo:
                        {
                            add_actor(new Boo(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            add_actor(new BlueCoinSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::bowser:
                        {
                            add_actor(new BlueCoinSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            add_actor(new Bowser(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT));
                            break;
                        }
                        case Board::up_dir_square:
                        {
                            add_actor(new DirectionalSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 90));
                            break;
                        }
                        case Board::down_dir_square:
                        {
                            add_actor(new DirectionalSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 270));
                            break;
                        }
                        case Board::left_dir_square:
                        {
                            add_actor(new DirectionalSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 180));
                            break;
                        }
                        case Board::right_dir_square:
                        {
                            add_actor(new DirectionalSquare(bd,this, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 0));
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
    }
    startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
    
    //initialize other StudentWorld variables
}


string StudentWorld::setGameStatTextHelper()
{
    ostringstream oss;
    oss << "P1 Roll: ";
    oss << "Stars: " << peach->getStars();
    oss << " $$: " <<peach->getCoins();
    if (peach->if_have_vortex())
        oss << " VOR";
    oss << " | Time: " << timeRemaining();
    oss << " | Bank: " << get_bank_coins();
    oss << " | P2 Roll: ";
    oss<<"Stars: " << yoshi->getStars();
    oss<< " $$: " << yoshi->getCoins();
    if (yoshi->if_have_vortex())
        oss << " VOR";
    string s = oss.str(); 
    return s;
}



int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.
    for (vector<Actor*>::iterator it = all_actors.begin(); it != all_actors.end();it++)
    {
        if ((*it)->getStatus() == ALIVE)
            (*it)->doSomething();
    }
    peach->doSomething();
    yoshi->doSomething();
    
    //delete dead actors
    for (vector<Actor*>::iterator it = all_actors.begin(); it != all_actors.end(); it++)
    {
        if ((*it)->getStatus() == DEAD)
        {
            delete (*it);
            it = all_actors.erase(it);
            it --;
        }
    }
    
    setGameStatText(setGameStatTextHelper());
    
    if (timeRemaining() <= 0)
    {
       playSound(SOUND_GAME_FINISHED);
       setFinalScore(Winner()->getStars(), Winner()->getCoins());
        if (Winner() == peach)
            return GWSTATUS_PEACH_WON;
        else
            return GWSTATUS_YOSHI_WON;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
//delete all remaining actors
    for (vector<Actor*>::iterator it = all_actors.begin(); it != all_actors.end();it++)
    {
        delete *it;
        it = all_actors.erase(it);
        it --;
    }
    delete peach;
    peach = nullptr;
    delete yoshi;
    yoshi = nullptr;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
    delete bd;
}


// Add a new actor to the StudentWorld (not for players, but all other game objects)
void StudentWorld::add_actor(Actor *actor)
{
    all_actors.push_back(actor);
}

// Determine if there is a square at the specified location. Used to determine if a gameobject
// like a player, enemy or vortex can move onto the specified location
bool StudentWorld::is_there_a_square_at_location(int dest_x, int dest_y) const
{
    return false;
}

// Get a pointer to the square at the specified location
Actor* StudentWorld::get_square_at_location(double x, double y) const
{
    for (int i = 0; i< all_actors.size(); i++)
    {
        if (all_actors[i]->is_a_square() && all_actors[i]-> getX() == x && all_actors[i]-> getX() == y)
            return all_actors[i];
    }
    return nullptr;
}

Actor* StudentWorld::get_impacted_baddie(double x, double y)const
{
    for (int i = 0; i< all_actors.size(); i++)
    {
        if (all_actors[i]->can_be_hit_by_vortex())
        {
            int baddie_x = all_actors[i]->getX();
            int baddie_upper_x = baddie_x + 15;
            
            int baddie_y = all_actors[i]->getY();
            int baddie_upper_y = baddie_y + 15;
            
            if ((x+15) >= baddie_x || x <= baddie_upper_x || y<= baddie_upper_y || (y+15) >= baddie_y)
                return all_actors[i];
        }
    }
    return nullptr;
}

// get # of coins in the bank
int StudentWorld::get_bank_coins() const
{
    return bank_balance;
}

// add coins to the bank (when player passes over a bank square)
void StudentWorld::deposit_bank_coins(int coins)
{
    bank_balance += coins;
}

// reset # of coins in the bank to zero
void StudentWorld::reset_bank_coins()
{
    bank_balance = 0; 
}

// Get a random location on the board that contains a square
void StudentWorld::get_random_square_location(int &x, int &y) const
{
    for (;;)
    {
        int random_x = randInt (0, 15);
        int random_y = randInt (0, 15);
        Board::GridEntry square = bd->getContentsOf(random_x, random_y);
        if (square != Board::empty && square != Board::player && square != Board::bowser && square != Board::boo && (x != random_x || y != random_y))
        {
            x = random_x;
            y = random_y;
            break;
        }
    }
}


Player* StudentWorld::get_other_player(Player* p) const
{
    if (p == peach)
        return getYoshi();
    else
        return getPeach();
}

Player* StudentWorld::getPeach() const
{
    return peach;
}

Player* StudentWorld::getYoshi() const
{
    return yoshi;    
}

Player* StudentWorld::Winner()
{
    if (peach->getStars() > yoshi->getStars())
        return peach;
    
    else if (peach->getStars() < yoshi->getStars())
        return yoshi;
    
    else
    {
        if (peach->getCoins() > yoshi->getCoins())
            return peach;
        else if (peach->getCoins() < yoshi->getCoins())
            return yoshi;
        else
        {
            int random = randInt(0, 1);
            if (random == 0)
                return yoshi;
            else
                return peach;
        }
    }
}
