#pragma once

#include <RGSDL/Engine.h>
#include <RGSDL/Scene.h>
#include <RGSDL/Utils.h>

#include <string>

#include "../components/Button.h"

namespace Scenes {

    typedef RGSDL::Vec2<int> mousePos;

    class MainScene : public RGSDL::Scene {
      public:
        MainScene( const char* windowclass, RGSDL::Utils::IniType* ini, float ww, float wh );

        bool   onStart( RGSDL::Engine* game );
        bool   onUpdate( RGSDL::Engine* game, float deltaTime );
        void   onDraw( RGSDL::Engine* game );
        Scene* onEnd( RGSDL::Engine* game );

      private:
        RGSDL::Utils::IniType* ini;
        RGSDL::Texture         font;
        RGSDL::UI::Textlayer   button_label;
        unsigned long int      mouseHistoryLimit = 3;
        std::vector<mousePos>  mouseHistory;

        std::string windowclass, suffix, prefix;
        float       winWidthMultiplyer, winHeightMultiplyer;
        bool        keep_running    = true;
        bool        buttonactivated = false;

#if 0
    std::function<bool( int, int, int, bool )> onButtonHandler = [ this ]( int id, int x, int y, bool pressed ) {
      if ( pressed ) {
        if ( buttonactivated )
          return false;

        buttonactivated = true;
        buttonMousePos  = mouseHistory[ 0 ];
      }

      if ( !pressed && commands.find( id ) != commands.end() ) {
        system( ( "xdotool mousemove --sync " +
                  std::to_string( buttonMousePos.x ) +
                  " " + std::to_string( buttonMousePos.y ) )
                  .c_str() );

        std::string command = "(xdotool " + prefix + commands[ id ] + suffix + ") &";
        Debug( "Run: " << command );

        system( command.c_str() );

        mouseHistory.clear();
        mouseHistory.push_back( buttonMousePos );

        buttonactivated = false;
      }

      return true;
    };
#endif
    };

} // namespace Scenes