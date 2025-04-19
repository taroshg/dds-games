#ifndef GAME_WRAPPER_HEADER
#define GAME_WRAPPER_HEADER

#include "../GameMessagePubSubTypes.hpp"

class GameWrapper {
protected:
    int my_wins_;
    int opp_wins_;
    int my_moves_;
    int opp_moves_;

    bool first_;
public:
    GameWrapper() : 
    my_wins_(0),
    opp_wins_(0) {}

    // update game_msg with state after your move
    virtual void get_user_move(GameMessage* game_msg, GameMessage* opp_game_msg) = 0;
    virtual int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) = 0;
    virtual bool is_end(GameMessage* game_msg, GameMessage* opp_game_msg) = 0;

    int get_my_wins() const {return my_wins_;}
    int get_opp_wins() const {return opp_wins_;}

    void I_moved() {my_moves_++;} // call this after every move you make
    void opp_moved() {opp_moves_++;} // call this after every opp move

    int my_moves() const {return my_moves_;}
    int opp_moves() const {return opp_moves_;} 

    bool is_full_turn() const {
        if (my_moves_ == 0 || opp_moves_ == 0) return false;
        return my_moves_ == opp_moves_;
    }

    void hard_reset(GameMessage* game_msg){
        game_msg->ttt(0);
        game_msg->rps(0);
        game_msg->c4(0);

        my_moves_ = 0;
        opp_moves_ = 0;
        my_wins_ = 0;
        opp_wins_ = 0;
    }

    void set_first(bool t){
        first_ = t;
    }
};

#endif