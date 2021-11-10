#pragma once

#include <RGSDL/Engine.h>
#include <SDL2/SDL.h>
#include <string>

namespace Components {

    class Button {
      public:
        Button();
        virtual ~Button() = default;

        virtual bool onDown() = 0;
        virtual std::unordered_map<std::string, Button*>*
                            nextButtonList( std::unordered_map<std::string, Button*>* def );
        virtual std::string onUp() = 0;
        virtual void onUpdate(RGSDL::Engine* game, RGSDL::Vec2<int> pointerCoords );
        virtual bool resetMouseOnClick();

        std::string command;

        SDL_Rect  rect;
        SDL_Color col;

        virtual void draw( RGSDL::Engine* game );
    };

    typedef std::unordered_map<std::string, Button*> ButtonList;
} // namespace Components