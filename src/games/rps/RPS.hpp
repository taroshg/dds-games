#include "../GameWrapper.hpp"
#include <iostream>

class RPS : public GameWrapper {
public:
    RPS(): GameWrapper() {};
    void get_user_move(GameMessage* game_msg) override {
        char move;
        do {
            std::cout << "'r' for rock\n";
            std::cout << "'p' for paper\n";
            std::cout << "'s' for scissors\n";
            std::cout << "Enter your choice: ";
            std::cin >> move;
        } while (move != 'r' && move != 'p' && move != 's');
        
        switch(move){
            case 'r': game_msg->rps(0); break;
            case 'p': game_msg->rps(1); break;
            case 's': game_msg->rps(2); break;
        }
    }

    int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) override {
        long move = game_msg->rps();
        long opp_move = opp_game_msg->rps();

        if (move == opp_move) return 0;

        // rock vs paper
        switch(move){
            case 0: // rock
                if (opp_move == 1) return -1; // rock vs paper 
                else return 1; // rock vs scissors
            case 1: // paper
                if (opp_move == 0) return 1; // paper vs rock 
                else return -1; // paper vs scissors
            case 2: // 
                if (opp_move == 0) return -1; // scissors vs rock 
                else return 1; // scissors vs paper
        }
        return 0;
    }

    bool is_end() override{
        return is_full_turn();
    }
};