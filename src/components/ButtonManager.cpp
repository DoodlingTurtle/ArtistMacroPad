#include "./ButtonManager.h"
#include "./buttons/Default.h"
#include "./buttons/Slider.h"

namespace Components::ButtonManager
{

    static auto button_instances = ButtonList();
    static auto buttons_none = ButtonList();

    static Components::Button *active_button = nullptr;
    static Components::Button *last_button = nullptr;
    static std::vector<std::string> command_chain = std::vector<std::string>();
    static std::unordered_map<std::string, Components::Button *> *button_list = &button_instances;
    static std::string nextCleanup = "";
    static SDL_FingerID currentTouch = 0;

    void clearAll()
    {
        for (auto b : button_instances)
            delete b.second;

        last_button = nullptr;
        active_button = nullptr;

        button_instances.clear();
        command_chain.clear();
    }

    void drawAll(RGSDL::Engine *game)
    {
        game->switchLayer(layer_Active);
        game->clear();

        if (active_button)
            active_button->draw(game);
    }

    bool updateAll(RGSDL::Engine *game, std::string &sendcommand, bool &resetMousePositionOnClick)
    {

        bool mouseDown = false;
        RGSDL::Vec2<int> fingerPos(-1);

        if (currentTouch == 0)
        {
            auto finger = game->touchPressed.begin();
            if (finger != game->touchPressed.end())
            {
                currentTouch = *finger;
                mouseDown = true;
                fingerPos = game->touchPositions[currentTouch];
            }
        }
        else
        {
            auto finger = game->touchPositions.find(currentTouch);
            if (finger == game->touchPositions.end())
            {
                mouseDown = false;
                currentTouch = 0;
            }
            else
            {
                mouseDown = true;
                fingerPos = game->touchPositions[currentTouch];
            }
        }

        if (mouseDown)
        {
            for (auto btn : *button_list)
            {
                if (fingerPos.x > btn.second->rect.x &&
                    fingerPos.x < btn.second->rect.x + btn.second->rect.w &&
                    fingerPos.y > btn.second->rect.y &&
                    fingerPos.y < btn.second->rect.y + btn.second->rect.h)
                {
                    if (btn.second->onDown())
                    {
                        if (active_button)
                            sendcommand += active_button->onUp();

                        active_button = btn.second;
                        button_list = active_button->nextButtonList(&buttons_none);
                        resetMousePositionOnClick = active_button->resetMouseOnClick();
                        return true;
                    }
                }
            }

            if (active_button)
                active_button->onUpdate(game, fingerPos);

            resetMousePositionOnClick = false;
            return true;
        }
        else
        {
            if (active_button)
            {
                sendcommand += active_button->onUp() + nextCleanup;
                if (active_button == last_button)
                    nextCleanup = "";
                else
                    nextCleanup = active_button->cleanupCommand;

                resetMousePositionOnClick = true;

                game->switchLayer(layer_Toggle);
                game->clear();

                if (nextCleanup.length() > 0)
                {
                    game->setDrawColor(active_button->col);
                    game->fillRect(active_button->rect);
                }

                last_button = active_button;
                active_button = nullptr;
                button_list = &button_instances;
                return true;
            }

            return false;
        }
    } // namespace Components::ButtonManager

    std::string parseCommand(std::string rawcommand, std::string *cleanUpCommand)
    {
        if (rawcommand.length() == 0)
            return rawcommand;

        std::vector<std::string> splitcommands;
        std::vector<std::string> action;
        std::string finalcommand = "";

        RGSDL::Utils::stringSplit(rawcommand, "#", splitcommands);
        Debug("Raw Command: " << rawcommand);

        for (auto commandline : splitcommands)
        {
            action.clear();

            RGSDL::Utils::stringSplit(commandline, " ", action, true, 2);

            if (action.size() == 2)
            {
                Debug("command: " << action[0] << " => " << action[1]);
                if (action[0] == "keydown" || action[0] == "keyup" || action[0] == "key" ||
                    action[0] == "sleep")
                {
                    finalcommand += " " + action[0] + " " + action[1];
                }
                else if (action[0] == "term")
                {
                    finalcommand += " && " + action[1] + " && xdotool ";
                }
                else if (action[0] == "toggle")
                {
                    if (cleanUpCommand)
                        cleanUpCommand->append(" keyup " + action[1]);
                    else
                        throw "toogle commands are not allowed for grouped buttons";
                    if (action[1] == "shift" || action[1] == "ctrl" || action[1] == "alt")
                        finalcommand += " keydown " + action[1];
                    else
                        throw "only alt, ctrl and shift can be toggled";
                }
                else if (action[0] == "term")
                {
                    finalcommand += " && " + action[1] + " && xdotool ";
                }
                else if (action[0] == "profile")
                {
                    finalcommand = "#" + action[1];
                    break;
                }
                else if (action[0] == "point")
                {
                    finalcommand += " mousemove_relative --sync " + action[1];
                }
            }
        }

        return finalcommand;
    }

