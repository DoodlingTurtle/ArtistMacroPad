#pragma once

#include <unordered_map>

#include <RGSDL/Engine.h>
#include <RGSDL/Utils.h>
#include <RGSDL/ui/Textlayer.h>

#define layer_SlideOver_Labels 255
#define layer_SlideOver_Active 254
#define layer_SlideOver_BG     253
#define layer_Labels           252
#define layer_Active           251
#define layer_BG               250

#define winDim( key, def, mult ) \
    static_cast<int>( \
        static_cast<float>( RGSDL::Utils::readIniGroupInt( iniGrp, key, def ) ) * mult )

namespace Components {
    class Button;
}

namespace Components::ButtonManager {

    void registerButton(
        RGSDL::Engine* game, float winWidth, float winHeight, RGSDL::UI::Textlayer button_label,
        const RGSDL::Utils::IniType& ini );

    void drawAll( RGSDL::Engine* game );

    /**
     * \return true if a button is currently pressed or has been released this cycle
     */
    bool
        updateAll( RGSDL::Engine* game, std::string& sendcommand, bool& resetMousePositionOnClick );

    std::string parseCommand( std::string rawcommand );

    void clearAll();

} // namespace Components::ButtonManager
