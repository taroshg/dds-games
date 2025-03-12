#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

class Player {
    private:
        string id;
        string name;
        string symbol;

    public:
        Player() {
            this->id = "";
            this->name = "";
        }

        Player(string id) {
            this->id = id;
            this->name = "";
        }

        string getid() {
            return id;
        }

        string getName() {
            return name;
        }

        string getSymbol() {
            return symbol;
        }

        void setSymbol(string symbol) {
            this->symbol = symbol;
        }

        void setName(string name) {
            this->name = name;
        }

        bool isO() {
            return symbol == "O";
        }
};

/*
 * There are 3 states for each grid point in the 3x3 tic tac toe grid
 * with 9 grid points that means there are 3^9 ways or 19683 ways to
 * fill the grid in a single game. What this effectively means is any
 * state of the grid can be represented with a single integer
 */

 class Grid
 {
     private:
         int gridstate;
         string gridSymbols;
 
         // Update gridSymbols 
         void updateGridSymbols()
         {
             int num = gridstate;
             int remainder;
             gridSymbols = "";
             int i = 0;

             while(i < 9) {
                 remainder = num % 3;
                 if(remainder == 0) { 
                     gridSymbols.append(" "); }
                 else if(remainder == 1) {
                     gridSymbols.append("O"); }
                 else {
                     gridSymbols.append("X"); }
                 num /= 3;
                 i++;
             }
         }
 
     public:
         Grid()
         {
             gridstate = 0;
             gridSymbols = "         ";
         }
 
         /**
          * @param pos the position on the grid you wish to update
          * @param isO whether or not an O or and X is being added to the grid
          * @return whether or not that position is filled
          */
         bool addToGrid(int pos, bool isO)
         {
             bool canBePlaced = verifyGridPosEmpty(pos);
             if(canBePlaced) {
                 if(isO) {
                     gridstate += powers(3, pos); // To add ternaries of just 1 it's just adding an exponetial of three in the corresponding postion
                 }
                 else {
                     gridstate += 2*powers(3, pos); // To add ternaries of just 2 it's the same but you add the power of three multiplied by 2
                 }
                 updateGridSymbols();
             }
             return canBePlaced;
         }
 
         void setGridState(int newState) {
             gridstate = newState;
             updateGridSymbols();
         }
 
         string getGridSymbol(int pos)
         {
            string symbol = "a";
            if (pos > -1 || pos < gridSymbols.size()) { //Check if the pos provided is actually in proper range
                symbol = gridSymbols[pos];
            }
            return symbol;
         }
 
         string getGrid()
         {
             return
             getGridSymbol(0) + "|" + getGridSymbol(1) + "|" + getGridSymbol(2) + "\n" +
             getGridSymbol(3) + "|" + getGridSymbol(4) + "|" + getGridSymbol(5) + "\n" +
             getGridSymbol(6) + "|" + getGridSymbol(7) + "|" + getGridSymbol(8);
         }
 
         string getExampleGrid()
         {
             return "1|2|3\n4|5|6\n7|8|9";
         }
 
         bool verifyGridPosEmpty(int pos)
         { 
            bool outcome = false;
             if(getGridSymbol(pos) == " ") {
                 outcome = true;
             }
             return outcome;
         }
 
         bool verifyWin()
         {
             bool won = false;
             string temp;
             int i = 0;
             //Horizontal substrings
             while(i < 3 & !won)
             {
                 temp = getGridSymbol(i*3) + getGridSymbol(i*3+1) + getGridSymbol(i*3+2);
                 if (temp == "XXX" | temp == "OOO") {
                     won = true;
                 }
                 i++;
             }
             //Vertical substrings
             i = 0;
             while(i < 3 & !won)
             {
                 temp = getGridSymbol(i) + getGridSymbol(i+3) + getGridSymbol(i+6);
                 if (temp == "XXX" | temp == "OOO") {
                     won = true;
                 }
                 i++;
             }
             // Diagonal Substrings
             i = 0;
             while(i < 2 & !won)
             {
                 temp = getGridSymbol((2*i)) + getGridSymbol(4) + getGridSymbol(8-(2*i));
                 if (temp == "XXX" | temp == "OOO") {
                     won = true;
                 }
                 i++;
             }

             return won;
         }
 
         int powers(int num, int exponent)
         {
             int result = 1;
             for(int i = 0; i < exponent; i++) {
                 result *= num;
             }
             return result;
         }
 
         void resetGrid()
         {
             gridstate = 0;
             gridSymbols = "         ";
         }
 };

class Game {
    private:
        Player p1{"1"};
        Player p2{"2"};
        Grid gameGrid;
        Player currentPlayer;

    public:           

        void run() {
            int running = 1;
            string input = "";
            
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
                cout << "Hit the maximum number of turns" << endl;
            }
            if(won) {
                cout << "Player " + currentPlayer.getName() + " wins!\n" + gameGrid.getGrid() << endl;
            }
        }

        void getUsernames() {
            string temp1;
            string temp2;

            cout << "Enter the username for player 1: " << endl;
            cin >> temp1;
            cout << "Enter the username for player 2: " << endl;
            cin >> temp2;
            
            system("clear");

            p1.setName(temp1);
            p2.setName(temp2);
        }

        void takeTurn() {
            bool finished = false;
            int pos;
            
            system("clear");
            cout << "It is Player " + currentPlayer.getid() + " " + currentPlayer.getName() + "'s Turn Symbol: " + currentPlayer.getSymbol() << endl; // Display turn info 
            cout << "Please choose a grid point from 1-9 that isn't empty: " << endl; // Remind user that input needs to be in range of 1-9
            cout << "Current Grid:\n" + gameGrid.getGrid() << endl; // give a view of the current tic tac toe grid
            
            bool choice = false;

            while(!choice) {
                cin >> pos;
                
                choice = gameGrid.addToGrid(pos - 1, currentPlayer.isO());    
            
                if(!choice)
                {
                    cin.ignore();
                    system("clear");
                    cout << "Please choose a grid point from 1-9 that isn't empty: " << endl;
                    cout << "Example Grid:\n" + gameGrid.getExampleGrid() + "\n" << endl; // give of annotated tic tac toe grid
                    cout << "Current Grid:\n" + gameGrid.getGrid() << endl; // give a view of the current tic tac toe grid
                }
            }

            system("clear");
        }
};


int main() {
    Game game;
    game.run();
}