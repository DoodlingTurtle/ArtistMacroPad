#include "./MainScene.h"
#include "../components/ButtonManager.h"
#include <RGSDL/Utils.h>

namespace Scenes {
    MainScene::MainScene( const RGSDL::Utils::IniType* ini, float ww, float wh )
        : RGSDL::Scene( true ), ini( ini ), prefix( "" ),
          suffix( " mousemove_relative --sync 1 1) &" ), windowid( "" ), winWidthMultiplyer( ww ),
          winHeightMultiplyer( wh ), nextProfile( "" ), mouseHistory()
    {
        mouseHistory.push_back( { 0, 0 } );
    }

    bool MainScene::onStart( RGSDL::Engine* game )
    {
        // Separate Mouse and Touch
        game->mergeMouseAndTouch( false );

        font                  = game->loadTexture( "./assets/gnsh-bitmapfont-colour8.png" );
        button_label          = game->createTextlayer( font, "Label", 5, 12, 12, 3 );

        Components::ButtonManager::registerButton(
            game, winWidthMultiplyer, winHeightMultiplyer, button_label, *ini );

        return true;
    }

    RGSDL::Scene* MainScene::onEnd( RGSDL::Engine* game )
    {
        game->destroyTextlayer( button_label );
        game->destroyTexture( font );
        Components::ButtonManager::clearAll();

        return nullptr;
    }

    bool MainScene::onUpdate( RGSDL::Engine* game, float deltaTime )
    {
        int         x, y;
        std::string command    = "";
        bool        resetMouse = true;

        SDL_GetGlobalMouseState( &x, &y );

        if ( Components::ButtonManager::updateAll( game, command, resetMouse ) ) {
            if ( command.length() > 0 ) {
                if ( command.at( 0 ) == '#' ) {
                    nextProfile = command.substr( 1 );
                    return false;
                }
                else {
                    std::vector<std::string> stdout;
                    RGSDL::Utils::exec(
                        "xdotool mousemove --sync " + std::to_string( mouseHistory[ 0 ].x ) + " " +
                            std::to_string( mouseHistory[ 0 ].y ) +
                            " getmouselocation --shell | grep WINDOW | cut "
                            "-d\"=\" -f2",
                        &stdout, true );
                    if ( stdout.size() ) windowid = stdout[ 0 ];

                    std::string finalcommand =
                        "(xdotool windowactivate --sync " + windowid + command + suffix;

                    Debug( "Send Command: " << finalcommand );
                    system( finalcommand.c_str() );
                }
            }
        }
        else {
            if ( x < game->windowPosition.x || x > game->windowPosition.x + game->windowSize.x ||
                 y < game->windowPosition.y || y > game->windowPosition.y + game->windowSize.y ) {

                mouseHistory.push_back( { x, y } );
                if ( mouseHistory.size() > mouseHistoryLimit )
                    mouseHistory.erase( mouseHistory.begin() );
            }
        }

        return keep_running;
    }
    void MainScene::onDraw( RGSDL::Engine* game ) { Components::ButtonManager::drawAll( game ); }

    std::string MainScene::getNextProfile() { return nextProfile; }

} // namespace Scenes