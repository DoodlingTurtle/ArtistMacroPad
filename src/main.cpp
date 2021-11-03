#include <RGSDL/Engine.h>
#include <RGSDL/Scene.h>
#include <RGSDL/UI.h>

#include <cstdlib>
#include <limits>

using namespace RGSDL;

namespace RGSDL::Utils {

  std::string stringTrim( const std::string& src )
  {
    size_t start = 0;
    size_t end   = src.length() - 1;

    size_t newStart = start;
    size_t newEnd   = end;

    char ch = src.at( start );
    while ( ch == ' ' || ch == '\t' ) {
      try {
        newStart++;
        ch = src.at( newStart );
      }
      catch ( std::out_of_range* e ) {
        break;
      }
    }
    start = newStart;

    ch = src.at( end );
    while ( newEnd >= start && ( ch == ' ' || ch == '\t' ) ) {
      try {
        newEnd--;
        ch = src.at( newEnd );
      }
      catch ( std::out_of_range* e ) {
        break;
      }
    }

    return src.substr( start, ( end - start + 1 ) );
  }

  int stringSplit(
    const std::string&        src,
    const std::string&        delimiter,
    std::vector<std::string>& fillin,
    bool                      trimResult = true,
    int                       maxsplits  = std::numeric_limits<int>::max()

  )
  {

    int splits = 1;

    size_t pos   = src.find( delimiter );
    size_t start = 0;

    while ( splits < maxsplits && pos != std::string::npos ) {
      if ( trimResult ) fillin.push_back( stringTrim( src.substr( start, pos - start ) ) );
      else
        fillin.push_back( src.substr( start, pos - start ) );
      start = pos + delimiter.length();
      pos   = src.find( delimiter, start );

      splits++;
    }

    if ( trimResult ) fillin.push_back( stringTrim( src.substr( start, pos - start ) ) );
    else
      fillin.push_back( src.substr( start, pos - start ) );

    return splits;
  }
} // namespace RGSDL::Utils

struct mousePos {
  int x, y;
};

class MainScene : public Scene {
public:
  MainScene( const char* windowclass, Utils::IniType& ini, float ww, float wh )
  {

    this->windowclass = windowclass;

    int                      btnID = 1;
    std::vector<std::string> splitcommands;
    std::vector<std::string> action;
    std::string              finalcommand = "";
    bool                     needsPrefix;

    //prefix = " windowactivate $(xdotool getmouselocation --shell | grep WINDOW | sed s/=/\\\\n/g | tail -n 1 | xargs) getactivewindow ";
    prefix = " search --onlyvisible " + this->windowclass + " windowactivate --sync";
    suffix = " mousemove_relative --sync 1 1";

    for ( auto btn : ini ) {
      Debug( "Button: " << btn.first );
      finalcommand = " ";
      needsPrefix  = false;

      std::string rawcommand = Utils::readIniGroupValue( btn.second, "command", "" );
      int         l          = rawcommand.length();
      if ( l == 0 ) continue;

      Debug( "Raw Command: " << rawcommand );
      splitcommands.clear();
      Debug( "splitting command: " << Utils::stringSplit( rawcommand, "#", splitcommands ) );
      for ( auto commandline : splitcommands ) {
        action.clear();
        Utils::stringSplit( commandline, " ", action, true, 2 );
        if ( action.size() == 2 ) {
          Debug( "command: " << action[ 0 ] << " => " << action[ 1 ] );

          if (
            action[ 0 ] == "keydown" || action[ 0 ] == "keyup" || action[ 0 ] == "key" || action[ 0 ] == "sleep" ) {
            finalcommand += " " + action[ 0 ] + " " + action[ 1 ];
          }
          else if ( action[ 0 ] == "term" ) {
            finalcommand += " exec bash -c\"" + action[ 1 ] + "\" --sync";
          }
          else if ( action[ 0 ] == "point" ) {
            finalcommand += " mousemove_relative " + action[ 1 ];
          }
        }
      }

      if ( finalcommand.length() == 0 ) continue;

      if ( needsPrefix ) finalcommand = prefix + finalcommand;

      Debug( "Final Command: " << finalcommand );
      commands.emplace( btnID, finalcommand );

      RGSDL::UI::registerButton(
        btnID,
        (int)( (float)atoi( Utils::readIniGroupValue( btn.second, "x", "0" ).c_str() ) * ww ),
        (int)( (float)atoi( Utils::readIniGroupValue( btn.second, "y", "0" ).c_str() ) * wh ),
        (int)( (float)atoi( Utils::readIniGroupValue( btn.second, "w", "1" ).c_str() ) * ww ),
        (int)( (float)atoi( Utils::readIniGroupValue( btn.second, "h", "1" ).c_str() ) * wh ),
        { (unsigned char)atoi( Utils::readIniGroupValue( btn.second, "r", "0" ).c_str() ),
          (unsigned char)atoi( Utils::readIniGroupValue( btn.second, "g", "0" ).c_str() ),
          (unsigned char)atoi( Utils::readIniGroupValue( btn.second, "b", "0" ).c_str() ),
          255 },
        { 255, 0, 32, 255 },
        Utils::readIniGroupValue( btn.second, "label", "" ).c_str(),
        onButtonHandler );

      btnID++;
    }
  }

