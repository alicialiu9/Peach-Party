#include "Actor.h"
#include "StudentWorld.h"
#include "Board.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


//ACTOR IMPLEMENTATION

int Actor::getStatus() const
{
    return status;
}

bool Actor::available_dir(int dir)
{
    int currentX = getX();
    int currentY = getY();
    getPositionInThisDirection(dir, 16, currentX, currentY);
    if (getBoard()->getContentsOf(currentX/16, currentY/16) != Board::empty)
        return true;
    return false;
}

void Actor::hit_by_vortex()
{
    return;
}
bool Actor::is_at_fork(int dir)
{
    bool at_fork = false;
    int num_paths = 0;
    switch (dir)
    {
        case right:
            if (!available_dir(left))
            {
                at_fork = false;
                break;
            }
        case left:
        case up:
        case down:
        {
            if (available_dir(up))
                num_paths++;
            if (available_dir(left))
                num_paths++;
            if (available_dir(down))
                num_paths++;
            if (available_dir(right))
                num_paths++;
            if (num_paths >= 3)
                at_fork = true;
            break;
        }
    }
    return at_fork;
}

//PLAYER IMPLEMENTATION
int Player::getStars() const {return n_stars; }

int Player::getCoins() const {return n_coins; }

int Player::getWalkingDirection() const {return walking_direction; }

void Player::setWalkingDirection(int direction) {walking_direction = direction; }

void Player::updateCoins(int amt) {n_coins += amt;}

void Player::updateStars(int amt) {n_stars += amt;}

void Player::resetCoins(){ n_coins = 0;}

void Player::resetStars() { n_stars = 0;}

int Player::get_pState() const {return state;}

bool Player::is_a_square() const {return false;}

bool Player::can_be_hit_by_vortex() const {return false;}

int Player::get_dice() const {return die_roll;}

void Player::give_vortex()
{
    if (has_Vortex == false)
        has_Vortex = true;
}
bool Player::isWaiting()
{
    if (state == WAITING_TO_ROLL)
        return true;
    return false;
}
bool Player::isWalking()
{
    if (state == WALKING)
        return true;
    return false;
}

void Player::teleport_me_to_random_sq()
{
    int x = getX();
    int y = getY();
    getWorld()->get_random_square_location(x,y);
    moveTo(x*SPRITE_WIDTH, y*SPRITE_HEIGHT);
    
    walking_direction = 1;
}

void Player::swap_positions(Player* other)
{
    int tempX = getX();
    int tempY = getY();
    moveTo(other->getX(), other->getY());
    other->moveTo(tempX, tempY) ;
    
    //switch ticks left
    int temp_ticks_left = ticks_to_move;
    ticks_to_move = other->ticks_to_move;
    other->ticks_to_move = temp_ticks_left;

    //switch walk direction
    int temp_walk_dir = walking_direction;
    walking_direction = other->walking_direction;
    other->walking_direction = temp_walk_dir;

    //switch sprite direction
    int temp_sprite_dir = getDirection();
    setDirection(other->getDirection());
    other->setDirection(temp_sprite_dir);

    //switch state (roll/walk)
    int temp_state = state;
    state = other->state;
    other->state = temp_state;
}

void Player::swap_stars()
{
    Player* other_p = getWorld()->get_other_player(this);
    int temp_stars = getStars();
    this->resetStars();
    this->updateStars(other_p->getStars());
    other_p->resetStars();
    other_p->updateStars(temp_stars);
}
void Player::swap_coins()
{
    Player* other_p = getWorld()->get_other_player(this);
    int temp_coins = getCoins();
    this->resetCoins();
    this->updateCoins(other_p->getCoins());
    other_p->resetCoins();
    other_p->updateCoins(temp_coins);
}
void Player::equip_with_vortex_projectile()
{
    int fire_dir_x = 0;
    int fire_dir_y = 0;

    if (walking_direction == up)
        fire_dir_y = 16;
    else if (walking_direction == down)
        fire_dir_y = -16;
    else if (walking_direction == left)
        fire_dir_x = -16;
    else
        fire_dir_x = 16;
    getWorld()->add_actor(new Vortex(walking_direction, getBoard(),getWorld(),getX()+ fire_dir_x, getY() + fire_dir_y));
}

