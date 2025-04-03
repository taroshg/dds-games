#include <string>

class Player{
private:
    std::string id;
    std::string name;
    std::string symbol;

public:
    // Default constructor
    Player() {
        this->id = "";
        this->name = "";
    }

    // Constructor with ID
    Player(std::string id) {
        this->id = id;
        this->name = "";
    }

    // Getters
    std::string getid() {
        return id;
    }

    std::string getName() {
        return name;
    }

    std::string getSymbol() {
        return symbol;
    }

    // Setters
    void Player::setSymbol(std::string symbol) {
        this->symbol = symbol;
    }

    void Player::setName(std::string name) {
        this->name = name;
    }

    // Check if symbol is "O"
    bool Player::isO() {
        return symbol == "O";
    }

};