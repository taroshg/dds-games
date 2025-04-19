#ifndef DDS_GAME_USER
#define DDS_GAME_USER

#include "DDSGameController.hpp"
#include "./games/GameWrapper.hpp"
#include "./games/rps/rps.hpp"

#include <thread>
#include <chrono>

class GameUser{
private:
    unsigned long last_message_count_; // used to keep track of when to read
    DDSGameController my_controller_;
    int current_game_id_ = -1; // -1 for when no game is selected

public:
    std::string uid_; // my ID
    std::string oid_; // opponent ID

    bool turn_;
    bool first_;
    GameUser(): last_message_count_(0), 
                                 turn_(false) {}

    bool init(){
        if (my_controller_.init()){
            while(my_controller_.n_publishers() < 2){
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            uid_ = my_controller_.uid();
            std::cout << "ME: " << uid_ << std::endl;

            GameMessage game_msg;
            game_msg.uid(uid_);
            game_msg.message("connected!");
            if (!my_controller_.publish(&game_msg)) return false;
            
            // wait until there are more than 1 broadcast, until another player joins
            while(my_controller_.participants().size() < 2){
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            
            // participants are going to be in the order of first joined
            int i = 0;
            for (std::string id : my_controller_.participants()){
                if (id != uid_){
                    oid_ = id;
                    break;
                }
                i++;
            }
            std::cout << "OPP: " << oid_ << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // if you are first participant, you start!
            turn_ = first_ = my_controller_.participants()[0] == uid_;

            last_message_count_ = my_controller_.message_count();

            if (first_){
                std::cout << "you go first!" << std::endl;
                std::cout << "select a game..." << std::endl;
            }
            else std::cout << "waiting for game selection..." << std::endl;

            return true;
        }
        return false;
    }

    bool selectGame(int game_id){
        GameMessage* msg = new GameMessage();
        msg->game_id(game_id);
        msg->message("game selection");
        if (my_controller_.publish(msg)){
            current_game_id_ = game_id;
            return true;
        }
        std::cout << "unable to publish game selection" << std::endl;
        return false;
    }

    int currentGameID(){
        return current_game_id_;
    }

    bool hasReceivedGameChoice(){
        if (current_game_id_ > 0) return true;
        // check the last message's game_id
        // Note: if a third player were to join, then they would be eaves drop, and join game in the middle
        if (my_controller_.message_count() > last_message_count_){
            GameMessage* opp_msg = my_controller_.read();
            last_message_count_ = my_controller_.message_count(); 

            current_game_id_ = opp_msg->game_id();
            std::cout << "running game_id: " << current_game_id_ << std::endl;
            return true;
        }
        return false;
    }

    void playCLI(GameWrapper* game_api_, int nrounds){
        int round = 1;
        // extra round to keep user active to publish msg
        nrounds = (first_) ? nrounds : 1 + nrounds;

        // important to set for the game logic
        game_api_->set_first(first_);

        GameMessage* opp_game_msg = new GameMessage();
        GameMessage* my_game_msg = new GameMessage();
        while(round < 1 + nrounds){
            // read if there is a new message and is from opp
            if (my_controller_.message_count() > last_message_count_){
                opp_game_msg = my_controller_.read();
                last_message_count_ = my_controller_.message_count(); 
                
                std::cout << "opp moved!" << std::endl;
                turn_ = true;
                game_api_->opp_moved();

                if (game_api_->is_end(my_game_msg, opp_game_msg)){
                    std::cout << "-------- round " << round << " -------" << std::endl;
                    round++;
                    int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                    if(score > 0) std::cout << "--- you won! ---" << std::endl;
                    else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                    else std::cout << "--- tie ---" << std::endl;
                }

            }
            
            else if (turn_){
                my_game_msg->uid(uid_);

                game_api_->get_user_move(my_game_msg, opp_game_msg);
                game_api_->I_moved();

                // publish my move
                if(my_controller_.publish(my_game_msg)){
                    turn_ = false;
                }

                if (game_api_->is_end(my_game_msg, opp_game_msg)){
                    std::cout << "-------- round " << round << " -------" << std::endl;
                    round++;
                    int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                    if(score > 0) std::cout << "--- you won! ---" << std::endl;
                    else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                    else std::cout << "--- tie ---" << std::endl;
                }
            }
        }
    }

    unsigned long messageCount(){
        return my_controller_.message_count();
    }

    unsigned long lastMessageCount(){
        return last_message_count_;
    }

    bool sendGameMessage(GameMessage* msg){
        msg->uid(uid_);
        if (my_controller_.publish(msg)){
            turn_ = false;
            return true;
        }
        return false;
    }
    bool messageAvailable(){
        return my_controller_.message_count() > last_message_count_;
    }

    GameMessage* readGameMessage(){
        if (messageAvailable()){
            last_message_count_ = my_controller_.message_count();
            turn_ = true;
            return my_controller_.read();
        }
        return nullptr;
    }
};
#endif