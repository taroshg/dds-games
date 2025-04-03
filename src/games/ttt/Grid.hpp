#include <string>

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
    std::string gridSymbols;

    void Grid::updateGridSymbols()
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

    /*
    * There are 3 states for each grid point in the 3x3 tic tac toe grid
    * with 9 grid points that means there are 3^9 ways or 19683 ways to
    * fill the grid in a single game. What this effectively means is any
    * state of the grid can be represented with a single integer
    */
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
        pos--; // This is the only method that directly interacts with user input so we decrement the input position since it is one ahead of how the actual grid string is indexed
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

    std::string getGridSymbol(int pos)
    {
        std::string symbol = "a";
        if (pos > -1 || pos < gridSymbols.size()) { //Check if the pos provided is actually in proper range
            symbol = gridSymbols[pos];
        }
        return symbol;
    }

    std::string getGrid()
    {
        return
        getGridSymbol(0) + "|" + getGridSymbol(1) + "|" + getGridSymbol(2) + "\n" +
        getGridSymbol(3) + "|" + getGridSymbol(4) + "|" + getGridSymbol(5) + "\n" +
        getGridSymbol(6) + "|" + getGridSymbol(7) + "|" + getGridSymbol(8);
    }

    std::string getExampleGrid()
    {
        return "1|2|3\n4|5|6\n7|8|9";
    }

    bool verifyGridPosEmpty(int pos)
    { 
    bool outcome = false;
        if(pos >= 0 && pos < 9 && getGridSymbol(pos) == " ") { // Check if pos is a valid index and then check if associated string position is empty
            outcome = true;
        }
        return outcome;
    }

    /**
     * Verify wins by checking known substrings of the grid string that could
     * be potential winning paterns
     */
    bool verifyWin()
    {
        bool won = false;
        std::string temp;
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