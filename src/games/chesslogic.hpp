#include "GameWrapper.hpp"
#include <iostream>
#include <bitset>
#include "chess.hpp"

using namespace chess;

class Chess : public GameWrapper {
private:
    Board* board_;
public:
    Chess(): GameWrapper(), board_(new Board()){};

    ~Chess(){}

    void get_user_move_CLI(GameMessage* my_game_msg, GameMessage* opp_game_msg) override {
        u_int8_t opp_from = opp_game_msg->chess_from();
        u_int8_t opp_to = opp_game_msg->chess_to();

        std::cout << boardString();

        // get user move here
        bool valid = false;
        uint8_t from;
        uint8_t to;
        while(!valid){            
            std::cout << "from: ";
            std::cin >> from;
            std::cout << std::endl;

            std::cout << "to: ";
            std::cin >> to;
            std::cout << std::endl;
            
            valid = make_move(from, to);
        }

        my_game_msg->chess_from(from);
        my_game_msg->chess_to(to);
    }

    bool make_move(uint8_t from, uint8_t to){
        Movelist movelist;
        movegen::legalmoves(movelist, *board_);

        Move m = Move::make<Move::NORMAL>(from, to);

        if (movelist.find(m) > -1){
            board_->makeMove(m);
            return true;
        }

        return false;
    }

    std::string boardString(){
        return board_->getFen();
    }

    int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) override {
        std::pair<GameResultReason, GameResult> result = board_->isGameOver();
        if (result.second == GameResult::NONE) return 0;
        if (result.second == GameResult::DRAW) return 0;
        if (result.second == GameResult::WIN && board_->sideToMove() == Color::WHITE) 
            return first_ ? 1 : -1;
        else return first_ ? -1 : 1;
    }

    bool is_end(GameMessage* game_msg, GameMessage* opp_game_msg) override{        
        std::pair<GameResultReason, GameResult> result = board_->isGameOver();
        return result.first != GameResultReason::NONE;
    }

    void reset(){
        board_ = new Board();
        opp_moves_ = 0;
        my_moves_ = 0;
    }
};