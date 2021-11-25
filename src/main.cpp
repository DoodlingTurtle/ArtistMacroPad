#include <RGSDL/Engine.h>
#include <RGSDL/Scene.h>
#include <RGSDL/Utils.h>

#include "./scenes/MainScene.h"
#include <SDL2/SDL_messagebox.h>

#include <cstdlib>

#define DIR_WINDOW_SIZE_CONFIG std::string(getenv("HOME")) + "/.config/ArtistMacroPad/windows/"

using namespace RGSDL;

int main(int argc, char **argv)
{
    std::string nextWindow = "./default.ini";
    if (argc == 2)
    {
        nextWindow = argv[1];
    }

    int ret = 0, winWidth, winHeight, viewWidth, viewHeight, winX, winY;
    uint32_t bgColor;
    bool alphaButtonScale = false;

    while (nextWindow.length() > 0)
    {
        Engine game;

        Debug(getenv("HOME"));

        // Reset window config values
        winWidth = 0;
        winHeight = 0;
        winX = 0;
        winY = SDL_WINDOWPOS_CENTERED_DISPLAY(0);

        std::vector<std::string> iniPath;
        Utils::stringSplit(nextWindow, "/", iniPath, false);

        // Read new Profile
        Utils::IniType *ini = new Utils::IniType();
        if (!Utils::readIni(nextWindow, *ini))
            return -1;

        Utils::IniGroup win = Utils::readIniGroup(*ini, "window");
        if (win.size() == 0)
        {
            Error("profile must contain a '[window]' group");
            delete ini;
            return -1;
        }

        viewWidth = Utils::readIniGroupInt(win, "width", 100);
        viewHeight = Utils::readIniGroupInt(win, "height", 100);
        alphaButtonScale = Utils::readIniGroupValue(win, "percentmessurements", "").length() > 0;
        bgColor = Utils::convertHex2Int(Utils::readIniGroupValue(win, "color", "#401040").substr(1));

        ini->erase(ini->find("window"));

        // Read Window User-Config
        Utils::IniType winConfig;
        if (Utils::readIni(
                DIR_WINDOW_SIZE_CONFIG + Utils::stringReplace(nextWindow, "/", "_"),
                winConfig))
        {

            win = Utils::readIniGroup(winConfig, "window");
            winX = Utils::readIniGroupInt(win, "x", 0);
            winY = Utils::readIniGroupInt(win, "y", SDL_WINDOWPOS_CENTERED_DISPLAY(0));
            winWidth = Utils::readIniGroupInt(win, "w", 0);
            winHeight = Utils::readIniGroupInt(win, "h", 0);

            game.scaleWindowToMatchDPI = false;
        }
        winConfig.clear();

        if (winWidth == 0 || winHeight == 0)
        {
            winWidth = 0;
            winHeight = 0;
            game.scaleWindowToMatchDPI = true;
        }

        Scenes::MainScene *ms = nullptr;
        try
        {
            ms = new Scenes::MainScene(ini,
                                       alphaButtonScale ? (float)viewWidth / 100.0f : 1.0f,
                                       alphaButtonScale ? (float)viewHeight / 100.0f : 1.0f);

            game.backgroundColor = {
                (unsigned char)((bgColor & 0xff0000) >> 16),
                (unsigned char)((bgColor & 0x00ff00) >> 8),
                (unsigned char)((bgColor & 0x0000ff)), 255};

            ret = game.start(
                argc, argv, viewWidth, viewHeight, 96.0f,
                ("AMP - " + iniPath[iniPath.size() - 1]).c_str(), ms, 0,
                SDL_WINDOW_ALWAYS_ON_TOP, winX, winY, winWidth, winHeight);

            // Save Window Position and Size for next time
            system(("mkdir -p " + DIR_WINDOW_SIZE_CONFIG).c_str());
            Utils::IniType cfg = {{"window",
                                   {
                                       {"x", std::to_string(game.trueWindowPosition.x)},
                                       {"y", std::to_string(game.trueWindowPosition.y)},
                                       {"w", std::to_string(game.windowSize.x)},
                                       {"h", std::to_string(game.windowSize.y)},
                                   }}};
            Utils::writeIni(
                DIR_WINDOW_SIZE_CONFIG + Utils::stringReplace(nextWindow, "/", "_"), cfg);

            nextWindow = "";
            nextWindow = ms->getNextProfile();
        }
        catch (char const *msg)
        {
            game.msgError("Error", msg);
            nextWindow = "";
        }

        delete ini;
        if (ms)
            delete ms;
    }

    return ret;
}
