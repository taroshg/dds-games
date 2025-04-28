#include "GameWrapper.hpp"
#include <iostream>
#include <bitset>

class C4 : public GameWrapper {
private:
    uint64_t my_board;
    uint64_t opp_board;
public:
    C4(): GameWrapper(), my_board(0), opp_board(0) {};

    ~C4(){}

    void get_user_move_CLI(GameMessage* my_game_msg, GameMessage* opp_game_msg) override {
        int move; // pos of square marked
        my_board = my_game_msg->c4();
        opp_board = opp_game_msg->c4();

        std::cout << boardString();

        // get user move here
        bool valid = false;
        while(!valid){            
            std::cout << "give an input 0-6 columns: ";
            std::cin >> move;

            if (std::cin.fail() || move < 0 || move > 6){
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            
            valid = make_move(move);
        }

        my_game_msg->c4(my_board);
    }

    bool make_move(int col){
        int top_bit = 5 * 7 + col;
        if (occupied() & (1UL << top_bit)) return false;

        for (int row = 0; row < 6; ++row){
            int bit = row * 7 + col;
            if(free() & (1UL << bit)){
                my_board = (my_board | (1UL << bit));
                return true;
            }
        }

        return false;
    }

    std::string boardString(){
        std::stringstream out;

        char mychar = first_ ? 'r' : 'b'; // i'm x if i'm first to move
        char oppchar = first_ ? 'b' : 'r'; // opp is o if i'm first to move

        for (int r = 5; r >= 0; --r){
            for (int c = 0; c < 7; ++c){
                int bit = r * 7 + c;
                if (my_board & (1UL << bit))
                    out << mychar;
                else if (opp_board & (1UL << bit))
                    out << oppchar;
                else
                    out << '.';
                out << ' ';
            }
            out << '\n';
        }

        return out.str();
    }

    unsigned long occupied(){
        return my_board | opp_board;
    }

    unsigned long free(){
        return (~occupied()) & (1UL << 42) - 1;
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
        unsigned long my_c4 = game_msg->c4();
        unsigned long opp_c4 = opp_game_msg->c4();

        if (check_win(my_c4)) return 1;
        if (check_win(opp_c4)) return -1;
        
        return 0;
    }

    bool check_win(unsigned long bitboard){
        // Horizontal win
        if (bitboard & (bitboard >> 1) & (bitboard >> 2) & (bitboard >> 3))
            return true;
        // Vertical win
        if (bitboard & (bitboard >> 7) & (bitboard >> 14) & (bitboard >> 21))
            return true;
        // Diagonal (\) win
        if (bitboard & (bitboard >> 6) & (bitboard >> 12) & (bitboard >> 18))
            return true;
        // Diagonal (/) win
        if (bitboard & (bitboard >> 8) & (bitboard >> 16) & (bitboard >> 24))
            return true;

        return false;
    }

    bool is_end(GameMessage* game_msg, GameMessage* opp_game_msg) override{
        //  (42 bits set to 1) = (0x3FFFFFFFFFF) = (1UL << 42) - 1
        int winner = get_winner(game_msg, opp_game_msg);
        // if there is no winner and the board is filled
        if (winner != 0) 
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