#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player
{
    private:
        std::string id;
        std::string name;
        std::string symbol;

    public:
        Player();
        Player(std::string id);

        std::string getid();
        std::string getName();
        std::string getSymbol();

        void setSymbol(std::string symbol);
        void setName(std::string name);

        bool isO();
};

#endif // PLAYER_HPP
