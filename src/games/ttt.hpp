#include "GameWrapper.hpp"
#include <iostream>
#include <bitset>

class TTT : public GameWrapper {
private:
    long WIN_STATE_MASK[8] = {448,   // top row
                                56,  // middle row
                                7,   // bottom row
                                292, // left col
                                146, // middle col
                                73,  // right col
                                273, // right-left diag.
                                84}; // left-right diag.

    unsigned long my_board;
    unsigned long opp_board;
public:
    TTT(): GameWrapper(), my_board(0), opp_board(0) {};

    ~TTT(){}

    void get_user_move_CLI(GameMessage* my_game_msg, GameMessage* opp_game_msg) override {
        int move; // pos of square marked
        unsigned long my_ttt = my_game_msg->ttt();
        unsigned long opp_ttt = opp_game_msg->ttt();

        // print board here
        char mychar = first_ ? 'x' : 'o'; // i'm x if i'm first to move
        char oppchar = first_ ? 'o' : 'x'; // opp is o if i'm first to move

        for(int i = 0; i < 9; ++i){
            if (my_ttt & (1 << i))
                std::cout << mychar;
            else if (opp_ttt & (1 << i))
                std::cout << oppchar;
            else
                std::cout << '.';

            if (i % 3 == 2)
                std::cout << '\n';
            else
                std::cout << ' ';
        }
        std::cout << std::endl;

        // get user move here
        bool valid = false;
        while(!valid){            
            std::cout << "give an input 0-8, 0 being top left and 8 being bottom right: ";
            std::cin >> move;

            if (std::cin.fail()){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            
            valid = make_move(move);
        }

        my_game_msg->ttt(my_board);
    }

    bool make_move(int pos){
        // if pos is in a open space
        if (valid_move(pos)){
            // place my marker on pos
            my_board = (my_board | (1 << pos));
            return true;
        }
        return false;
    }

    bool valid_move(int pos){
        return ~(occupied()) & (1 << pos);
    }

    std::string boardString(){
        std::stringstream out;
        for(int i = 0; i < 9; ++i){
            if (my_board & (1 << i))
                out << 'x';
            else if (opp_board & (1 << i))
                out << 'o';
            else
                out << '.';

            if (i % 3 == 2)
                out << '\n';
            else
                out << ' ';
        }
        return out.str();
    }

    unsigned long occupied(){
        return my_board | opp_board;
    }

    unsigned long myState(){
        return my_board;
    }

    unsigned long oppState(){
        return opp_board;
    }

    void setOppState(unsigned long opp_state){
        opp_board = opp_state;
    }

    int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) override {
        unsigned long my_ttt = game_msg->ttt();
        unsigned long opp_ttt = opp_game_msg->ttt();

        for (int i = 0; i < 8; ++i){
            if ((my_ttt & WIN_STATE_MASK[i]) == WIN_STATE_MASK[i]){
                return 1;
            }
            if ((opp_ttt & WIN_STATE_MASK[i]) == WIN_STATE_MASK[i]){
                return -1;
            }
        }
        return 0;
    }

    bool is_end(GameMessage* game_msg, GameMessage* opp_game_msg) override{
        int winner = get_winner(game_msg, opp_game_msg);
        // if there is no winner and the board is filled
        if (winner != 0 || (opp_moves_ + my_moves_) > 7) 
            return true;

        return false;
    }

    void reset(){
        my_board = 0;
        opp_board = 0;
        opp_moves_ = 0;
        my_moves_ = 0;
    }
};