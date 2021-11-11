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
    int ret = 0;
    std::string nextWindow = argv[ 1 ];

    while ( nextWindow.length() > 0 ) {

        std::vector<std::string> iniPath;
        Utils::stringSplit(nextWindow, "/", iniPath, false);

        Utils::IniType* ini = new Utils::IniType();
        if ( !Utils::readIni( nextWindow, *ini ) ) return -1;

        Utils::IniGroup win = Utils::readIniGroup( *ini, "window" );
        if ( win.size() == 0 ) {
            Error( "profile must contain a '[window]' group" );
            delete ini;
            return -1;
        }
        ini->erase( ini->find( "window" ) );

        int winWidth  = Utils::readIniGroupInt( win, "width", 100 );
        int winHeight = Utils::readIniGroupInt( win, "height", 100 );

        Scenes::MainScene* ms =
            new Scenes::MainScene( ini, (float)winWidth / 100.0f, (float)winHeight / 100.0f );

        Engine game;

        ret = game.start(
            argc, argv, winWidth, winHeight, 96.0f, ("AMP - " + iniPath[iniPath.size()-1]).c_str(), ms, 0, SDL_WINDOW_ALWAYS_ON_TOP,
            0 );

        nextWindow = ms->getNextProfile();

        delete ini;
        delete ms;
    }

    return ret;
}