    void registerButton(
        RGSDL::Engine *game, float winWidth, float winHeight, RGSDL::UI::Textlayer text_layer,
        const RGSDL::Utils::IniType &ini)
    {

        Button *btn = nullptr;

        for (auto iniGrpPair : ini)
        {
            // if your group name starts with a '#' it is a hidden group, that is connected to other
            // buttons
            if (iniGrpPair.first.at(0) == '#')
                continue;

            iniGrpPair.second.find("useprealphabuttonscale");

            auto iniGrp = iniGrpPair.second;

            std::string grp = RGSDL::Utils::readIniGroupValue(iniGrp, "group", "");

            if (grp != "")
                btn = new Components::Buttons::Slider(
                    game, grp, ini, winWidth, winHeight, text_layer);
            else
                btn = new Components::Buttons::Default();

            SDL_Rect rect = {
                winDim("x", -1, winWidth),
                winDim("y", -1, winHeight),
                winDim("w", 1, winWidth),
                winDim("h", 1, winHeight)};

            SDL_Color col;
            std::string colorAttr = RGSDL::Utils::readIniGroupValue(iniGrp, "color", "");

            if (colorAttr.length() > 0)
            {
                if (colorAttr.at(0) == '#')
                {
                    uint32_t colorValue = RGSDL::Utils::convertHex2Int(colorAttr.substr(1));
                    col = {
                        (unsigned char)((colorValue & 0xff0000) >> 16),
                        (unsigned char)((colorValue & 0x00ff00) >> 8),
                        (unsigned char)((colorValue & 0x0000ff)), 255};
                }
                else
                    col = {96, 96, 96, 255};
            }
            else
            {
                col = {
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "r", 96)),
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "g", 96)),
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "b", 96)), 255};
            }

            game->switchLayer(layer_BG);
            game->setDrawColor(col);
            game->fillRect(rect);

            game->switchLayer(layer_Labels);

            RGSDL::Texture tx;
            std::string icon = RGSDL::Utils::readIniGroupValue(iniGrp, "icon", "");
            if (icon.length() > 0)
            {
                tx = game->loadTexture(icon.c_str());
                tx.draw({{(float)rect.x, (float)rect.y},
                         {(int)-((float)(rect.w - tx.crop.w) / 2.0f),
                          (int)-((float)(rect.h - tx.crop.h) / 2.0f)},
                         {1.0f, 1.0f},
                         0.0f});
                game->destroyTexture(tx);
            }
            else
            {
                text_layer->setText(
                    RGSDL::Utils::stringReplace(
                        RGSDL::Utils::readIniGroupValue(
                            iniGrp, "label", "btn"),
                        "#", "\n")
                        .c_str());

                text_layer->setPosition(rect.x + (rect.w - text_layer->getTextPXWidth()) / 2,
                                        rect.y + (rect.h - text_layer->getTextPXHeight()) / 2);
                text_layer->draw(game);
            }

            std::string str = RGSDL::Utils::readIniGroupValue(iniGrp, "command", "btn");
            std::string cleanupCommand = "";
            btn->command = parseCommand(str, &cleanupCommand);
            btn->cleanupCommand = cleanupCommand;

            btn->rect = rect;

            col.r = 255 - col.r;
            col.g = 255 - col.g;
            col.b = 255 - col.b;
            btn->col = col;

            button_instances.emplace(iniGrpPair.first, btn);
        }
    }

} // namespace Components::ButtonManager