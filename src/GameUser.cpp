#include "DDSGameController.hpp"

#include <iostream>
#include <thread>
#include <chrono>

class GameWrapper {
protected:
    int my_wins_;
    int opp_wins_;
    const bool end_on_full_turn_;
    int my_moves_;
    int opp_moves_;
public:
    GameWrapper(bool end_on_full_turn) : 
    my_wins_(0),
    opp_wins_(0),
    end_on_full_turn_(end_on_full_turn) {}

    virtual void get_user_move(GameMessage* game_msg) = 0;
    virtual int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) = 0;

    int get_my_wins() const {return my_wins_;}
    int get_opp_wins() const {return opp_wins_;}

    bool end_on_full_turn() const {return end_on_full_turn_;}

    void I_moved() {my_moves_++;}
    void opp_moved() {opp_moves_++;}

    int my_moves() const {return my_moves_;}
    int opp_moves() const {return opp_moves_;} 

    bool is_full_turn() const {return my_moves_ == opp_moves_;}
};

class RPS : public GameWrapper {
public:
    RPS(): GameWrapper(true) {};
    void get_user_move(GameMessage* game_msg) override {
        char move;
        do {
            std::cout << "'r' for rock\n";
            std::cout << "'p' for paper\n";
            std::cout << "'s' for scissors\n";
            std::cout << "Enter your choice: ";
            std::cin >> move;
        } while (move != 'r' && move != 'p' && move != 's');
        
        switch(move){
            case 'r': game_msg->rps(0); break;
            case 'p': game_msg->rps(1); break;
            case 's': game_msg->rps(2); break;
        }
    }

    int get_winner(GameMessage* game_msg, GameMessage* opp_game_msg) override {
        long move = game_msg->rps();
        long opp_move = opp_game_msg->rps();

        if (move == opp_move) return 0;

        // rock vs paper
        switch(move){
            case 0: // rock
                if (opp_move == 1) return -1; // rock vs paper 
                else return 1; // rock vs scissors
            case 1: // paper
                if (opp_move == 0) return 1; // paper vs rock 
                else return -1; // paper vs scissors
            case 2: // 
                if (opp_move == 0) return -1; // scissors vs rock 
                else return 1; // scissors vs paper
        }
        return 0;
    }
};

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
            while(my_controller_.message_count() < 2){
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
                std::cout << "you go first!" << std::endl;
            }

            return true;
        }
        return false;
    }

    void play(int nrounds){
        int round = 0;
        GameMessage* opp_game_msg = new GameMessage();
        GameMessage* my_game_msg = new GameMessage();

        while(round < nrounds){
            // read if there is a new message and is from opp
            if (my_controller_.message_count() > last_message_count_){
                opp_game_msg = my_controller_.read();
                last_message_count_ = my_controller_.message_count(); 
                
                if (opp_game_msg->uid() != oid_) continue;

                std::cout << "opp moved!" << std::endl;
                turn_ = true;
                game_api_->opp_moved();

                // rps specific!!
                if (game_api_->is_full_turn() && game_api_->end_on_full_turn()){
                    std::cout << "-------- round " << round << " -------" << std::endl;
                    // print moves
                    std::cout << "my (" << my_game_msg->uid() << ") move: " << my_game_msg->rps() << std::endl;
                    std::cout << "opp (" << opp_game_msg->uid() << ") move: " << opp_game_msg->rps() << std::endl;

                    round++;
                    int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                    if(score > 0) std::cout << "--- you won! ---" << std::endl;
                    else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                    else std::cout << "--- tie ---" << std::endl;
                }
            }
            
            if (turn_){
                my_game_msg->uid(uid_);

                game_api_->get_user_move(my_game_msg);
                game_api_->I_moved();

                // after my move, if there is opp move
                if (my_controller_.last_uid() == oid_){

                    // rps specific!! (determine winner)
                    if (game_api_->is_full_turn() && game_api_->end_on_full_turn()){
                        std::cout << "-------- round " << round << " -------" << std::endl;
                        // print moves
                        std::cout << "my (" << my_game_msg->uid() << ") move: " << my_game_msg->rps() << std::endl;
                        std::cout << "opp (" << opp_game_msg->uid() << ") move: " << opp_game_msg->rps() << std::endl;

                        round++;
                        int score = game_api_->get_winner(my_game_msg, opp_game_msg);
                        if(score > 0) std::cout << "--- you won! ---" << std::endl;
                        else if(score < 0) std::cout << "--- you lose :( ---" << std::endl;
                        else std::cout << "--- tie ---" << std::endl;
                    }
                }

                // publish my move
                if(my_controller_.publish(my_game_msg)){
                    turn_ = false;
                }
            }
        }
    }
};
int main(){
    GameWrapper* rps = new RPS();
    GameUser* user = new GameUser(rps);
    if(user->init()){
        user->play(3);
    }
}