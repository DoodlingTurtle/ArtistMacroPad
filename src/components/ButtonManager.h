#pragma once

#include <unordered_map>

#include <RGSDL/Engine.h>
#include <RGSDL/Utils.h>
#include <RGSDL/ui/Textlayer.h>

#define layer_SlideOver_Labels 255
#define layer_SlideOver_Rects  254
#define layer_Labels           253
#define layer_Active           252
#define layer_BG               251

namespace Components {
    class Button;
}

namespace Components::ButtonManager {

    void registerButton(
        RGSDL::Engine* game, float winWidth, float winHeight, RGSDL::UI::Textlayer button_label,
        const std::string& btnLabel, const RGSDL::Utils::IniGroup& iniGrp );

    void drawAll( RGSDL::Engine* game );

    /**
     * \return true if a button is currently pressed or has been released this cycle
     */
    bool updateAll(RGSDL::Engine* game, std::string& sendcommand);

    void clearAll();

} // namespace Components::ButtonManager
