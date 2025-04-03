#include "Player.hpp"

// Default constructor
Player::Player() {
    this->id = "";
    this->name = "";
}

// Constructor with ID
Player::Player(std::string id) {
    this->id = id;
    this->name = "";
}

// Getters
std::string Player::getid() {
    return id;
}

std::string Player::getName() {
    return name;
}

std::string Player::getSymbol() {
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
