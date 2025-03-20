#include "GameSubscriber.hpp"
#include "GamePublisher.hpp"

#include <iostream>
#include <thread>
#include <random>
#include <sstream>

void send(GamePublisher* pub, std::string uid){
    Game game;

    while(game.message() != "quit!"){
        game.uid(uid);

        std::string msg;
        
        std::cin >> msg;
        std::cout << "\033[A\33[2K\r";
        
        game.message(msg);
        if(!pub->publish(&game)){
            std::cout << "missed send!";
        }
    }

}

void get(GameSubscriber* sub, std::string uid){
    Game game;
    while(game.message() != "quit!"){
        if(sub->get(&game) && game.uid() != uid){
            std::cout << "msg from " << game.uid() << ": " << game.message() << "\n";
        }
    }
}

std::string generateUUID() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t part1 = dis(gen);
    uint64_t part2 = dis(gen);

    std::stringstream ss;
    ss << std::hex << part1 << "-" << part2;
    return ss.str();
}

int main(){

    GameSubscriber sub;
    GamePublisher pub;

    std::string uid = generateUUID();

    std::cout << "created a user with " << uid;

    if (pub.init() && sub.init()){
        std::thread pub_td(send, &pub, uid);
        std::thread sub_td(get, &sub, uid);

        pub_td.join();
        sub_td.join();
    }
}