  ~MainScene()
  {
    commands.clear();
  }

  bool onStart( Engine* game ) override
  {
    font         = game->loadTexture( "./assets/gnsh-bitmapfont-colour8.png" );
    button_label = game->createTextlayer( font, "Label", 5, 12, 12, 3 );
    UI::setLabelTextLayer( button_label );
    game->backgroundColor = { 64, 16, 64, 255 };
    return true;
  }
  bool onUpdate( Engine* game, float deltaTime ) override
  {
    RGSDL::UI::onUpdate( game );

    if ( !buttonactivated ) {
      int x, y;
      SDL_GetGlobalMouseState( &x, &y );

      mouseHistory.push_back( { x, y } );
      if ( mouseHistory.size() > mouseHistoryLimit )
        mouseHistory.erase( mouseHistory.begin() );
    }

    //Debug("Mouse position: " << buttonactivated << mouseHistory[ 0 ].x  << "x" << mouseHistory[0].y);

    return keep_running;
  };

  void onDraw( Engine* game ) override
  {
    RGSDL::UI::onDraw( game );
  }

  RGSDL::Scene* onEnd( Engine* game ) override
  {
    game->destroyTextlayer( button_label );
    game->destroyTexture( font );

    return nullptr;
  }

private:
  std::string                                suffix;
  std::string                                prefix;
  std::string                                windowclass;
  mousePos                                   buttonMousePos;
  bool                                       buttonactivated = false;
  std::unordered_map<int, std::string>       commands;
  bool                                       keep_running         = true;
  std::function<void( int, int, int, bool )> onCloseButtonHandler = [ this ]( int i, int x, int y, bool p ) {
    if ( p )
      keep_running = false;
  };
  std::function<bool( int, int, int, bool )> onButtonHandler = [ this ]( int id, int x, int y, bool pressed ) {
    if(pressed) {
      if(buttonactivated) return false;

      buttonactivated = true;
      buttonMousePos = mouseHistory[0];
    }

    if ( !pressed && commands.find( id ) != commands.end() ) {
      system(("xdotool mousemove --sync " + 
              std::to_string( buttonMousePos.x ) + 
        " " + std::to_string( buttonMousePos.y )).c_str() 
      );

      std::string command = "(xdotool " + prefix + commands[ id ] + suffix + ") &";
      Debug( "Run: " << command );

      system( command.c_str() );

      mouseHistory.clear();
      mouseHistory.push_back(buttonMousePos);

      buttonactivated = false;
    }

    return true;
  };

  RGSDL::Texture       font;
  RGSDL::UI::Textlayer button_label;

  unsigned long int     mouseHistoryLimit = 3;
  std::vector<mousePos> mouseHistory;
};

int main( int argc, char** argv )
{
  if ( argc != 2 ) {
    Error( "Usage: " << argv[ 0 ] << " window_class" );
    return 0;
  }

  Utils::IniType ini;
    if(!Utils::readIni(
      std::string( "assets/" ) + std::string( argv[ 1 ] ) + std::string( ".ini" ),
      ini )) return -1;

  Utils::IniGroup win = Utils::readIniGroup( ini, "window" );
  ini.erase( ini.find( "window" ) );

  int winWidth  = atoi( Utils::readIniGroupValue( win, "width", "100" ).c_str() );
  int winHeight = atoi( Utils::readIniGroupValue( win, "height", "100" ).c_str() );

  MainScene ms = MainScene( argv[ 1 ], ini, (float)winWidth / 100.0f, (float)winHeight / 100.0f );

  Engine game;

  return game.start( argc, argv, winWidth, winHeight, 96.0f, "MyMacroPad", &ms, 0, SDL_WINDOW_ALWAYS_ON_TOP, 0 );
}
