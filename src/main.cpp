#include "GameUser.hpp"
#include "./games/ttt.hpp"
#include "./games/rps.hpp"

int main(){
    GameWrapper* rps = new RPS();
    GameWrapper* ttt = new TTT();
    GameUser* user = new GameUser();
    if(user->init()){
        user->playCLI(rps, 3);
    }
}