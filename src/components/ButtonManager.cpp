#include "./ButtonManager.h"
#include "./buttons/Default.h"

#define winDim( key, def, mult ) \
    static_cast<int>( \
        static_cast<float>( RGSDL::Utils::readIniGroupInt( iniGrp, key, def ) ) * mult )

namespace Components::ButtonManager {

    static auto button_instances = ButtonList();
    static auto buttons_none     = ButtonList();

    static Components::Button*      active_button = nullptr;
    static std::vector<std::string> command_chain = std::vector<std::string>();
    static std::unordered_map<std::string, Components::Button*>* button_list = &button_instances;
    void                                                         clearAll()
    {
        for ( auto b : button_instances )
            delete b.second;

        button_instances.clear();
        command_chain.clear();
    }

    void drawAll( RGSDL::Engine* game )
    {
        game->switchLayer( layer_Active );
        game->clear();

        if ( active_button ) active_button->draw( game );
    }

    bool updateAll( RGSDL::Engine* game, std::string& sendcommand )
    {
        bool mouseDown =
            game->mousePressed( SDL_BUTTON_LEFT ) || game->mouseHeld( SDL_BUTTON_LEFT );

        if ( mouseDown ) {
            for ( auto btn : *button_list ) {
                if ( game->mousePosition.x > btn.second->rect.x &&
                     game->mousePosition.x < btn.second->rect.x + btn.second->rect.w &&
                     game->mousePosition.y > btn.second->rect.y &&
                     game->mousePosition.y < btn.second->rect.y + btn.second->rect.h ) {
                    if ( btn.second->onDown() ) {
                        active_button = btn.second;
                        button_list = active_button->nextButtonList( &buttons_none );
                    }
                }
            }

            return true;
        }
        else {
            if (active_button) {
                sendcommand += active_button->onUp();
                active_button = nullptr;
                button_list = &button_instances;
                return true;
            }

            return false;
        }

    }

    static std::string _parseCommand( std::string rawcommand )
    {
        if ( rawcommand.length() == 0 ) return rawcommand;

        std::vector<std::string> splitcommands;
        std::vector<std::string> action;
        std::string              finalcommand = "";

        int splits = RGSDL::Utils::stringSplit( rawcommand, "#", splitcommands );

        Debug( "Raw Command: " << rawcommand );
        Debug( "splitting command: " << splits );

        for ( auto commandline : splitcommands ) {
            action.clear();

            RGSDL::Utils::stringSplit( commandline, " ", action, true, 2 );

            if ( action.size() == 2 ) {
                Debug( "command: " << action[ 0 ] << " => " << action[ 1 ] );

                if ( action[ 0 ] == "keydown" || action[ 0 ] == "keyup" || action[ 0 ] == "key" ||
                     action[ 0 ] == "sleep" ) {
                    finalcommand += " " + action[ 0 ] + " " + action[ 1 ];
                }
                else if ( action[ 0 ] == "term" ) {
                    finalcommand += " && " + action[ 1 ] + " && xdotool ";
                }
                else if ( action[ 0 ] == "point" ) {
                    finalcommand += " mousemove_relative --sync " + action[ 1 ];
                }
            }
        }

        return finalcommand;
    }

    void registerButton(
        RGSDL::Engine* game, float winWidth, float winHeight, RGSDL::UI::Textlayer text_layer,
        const std::string& btnLabel, const RGSDL::Utils::IniGroup& iniGrp )
    {

        Button* btn = nullptr;

        switch ( RGSDL::Utils::readIniGroupInt( iniGrp, "type" ) ) {
            default: btn = new Components::Buttons::Default();
        }

        SDL_Rect rect = {
            winDim( "x", -1, winWidth ),
            winDim( "y", -1, winHeight ),
            winDim( "w", 1, winWidth ),
            winDim( "h", 1, winHeight ),
        };

        Debug(
            static_cast<float>( RGSDL::Utils::readIniGroupInt( iniGrp, "x" ) ) / 100.0f *
            winWidth );

        SDL_Color col = {
            static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "r" ) ),
            static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "g" ) ),
            static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "b" ) ), 255
        };

        game->switchLayer( layer_BG );
        game->setDrawColor( col );
        game->fillRect( rect );

        game->switchLayer( layer_Labels );
        text_layer->setText( RGSDL::Utils::readIniGroupValue( iniGrp, "label", "btn" ).c_str() );
        text_layer->setPosition( rect.x, rect.y );
        text_layer->draw( game );

        std::string str = RGSDL::Utils::readIniGroupValue( iniGrp, "command", "btn" );
        btn->command    = _parseCommand( str );
        btn->rect       = rect;

        col.r    = 255 - col.r;
        col.g    = 255 - col.g;
        col.b    = 255 - col.b;
        btn->col = col;

        button_instances.emplace( btnLabel, btn );
    }

} // namespace Components::ButtonManager