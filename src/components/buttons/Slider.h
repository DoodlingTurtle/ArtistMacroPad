#include "../Button.h"
#include <RGSDL/Texture.h>

namespace Components::Buttons {

    class Slider : public Components::Button {
      public:
        struct SubButtonActive {
            SDL_Rect  position;
            SDL_Color color;
        };

        struct SubButton {
            SDL_Rect    position;
            SDL_Color   color;
            std::string label;
        };

        Slider(
            RGSDL::Engine* game, const std::string& group, const RGSDL::Utils::IniType& ini,
            float withMultiplyer, float heightMultiplyer, RGSDL::UI::Textlayer text_layer);
        ~Slider();

        bool        onDown() override;
        std::string onUp() override;
        ButtonList* nextButtonList( ButtonList* def ) override;
        bool        resetMouseOnClick() override;
        void        draw( RGSDL::Engine* game ) override;
        void        onUpdate( RGSDL::Engine* game ) override;

      private:
        int                  selected = 0;
        RGSDL::Engine*       game;
        RGSDL::UI::Textlayer text_layer;
        RGSDL::Texture       buttonFaces;

        std::vector<SubButton>       buttonRenders;
        std::vector<SubButtonActive> buttonActiveRenders;
        std::vector<std::string>     buttonCommands;
    };

} // namespace Components::Buttons