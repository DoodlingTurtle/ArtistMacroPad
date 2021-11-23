#include "./Slider.h"
#include "../ButtonManager.h"

namespace Components::Buttons
{

    Slider::Slider(
        RGSDL::Engine *game, const std::string &group, const RGSDL::Utils::IniType &ini, float ww,
        float wh, RGSDL::UI::Textlayer text_layer)
        : Button(), selected(-1), game(game), text_layer(text_layer), buttonActiveRenders(),
          buttonRenders(), buttonCommands()
    {

        std::vector<std::string> subbuttongroups;
        RGSDL::Utils::stringSplit(group, "#", subbuttongroups, true);
        SDL_Rect btnRect;
        SDL_Color btnCol;

        for (auto grpName : subbuttongroups)
        {
            auto iniGrp = RGSDL::Utils::readIniGroup(ini, "#" + grpName);
            if (iniGrp.size() > 0)
            {
                std::string rawcommand = RGSDL::Utils::readIniGroupValue(iniGrp, "command", "");
                std::string parsedCommand = "";
                if (rawcommand.length() > 0)
                    parsedCommand = Components::ButtonManager::parseCommand(rawcommand);

                btnRect.x = winDim("x", -1, ww);
                btnRect.y = winDim("y", -1, wh);
                btnRect.w = winDim("w", 1, ww);
                btnRect.h = winDim("h", 1, wh);

                btnCol.r =
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "r", 96));
                btnCol.g =
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "g", 96));
                btnCol.b =
                    static_cast<unsigned char>(RGSDL::Utils::readIniGroupInt(iniGrp, "b", 96));

                btnCol.a = 255;

                buttonRenders.push_back(
                    {btnRect, btnCol,
                     RGSDL::Utils::stringReplace(
                         RGSDL::Utils::readIniGroupValue(iniGrp, "label", "btn"), "#", "\n"),
                     RGSDL::Utils::readIniGroupValue(iniGrp, "icon", "")});

                buttonActiveRenders.push_back(
                    {btnRect, (SDL_Color){(Uint8)255 - btnCol.r, (Uint8)255 - btnCol.g,
                                          (Uint8)255 - btnCol.b, 255}});

                buttonCommands.push_back(parsedCommand);
            }
        }
    }

    Slider::~Slider()
    {
        buttonRenders.clear();
        buttonActiveRenders.clear();
        buttonCommands.clear();
    }

    bool Slider::onDown()
    {
        selected = 0;

        game->switchLayer(layer_SlideOver_BG);
        game->clear();
        for (auto btn : buttonRenders)
        {
            game->setDrawColor(btn.color);
            game->fillRect({
                btn.position.x + this->rect.x,
                btn.position.y + this->rect.y,
                btn.position.w,
                btn.position.h,
            });
        }

        game->switchLayer(layer_SlideOver_Labels);
        game->clear();
        for (auto btn : buttonRenders)
        {
            if (btn.iconPath.length() == 0)
            {
                text_layer->setText(btn.label.c_str());
                text_layer->setPosition(
                    btn.position.x + this->rect.x + (btn.position.w - text_layer->getTextPXWidth()) / 2,
                    btn.position.y + this->rect.y + (btn.position.h - text_layer->getTextPXHeight()) / 2);
                text_layer->draw(game);
            }
            else
            {

                RGSDL::Texture tx = game->loadTexture(btn.iconPath.c_str());
                tx.draw({{btn.position.x + this->rect.x + btn.position.w / 2.0f,
                          btn.position.y + this->rect.y + btn.position.h / 2.0f},
                         {(int)((float)btn.position.w / 2.0f), (int)((float)btn.position.h / 2.0f)},
                         {1.0f, 1.0f},
                         0.0f});
                game->destroyTexture(tx);
            }
        }

        game->switchLayer(layer_SlideOver_Active);
        return true;
    }

    std::string Slider::onUp()
    {
        game->switchLayer(layer_SlideOver_Active);
        game->clear();
        game->switchLayer(layer_SlideOver_BG);
        game->clear();
        game->switchLayer(layer_SlideOver_Labels);
        game->clear();

        return selected > 0 ? buttonCommands.at(selected - 1) : command;
    }

    ButtonList *Slider::nextButtonList(ButtonList *def) { return def; };
    bool Slider::resetMouseOnClick() { return false; }

    void Slider::onUpdate(RGSDL::Engine *game, RGSDL::Vec2<int> pointerCoords)
    {
        SDL_Rect rect;
        selected = 0;

        for (int a = 0; a < buttonActiveRenders.size(); a++)
        {
            rect = buttonActiveRenders.at(a).position;
            rect.x += this->rect.x;
            rect.y += this->rect.y;

            if (pointerCoords.x > rect.x && pointerCoords.x < rect.x + rect.w &&
                pointerCoords.y > rect.y && pointerCoords.y < rect.y + rect.h)
            {
                selected = a + 1;
                return;
            }
        }
    }

    void Slider::draw(RGSDL::Engine *game)
    {
        game->switchLayer(layer_SlideOver_Active);
        game->clear();

        if (selected > 0)
        {
            SDL_Rect pos = buttonActiveRenders.at(selected - 1).position;
            pos.x += this->rect.x;
            pos.y += this->rect.y;
            game->setDrawColor(buttonActiveRenders.at(selected - 1).color);
            game->fillRect(pos);
        }
    }

} // namespace Components::Buttons
