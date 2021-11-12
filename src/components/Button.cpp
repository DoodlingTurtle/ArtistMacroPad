#include "./Button.h"
#include "./ButtonManager.h"

namespace Components {

    Button::Button()
        : col( { 0, 0, 0, 255 } ), rect( { -1, -1, 1, 1 } ), command( "" ), cleanupCommand( "" )
    {
    }

    ButtonList* Button::nextButtonList( ButtonList* def ) { return def; };

    void Button::draw( RGSDL::Engine* game )
    {
        game->switchLayer( layer_Active );
        game->setDrawColor( col );
        game->fillRect( rect );
    }

    bool Button::resetMouseOnClick() { return true; }

    void Button::onUpdate( RGSDL::Engine* game, RGSDL::Vec2<int> pointerCoords ) {}

} // namespace Components