bool Player::if_have_vortex() const
{
    return has_Vortex;
}

void Player::set_at_dir_square()
{
    at_dir_square = true;
}

bool Player::get_at_dir_square() const
{
    return at_dir_square;
}

void Player::doSomething()
{
    if (state == WAITING_TO_ROLL)
    {
        if (walking_direction != right && walking_direction != left && walking_direction != up && walking_direction!= down)
        {
            bool got_valid_dir = false;
            while(got_valid_dir == false)
            {
                int r = randInt(0,3); // 0 right, 1 left, 2 up, 3 down
                switch (r)
                {
                    case 0:
                    {
                        if (available_dir(right))
                        {
                            walking_direction = right;
                            got_valid_dir = true;
                        }
                        break;
                    }
                    case 1:
                    {
                        if (available_dir(left))
                        {
                            walking_direction = left;
                            got_valid_dir = true;
                        }
                        break;
                    }
                    case 2:
                    {
                        if (available_dir(up))
                        {
                            walking_direction = up;
                            got_valid_dir = true;
                        }
                        break;
                    }
                    case 3:
                    {
                        if (available_dir(down))
                        {
                            walking_direction = down;
                            got_valid_dir = true;
                        }
                        break;
                    }
                }
            }
            if (walking_direction == left)
                setDirection(left);
            else
                setDirection(right);
        }
        switch (getWorld()->getAction(player_num))
        {
            case ACTION_ROLL:
            {
                die_roll = randInt(1,10);
                ticks_to_move = die_roll * 8;
                state = WALKING;
                break;
            }
            case ACTION_FIRE:
            {
                if (has_Vortex == true)
                {
                    equip_with_vortex_projectile();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    has_Vortex = false;
                }
                break;
            }
            default:
                return;
        }
    }
    if (state == WALKING)
    {
        if (!get_at_dir_square()) // if not at a direction square
        {
            if (getX() % 16 == 0 && getY() % 16 == 0 && is_at_fork(walking_direction))
            {
                switch (getWorld()->getAction(player_num))
                {
                    case ACTION_LEFT:
                    {
                        if (walking_direction!=left && available_dir(left))
                        {
                            walking_direction = left;
                            setDirection(left);
                        }
                        break;
                    }
                    case ACTION_RIGHT:
                    {
                        if (walking_direction!=right && available_dir(right))
                        {
                            walking_direction = right;
                            setDirection(right);
                        }
                        break;
                    }
                    case ACTION_UP:
                    {
                        if (walking_direction!=up && available_dir(up))
                        {
                            walking_direction = up;
                            setDirection(right);
                        }
                        break;
                    }
                    case ACTION_DOWN:
                    {
                        if (walking_direction!=down && available_dir(down))
                        {
                            walking_direction = down;
                            setDirection(right);
                          
                        }
                        break;
                    }
                    default:
                        return;
                }

            }

        }
    
        int xnew = getX();
        int ynew = getY();
        getPositionInThisDirection(walking_direction,16, xnew, ynew);
        if (xnew % 16 == 0 && ynew % 16 == 0)
        {
            if (getBoard()->getContentsOf(xnew/SPRITE_WIDTH, ynew/SPRITE_HEIGHT) == Board::empty)
            {
                if (walking_direction == right || walking_direction == left)
                {
                    if(available_dir(up))
                        setWalkingDirection(up);
                    else
                        setWalkingDirection(down);
                }
                else
                {
                    if(available_dir(right))
                        setWalkingDirection(right);
                    
                    else
                        setWalkingDirection(left);
                }
                
                if (getWalkingDirection() == left)
                    setDirection(left);
                else
                    setDirection(right);
            }
        }
        moveAtAngle(getWalkingDirection(),2);
        ticks_to_move --;
        die_roll = ticks_to_move/8;
        if (ticks_to_move == 0 )
            state = WAITING_TO_ROLL;
    }
    
}

// ACTIVATEONPLAYER IMPLEMENTATION
bool ActivateOnPlayer::overlap(Player* player, Actor* actor)
{
    if (player->getX() == actor->getX()  && player->getY() == actor->getY())
        return true;
    return false; 
}

