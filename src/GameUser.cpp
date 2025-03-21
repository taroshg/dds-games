#include "GameSubscriber.hpp"
#include "GamePublisher.hpp"

#include <iostream>
#include <thread>
#include <random>
#include <sstream>

bool send(GamePublisher* pub, Game* game, std::string uid){
    game->uid(uid);

    std::string msg;
    
    std::getline(std::cin, msg);
    std::cout << "\033[A\33[2K\r";
    
    game->message(msg);
    if(pub->publish(game)){
        return true;
    }
    std::cout << "missed send!";
    return false;
}

bool get(GameSubscriber* sub, Game* game, std::string uid){
    if(sub->get(game) && game->uid() != uid){
        std::cout << "msg from " << game->uid() << ": " << game->message() << "\n";
        return true;
    }
    return false;
}

std::atomic<bool> running(true);

void send_worker(GamePublisher* pub, Game* game, std::string uid){
    while (running){
        if (game->message() == "quit!"){
            running = false;
            break;
        }
        send(pub, game, uid);
    }
}

void get_worker(GameSubscriber* sub, Game* game, std::string uid){
    while (running){
        get(sub, game, uid);
    }
}

std::string generateUUID() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    uint64_t uuid = dis(gen);

    std::stringstream ss;
    ss << std::hex << uuid;
    return ss.str();
}

int main(){

    GameSubscriber sub;
    GamePublisher pub;

    std::string uid = generateUUID();

    std::cout << "created a user with " << uid;

    Game myGame;
    Game oppGame;

    bool sent = false;
    bool received = false;

    bool run = true;
    if (pub.init() && sub.init()){
    
        std::thread pub_td(send_worker, &pub, &myGame, uid);
        std::thread sub_td(get_worker, &sub, &oppGame, uid);

        pub_td.join();
        sub_td.join();
    }
}