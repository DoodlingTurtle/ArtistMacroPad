#include "./ButtonManager.h"
#include "./buttons/Default.h"
#include "./buttons/Slider.h"

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

    bool updateAll( RGSDL::Engine* game, std::string& sendcommand, bool& resetMousePositionOnClick )
    {
        
        bool mouseDown = false;
        RGSDL::Vec2<int> fingerPos(-1);
        auto finger = game->touchHeld.begin();
        if(finger != game->touchHeld.end()) {
            fingerPos = game->touchPositions[*finger];
            Debug("input_finger ("<<game->touchHeld.size()<<"): " << game->touchPositions[*finger]);
            Debug("input_mouse: "<< game->mousePosition);
            mouseDown = true;
        }

        if ( mouseDown ) {
            for ( auto btn : *button_list ) {
                if ( fingerPos.x > btn.second->rect.x &&
                     fingerPos.x < btn.second->rect.x + btn.second->rect.w &&
                     fingerPos.y > btn.second->rect.y &&
                     fingerPos.y < btn.second->rect.y + btn.second->rect.h ) {
                    if ( btn.second->onDown() ) {
                        if ( active_button ) sendcommand += active_button->onUp();

                        active_button             = btn.second;
                        button_list               = active_button->nextButtonList( &buttons_none );
                        resetMousePositionOnClick = active_button->resetMouseOnClick();
                        return true;
                    }
                }
            }

            if(active_button) active_button->onUpdate(game, fingerPos);

            resetMousePositionOnClick = false;
            return true;
        }
        else {
            if ( active_button ) {
                sendcommand += active_button->onUp();
                resetMousePositionOnClick = true;
                active_button             = nullptr;
                button_list               = &button_instances;
                return true;
            }

            return false;
        }
    }

    std::string parseCommand( std::string rawcommand )
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
        const RGSDL::Utils::IniType& ini )
    {

        Button* btn = nullptr;

        for ( auto iniGrpPair : ini ) {
            // if your group name starts with a '#' it is a hidden group, that is connected to other
            // buttons
            if ( iniGrpPair.first.at( 0 ) == '#' ) continue;

            auto iniGrp = iniGrpPair.second;

            std::string grp = RGSDL::Utils::readIniGroupValue( iniGrp, "group", "" );

            if ( grp != "" ) btn = new Components::Buttons::Slider( game, grp, ini, winWidth, winHeight, text_layer );
            else
                btn = new Components::Buttons::Default();

            SDL_Rect rect = {
                winDim( "x", -1, winWidth ),
                winDim( "y", -1, winHeight ),
                winDim( "w", 1, winWidth ),
                winDim( "h", 1, winHeight ),
            };

            SDL_Color col = {
                static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "r", 96 ) ),
                static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "g", 96 ) ),
                static_cast<unsigned char>( RGSDL::Utils::readIniGroupInt( iniGrp, "b", 96 ) ), 255
            };

            game->switchLayer( layer_BG );
            game->setDrawColor( col );
            game->fillRect( rect );

            game->switchLayer( layer_Labels );
            text_layer->setText(
                RGSDL::Utils::readIniGroupValue( iniGrp, "label", "btn" ).c_str() );
            text_layer->setPosition( rect.x, rect.y );
            text_layer->draw( game );

            std::string str = RGSDL::Utils::readIniGroupValue( iniGrp, "command", "btn" );
            btn->command    = parseCommand( str );
            btn->rect       = rect;

            col.r    = 255 - col.r;
            col.g    = 255 - col.g;
            col.b    = 255 - col.b;
            btn->col = col;

            button_instances.emplace( iniGrpPair.first, btn );
        }
    }

} // namespace Components::ButtonManager