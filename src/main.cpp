#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "GameUser.hpp"
#include "./games/ttt/ttt.hpp"

int main(){
    GameWrapper* rps = new RPS();
    GameWrapper* ttt = new TTT();
    GameUser* user = new GameUser();
    if(user->init()){
        user->playCLI(rps, 3);
    }
}