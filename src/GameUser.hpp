#include "DDSGameController.hpp"
#include "./games/GameWrapper.hpp"
#include "./games/rps/RPS.hpp"

#include <thread>
#include <chrono>

class GameUser{
private:
    unsigned long last_message_count_;
    DDSGameController my_controller_;
    GameWrapper* game_api_;
    
public:
    std::string uid_; // my ID
    std::string oid_; // opponent ID

    bool turn_;
    GameUser(GameWrapper* game_api): last_message_count_(0), 
                                 turn_(false) {
                                    game_api_ = game_api;
                                 }

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
            turn_ = my_controller_.participants()[0] == uid_;

            last_message_count_ = my_controller_.message_count();

            if (turn_){
                game_api_->is_first_to_play();
                std::cout << "you go first!" << std::endl;
            }

            return true;
        }
        return false;
    }

    void play(int nrounds){
        int round = 0;
        // extra round to keep user active to publish msg
        nrounds = (turn_) ? nrounds : nrounds + 1;
        GameMessage* opp_game_msg = new GameMessage();
        GameMessage* my_game_msg = new GameMessage();
        while(round < nrounds){
            // read if there is a new message and is from opp
            if (my_controller_.message_count() > last_message_count_){
                opp_game_msg = my_controller_.read();
                last_message_count_ = my_controller_.message_count(); 
                
                // blocked until msg is from opp
                if (opp_game_msg->uid() != oid_) continue;

                std::cout << "opp moved!" << std::endl;
                turn_ = true;
                game_api_->opp_moved();

                if (game_api_->is_end(my_game_msg, opp_game_msg)){
                    std::cout << "-------- round " << round << " -------" << std::endl;
                    round++;
                    // print moves
                    int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                    if(score > 0) std::cout << "--- you won! ---" << std::endl;
                    else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                    else std::cout << "--- tie ---" << std::endl;
                }
            }
            
            if (turn_){
                my_game_msg->uid(uid_);

                game_api_->get_user_move(my_game_msg, opp_game_msg);
                game_api_->I_moved();

                if (my_controller_.last_uid() == oid_ && game_api_->is_end(my_game_msg, opp_game_msg)){
                    std::cout << "-------- round " << round << " -------" << std::endl;
                    round++;
                    int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                    if(score > 0) std::cout << "--- you won! ---" << std::endl;
                    else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                    else std::cout << "--- tie ---" << std::endl;
                }

                // publish my move
                if(my_controller_.publish(my_game_msg)){
                    turn_ = false;
                }
            }
        }
    }
};