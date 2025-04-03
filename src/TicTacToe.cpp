#include <iostream>
#include <string>
#include <cstdlib>
#include "Player.hpp"
#include "Grid.hpp"


class Game {
    private:
        Player p1{"1"};
        Player p2{"2"};
        Grid gameGrid;
        Player currentPlayer;

    public:           

        void run() {
            int running = 1;
            std::string input = "";
            
            getUsernames(); // Likely going to remove this since user creation will likely be handled outside of the game in the finall product
            
            gameLogic();
        }

        void gameLogic() {
            int leadPlayer = 1; // Add Random Selection here
            bool won = false;
            int turns = 1;
            gameGrid.resetGrid();
            
            // Pre game setup
            p1.setSymbol("O");
            p2.setSymbol("X");
            if (leadPlayer == 1) {
                currentPlayer = p1;
            }
            else {
                currentPlayer = p2;
            }

            while(turns < 10 & !won)
            {
                won = gameGrid.verifyWin();

                takeTurn();
                
                won = gameGrid.verifyWin();
                //Turn progression
                if(!won && currentPlayer.getid() == "1") {
                    currentPlayer = p2;
                }
                else if (!won) {
                    currentPlayer = p1;
                }
                turns++;
            }
            if(turns == 10)
            {
                std::cout << "Hit the maximum number of turns" << std::endl;
            }
            if(won) {
                std::cout << "Player " + currentPlayer.getName() + " wins!\n" + gameGrid.getGrid() << std::endl;
            }
        }

        void getUsernames() {
            std::string temp1;
            std::string temp2;

            std::cout << "Enter the username for player 1: " << std::endl;
            std::cin >> temp1;
            std::cout << "Enter the username for player 2: " << std::endl;
            std::cin >> temp2;
            
            system("clear");

            p1.setName(temp1);
            p2.setName(temp2);
        }

        void takeTurn() {
            bool finished = false;
            int pos;
            
            system("clear");
            std::cout << "It is Player " + currentPlayer.getid() + " " + currentPlayer.getName() + "'s Turn Symbol: " + currentPlayer.getSymbol() << std::endl; // Display turn info 
            std::cout << "Please choose a grid point from 1-9 that isn't empty: " << std::endl; // Remind user that input needs to be in range of 1-9
            std::cout << "Current Grid:\n" + gameGrid.getGrid() << std::endl; // give a view of the current tic tac toe grid

            while(!(std::cin >> pos) || !gameGrid.addToGrid(pos, currentPlayer.isO())) {
                system("clear");
                std::cout << "It is Player " + currentPlayer.getid() + " " + currentPlayer.getName() + "'s Turn Symbol: " + currentPlayer.getSymbol() << std::endl; // Display turn info 
                std::cout << "Please choose a grid point from 1-9 that isn't empty: " << std::endl;
                std::cout << "Example Grid:\n" + gameGrid.getExampleGrid() + "\n" << std::endl; // give of annotated tic tac toe grid
                std::cout << "Current Grid:\n" + gameGrid.getGrid() + "\n" << std::endl; // give a view of the current tic tac toe grid
                std::cin.clear();
                std::cin.ignore(123, '\n');
            }

            system("clear");
        }
};


int main() {
    Game game;
    game.run();
}