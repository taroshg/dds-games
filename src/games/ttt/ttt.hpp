#include "../GameWrapper.hpp"
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
public:
    TTT(): GameWrapper() {};

    void get_user_move(GameMessage* my_game_msg, GameMessage* opp_game_msg) override {
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
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "give an input 0-8, 0 being top left and 8 being bottom right: ";
            if (std::cin >> move){
                // makes a occupied board by combining x's and o's
                // inverts 0's to 1's, where 1's rep. empty square
                // checks if intented pos to mark is an empty square
                valid = ~(my_ttt | opp_ttt) & (1 << move);
            }  
        }

        my_game_msg->ttt(my_ttt | (1 << move));
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
};