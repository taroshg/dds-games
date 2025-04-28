#include "GameUser.hpp"
#include "./games/ttt.hpp"
#include "./games/rps.hpp"
#include "./games/c4.hpp"

int main(){
    GameWrapper* c4 = new C4();
    GameUser* user = new GameUser();
    if(user->init()){
        user->playCLI(c4, 3);
    }
}