Player* ActivateOnPlayer::r_Peach() const
{
    return getWorld()->getPeach();
}

Player* ActivateOnPlayer::r_Yoshi() const
{
    return getWorld()->getYoshi();
}

void ActivateOnPlayer::setActivated(Player* player, bool value)
{
    if (player == r_Peach())
        activated_p1 = value;
    
    if (player == r_Yoshi())
        activated_p2 = value;
}

bool ActivateOnPlayer::isActivated(Player* player) const
{
    if (player == r_Peach())
        return activated_p1;
    
    else
        return activated_p2;
}

bool ActivateOnPlayer::getActivated(Player* player)
{
    if (player == r_Peach())
        return activated_p1;
    
    else
        return activated_p2;
}

void ActivateOnPlayer::deactivate(Player* player)
{
    if (player == r_Peach())
        activated_p1 = false;
    else
        activated_p2 = false;
}

bool ActivateOnPlayer::new_activation(Player* player)
{
    if (overlap(player, this))
    {
        if (!isActivated(player))
        {
            setActivated(player,true);
            return true;
        }
    }
    else
        deactivate(player);
    return false;
}

bool ActivateOnPlayer::new_player_landed(Player* player)
{
    if (new_activation(player) && player->get_pState() == WAITING_TO_ROLL)
        return true;
    return false;
}
bool ActivateOnPlayer::new_player_moved_over(Player* player)
{
    if (new_activation(player) && (player->get_pState() == WALKING))
        return true;
    return false;
}

//SQUAREIMPLEMENTATION
 
bool Square::is_a_square() const
{
    return true;
}

bool Square::can_be_hit_by_vortex() const {return false;}

// COINSQUARE IMPLEMENTATION
void CoinSquare::doSomething()
{
    if (this->getStatus() == DEAD)
        return;
    
    if (new_player_landed(r_Yoshi()))
        change_coins(r_Yoshi());
    
    if (new_player_landed(r_Peach()))
        change_coins(r_Peach());
}
        
// BLUECOINSQUARE IMPLEMENTATION
void BlueCoinSquare::change_coins(Player* player)
{
    player->updateCoins(3);
    getWorld()->playSound(SOUND_GIVE_COIN);
}
        

// REDCOINSQUARE IMPLEMENTATION
void RedCoinSquare::change_coins(Player* player)
{
    if (player->getCoins() >= 3)
        player->updateCoins(-3);
    else
        player->resetCoins();
        
    getWorld()->playSound(SOUND_TAKE_COIN);
}
        
// STARSQUARE IMPLEMENTATION

