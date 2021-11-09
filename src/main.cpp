#include <RGSDL/Engine.h>
#include <RGSDL/Scene.h>
#include <RGSDL/Utils.h>

#include "./scenes/MainScene.h"

#include <cstdlib>

using namespace RGSDL;

int main( int argc, char** argv )
{
    if ( argc != 2 ) {
        Error( "Usage: " << argv[ 0 ] << " profile INI file" );
        return 0;
    }

    Utils::IniType* ini = new Utils::IniType();
    if ( !Utils::readIni( std::string( argv[ 1 ] ), *ini ) ) return -1;

    Utils::IniGroup win = Utils::readIniGroup( *ini, "window" );
    if(win.size() == 0) {
        Error("profile must contain a '[window]' group");
        delete ini;
        return -1;
    } 
    ini->erase( ini->find( "window" ) );

    int winWidth  = Utils::readIniGroupInt( win, "width", 100 );
    int winHeight = Utils::readIniGroupInt( win, "height", 100 );

    Scenes::MainScene ms =
        Scenes::MainScene( ini, (float)winWidth / 100.0f, (float)winHeight / 100.0f );

    Engine game;

    int ret = game.start(
        argc, argv, winWidth, winHeight, 96.0f, "MyMacroPad", &ms, 0, SDL_WINDOW_ALWAYS_ON_TOP, 0 );

    delete ini;

    return ret;
}
