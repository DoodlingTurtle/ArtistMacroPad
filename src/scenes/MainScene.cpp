#include "./MainScene.h"
#include "../components/ButtonManager.h"
#include <RGSDL/Utils.h>

namespace Scenes {
    MainScene::MainScene( const char* windowclass, RGSDL::Utils::IniType* ini, float ww, float wh )
        : windowclass( windowclass ), ini( ini ),
          prefix(
              " search --onlyvisible " + std::string( windowclass ) + " windowactivate --sync" ),
          suffix( " mousemove_relative --sync 1 1) &" ), winWidthMultiplyer( ww ),
          winHeightMultiplyer( wh )
    {
    }

    bool MainScene::onStart( RGSDL::Engine* game )
    {
        font                  = game->loadTexture( "./assets/gnsh-bitmapfont-colour8.png" );
        button_label          = game->createTextlayer( font, "Label", 5, 12, 12, 3 );
        game->backgroundColor = { 64, 16, 64, 255 };

        for ( auto btn : *ini ) {
            if ( btn.first.at( 0 ) == '#' ) continue;
            Components::ButtonManager::registerButton(
                game, winWidthMultiplyer, winHeightMultiplyer, button_label, btn.first,
                btn.second );
        }

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

        std::string command = "";
        if ( Components::ButtonManager::updateAll( game, command ) ) {

            std::string finalcommand = "(xdotool mousemove --sync " +
                                       std::to_string( mouseHistory[ 0 ].x ) + " " +
                                       std::to_string( mouseHistory[ 0 ].y ) + suffix;

            system( finalcommand.c_str() );

            if ( command.length() > 0 ) {
                finalcommand =
                    "(xdotool mousemove --sync " + std::to_string( mouseHistory[ 0 ].x ) + " " +
                    std::to_string( mouseHistory[ 0 ].y ) + " " + prefix + command + suffix;

                Debug( "Send Command: " << finalcommand );
                system( finalcommand.c_str() );
            }
        }
        else {
            int x, y;
            SDL_GetGlobalMouseState( &x, &y );

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

} // namespace Scenes