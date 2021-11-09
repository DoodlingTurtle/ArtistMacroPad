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
        MainScene(const RGSDL::Utils::IniType* ini, float ww, float wh );

        bool   onStart( RGSDL::Engine* game );
        bool   onUpdate( RGSDL::Engine* game, float deltaTime );
        void   onDraw( RGSDL::Engine* game );
        Scene* onEnd( RGSDL::Engine* game );

      private:
        const RGSDL::Utils::IniType* ini;
        RGSDL::Texture         font;
        RGSDL::UI::Textlayer   button_label;
        unsigned long int      mouseHistoryLimit = 1;
        std::vector<mousePos>  mouseHistory;

        std::string windowid, suffix, prefix;
        float       winWidthMultiplyer, winHeightMultiplyer;
        bool        keep_running    = true;
        bool        buttonactivated = false;

    };

} // namespace Scenes