void StarSquare::giveStar(Player* player)
{
    if (player->getCoins() < 20)
        return;
    
    else
    {
        player->updateCoins(-20);
        player->updateStars(1);
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
}

void StarSquare::doSomething()
{
    if (new_activation(r_Yoshi()) && (r_Yoshi()->isWaiting() || r_Yoshi()->isWalking()))
    {
        giveStar(r_Yoshi());
    }
    
    if (new_activation(r_Peach()) && (r_Peach()->isWaiting() || r_Peach()->isWalking()))
    {
        giveStar(r_Peach());
    }
        
}

//BANKSQUARE IMPLEMENTATION
void BankSquare::withdraw(Player* player)
{
    player->updateCoins(getWorld()->get_bank_coins());
    getWorld()->reset_bank_coins();
    getWorld()->playSound(SOUND_WITHDRAW_BANK);
}

void BankSquare::deposit(Player* player)
{
    int curr_bal = player->getCoins();
    if (curr_bal >= 5)
    {
        player->updateCoins(-5);
        getWorld()->deposit_bank_coins(5);
    }
    else
    {
        player->resetCoins();
        getWorld()->deposit_bank_coins(curr_bal);
    }
    getWorld()->playSound(SOUND_DEPOSIT_BANK);

}

//FIX
void BankSquare::doSomething()
{
    if (new_player_landed(r_Yoshi()))
    {
        withdraw(r_Yoshi());
    }
    
    if (overlap(r_Yoshi(), this) && r_Yoshi()->isWalking())
    {
        deposit(r_Yoshi());
    }
    if (new_player_landed(r_Peach()))
    {
        withdraw(r_Peach());
    }
    
    if (overlap(r_Peach(),this) && r_Peach()->isWalking())
    {
        deposit(r_Peach());
    }
}
            
//DIRECTIONALSQAURE IMPLEMENTATION
int  DirectionalSquare::getForcingDir() const
{
    return d_start_dir;
}

void DirectionalSquare::doSomething()
{
    if (new_activation(r_Yoshi()) && (r_Yoshi()->isWaiting() || r_Yoshi()->isWalking()))
    {
        r_Yoshi()->set_at_dir_square();
        r_Yoshi()->setWalkingDirection(getForcingDir());
        if (getForcingDir() == 180)
            r_Yoshi()->setDirection(left);
        else
            r_Yoshi()->setDirection(right);
    }
    if (new_activation(r_Peach()) && (r_Peach()->isWaiting() || r_Peach()->isWalking()))
    {
        r_Peach()->set_at_dir_square();
        r_Peach()->setWalkingDirection(getForcingDir());
        if (getForcingDir() == 180)
            r_Peach()->setDirection(left);
        else
            r_Peach()->setDirection(right);
    }
}

//DROPPINGSQAURE IMPLEMENTATION
void DroppingSquare::doActivity(Player *player)
{
    int option_n =  randInt(0,1);
    
    if (option_n == 0)
    {
        if (player->getCoins() >= 10)
            player->updateCoins(-10);
        else
            player->resetCoins();
    }
    
    else if (option_n == 1)
    {
        if (player->getStars() >= 1)
            player->updateStars(-1);
    }
    
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}


void DroppingSquare::doSomething()
{
    if (new_player_landed(r_Yoshi()))
    {
        doActivity(r_Yoshi());
    }
    if (new_player_landed(r_Peach()))
    {
        doActivity(r_Peach());
    }
}


//EVENTSQAURE IMPLEMENTATION

void EventSquare::inform_teleport(Player* player)
{
    player->teleport_me_to_random_sq();
    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
}
void EventSquare::inform_swap(Player* player, Player* other)
{
    player->swap_positions(other);
    int temp = getActivated(player);
    setActivated(player,getActivated(other));
    setActivated(other, temp);
    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
}

void EventSquare::give_vortex(Player* player)
{
    player->give_vortex();
    getWorld()->playSound(SOUND_GIVE_VORTEX);
}

void EventSquare::doActivity(Player* player, Player* other)
{
    int action_num = randInt(0,2);
    if (action_num == 0)
    {
        inform_teleport(player);
    }
    else if (action_num == 1)
    {
        inform_swap(player,other);
    }
    else
    {
        give_vortex(player);
    }
}

void EventSquare::doSomething()
{
    if (new_player_landed(r_Yoshi()))
    {
        doActivity(r_Yoshi(), r_Peach());
    }
    if (new_player_landed(r_Peach()))
    {
        doActivity(r_Peach(), r_Yoshi());
    }
}

//VORTEX IMPLEMENTATION
bool Vortex::is_a_square() const
{
    return false; 
}

bool Vortex::can_be_hit_by_vortex() const {return false;}

void Vortex::doSomething()
{
    std::cerr << "get vortex status" << std::endl;
    if (getStatus() == DEAD)
        return;
        
    moveAtAngle(v_start_dir, 2);
    
    int vortex_x = getX();
    int vortex_y = getY();
    
    if(vortex_x <= 0 || vortex_x >= VIEW_WIDTH || vortex_y<=0 || vortex_y >= VIEW_HEIGHT)
    {
        changeStatus(DEAD);
    }
    Actor* impacted_baddie = getWorld()->get_impacted_baddie(getX(), getY());
     if (impacted_baddie != nullptr)
    {
        std::cerr << "baddie hit" << std::endl;
        impacted_baddie->hit_by_vortex();
        changeStatus(DEAD);
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}

// ENEMY IMPLEMENTATION
bool Enemy::is_a_square() const
{
    return false;
}

bool Enemy::can_be_hit_by_vortex() const {return true;}

void Enemy::setWalkingDirection(int dir)
{
    walking_direction = dir; 
}

int Enemy::getWalkingDirection() const
{
    return walking_direction;
}

void Enemy::hit_by_vortex()
{
    int x = getX();
    int y = getY();
    getWorld()->get_random_square_location(x,y);
    moveTo(x*SPRITE_WIDTH, y*SPRITE_HEIGHT);

    walking_direction = right;
    setDirection(right);
    
    enemy_state = E_PAUSED;
    pause_counter = 180;
}

void Enemy::change_to_new_legal_dir()
{
    bool got_valid_dir = false;
    while(got_valid_dir == false)
    {
        int r = randInt(0,3); // 0 right, 1 left, 2 up, 3 down
        switch (r)
        {
            case 0:
            {
                if (available_dir(right))
                {
                    walking_direction = right;
                    got_valid_dir = true;
                }
                break;
            }
            case 1:
            {
                if (available_dir(left))
                {
                    walking_direction = left;
                    got_valid_dir = true;
                }
                break;
            }
            case 2:
            {
                if (available_dir(up))
                {
                    walking_direction = up;
                    got_valid_dir = true;
                }
                break;
            }
            case 3:
            {
                if (available_dir(down))
                {
                    walking_direction = down;
                    got_valid_dir = true;
                }
                break;
            }
        }
    }
    if (walking_direction == left)
        setDirection(left);
    else
        setDirection(right);
}

void Enemy::doActivity(Player* player)
{
    if (enemy_state == E_PAUSED)
    {
        if (new_player_landed(player))
            specialPause(player);
        pause_counter --;
        if ( pause_counter <= 0)
        {
            travel_distance = rand_squares_to_move();
            ticks_to_move = travel_distance * 8;
            change_to_new_legal_dir();
            enemy_state = E_WALKING;
        }
    }
    else if (enemy_state == E_WALKING)
    {
        if( getX() % 16 == 0 && getY() % 16 == 0 && is_at_fork(walking_direction))
        {
            change_to_new_legal_dir();
        }
        else
        {
            int xnew = getX();
            int ynew = getY();
            getPositionInThisDirection(walking_direction,16, xnew, ynew);
            if (xnew % 16 == 0 && ynew % 16 == 0)
            {
                if (getBoard()->getContentsOf(xnew/SPRITE_WIDTH, ynew/SPRITE_HEIGHT) == Board::empty)
                {
                    if (walking_direction == right || walking_direction == left)
                    {
                        if(available_dir(up))
                            setWalkingDirection(up);
                        else
                            setWalkingDirection(down);
                    }
                    else
                    {
                        if(available_dir(right))
                            setWalkingDirection(right);
                        
                        else
                            setWalkingDirection(left);
                    }
                    
                    if (getWalkingDirection() == left)
                        setDirection(left);
                    else
                        setDirection(right);
                }
            }
            
        }
        moveAtAngle(walking_direction, 2);
        ticks_to_move --;
        if (ticks_to_move == 0)
        {
            enemy_state = E_PAUSED;
            pause_counter = 180;
            specialWalk();
        }
    }
}


void Enemy::doSomething()
{
    doActivity(r_Yoshi());
    doActivity(r_Peach());
}


//BOWSER IMPLEMENTATION
int Bowser::rand_squares_to_move()
{
    return randInt(1,10);
}

void Bowser::specialPause(Player* player)
{
    int random = randInt(0,1);
    if (random == 0)
    {
        player->resetCoins();
        player->resetStars();
        getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::specialWalk()
{
    int r = randInt(1,4);
    if (r == 1)
    {
        getWorld()->add_dropping_square_at_location(getX(), getY());
        std::cerr << "new dropping square" << std::endl;
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
    
}


//BOO IMPLEMENTATION
int Boo::rand_squares_to_move()
{
    return randInt(1,3);
}

void Boo::specialPause(Player* player)
{
    int random = randInt(0,1);
    if (random == 0)
    {
        player->swap_coins();
    }
    else
    {
        player->swap_stars();
    }
    
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

void Boo::specialWalk()
{
    
}
