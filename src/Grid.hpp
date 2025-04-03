#ifndef GRID_HPP
#define GRID_HPP

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

        void updateGridSymbols();

    public:
        Grid();

        /**
         * @param pos the position on the grid you wish to update
         * @param isO whether or not an O or and X is being added to the grid
         * @return whether or not that position is filled
         */
        bool addToGrid(int pos, bool isO);

        void setGridState(int newState);

        std::string getGridSymbol(int pos);

        std::string getGrid();

        std::string getExampleGrid();

        bool verifyGridPosEmpty(int pos);

        bool verifyWin();

        int powers(int num, int exponent);

        void resetGrid();
};

#endif // PLAYER_HPP