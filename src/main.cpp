#include <RGSDL/Engine.h>
#include <RGSDL/UI.h>
#include <RGSDL/Scene.h>
#include <cstdlib>
#include <fstream>
#include <unordered_map>

using namespace RGSDL;


static void _strReplace(std::string& str, const std::string& from, const std::string& to) {
    size_t location = str.find(from);
    while(location != std::string::npos) {
        str.replace(location, from.length(), to);
        location = str.find(from);
    }
}

class MainScene : public Scene {
    public:

        MainScene(const char* windowclass, std::ifstream& file, float ww, float wh) {
            prefix = "xdotool windowactivate $(xdotool search --class " 
                + std::string(windowclass)
                + ") && sleep 0.15 " ;


            char buffer[1024];
            int btnID = 1;

            while(!file.eof()) {
                try {
                    int x = 0; file.getline(buffer, 1024); x = atoi(buffer) * ww;
                    int y = 0; file.getline(buffer, 1024); y = atoi(buffer) * wh;
                    int w = 0; file.getline(buffer, 1024); w = atoi(buffer) * ww;
                    int h = 0; file.getline(buffer, 1024); h = atoi(buffer) * wh;
                    Debug("Coords: " << x << " " << y << " " << w << " " << h);
                    if( w == 0  || h == 0) break;

                    file.getline(buffer, 1024); unsigned char r = atoi(buffer);
                    file.getline(buffer, 1024); unsigned char g = atoi(buffer);
                    file.getline(buffer, 1024); unsigned char b = atoi(buffer);
                    Debug("Color:  " << r << " - " << g << " - " << b);
                    
                    file.getline(buffer, 1024); std::string label = std::string(buffer);
                    _strReplace(label, std::string("\\")+std::string("n"), "\n");
                    Debug("Label:  " << label);

                    file.getline(buffer, 1024); int cmdcnt = atoi(buffer);

                    std::string command = "";
                    while(cmdcnt > 0) {
                        command += " && ";
                        char command_prefix[8];
                        char read_char;
                        int readptr = 0;
                        read_char = file.get();
                        while(read_char != ' ' && readptr < 7) {
                            command_prefix[readptr] = (char)read_char;
                            readptr++;
                            read_char = file.get();
                        }
                        command_prefix[readptr] = '\0';

                        if(
                                !strcmp(command_prefix, "keydown")
                                || !strcmp(command_prefix, "keyup")
                                || !strcmp(command_prefix, "key")
                          ) {
                            command += std::string("xdotool ") + std::string(command_prefix);
                        }
                        else {
                            command += std::string(command_prefix);
                        }
                        file.getline(buffer, 1024);
                        command += std::string(" ") + std::string(buffer);
                        cmdcnt--;
                    }

                    Debug("Command:" << command);

                    commands.emplace(btnID, command);
                    RGSDL::UI::registerButton(
                            btnID, 
                            x, y, w, h, 
                            {r, g, b, 255}, 
                            {255, 0, 32, 255}, 
                            label.c_str(), 
                            onButtonHandler
                            );

                    btnID++;
                }
                catch(std::ios_base::failure& f){
                    std::cout << "error" << std::endl;
                    Error("Failed to read button: " << f.what());
                }
            }
        }

        ~MainScene() { commands.clear(); }

        bool onStart(Engine* game) override {
            font = game->loadTexture("./assets/gnsh-bitmapfont-colour8.png");
            button_label = game->createTextlayer(
                    font, "Label",
                    5, 12,
                    12, 3
                    );
            UI::setLabelTextLayer(button_label);
            game->backgroundColor = { 64, 16, 64, 255 };
            return true;
        }
        bool onUpdate(Engine* game, float deltaTime) override { 
            RGSDL::UI::onUpdate(game);
            return true; 
        };

        void onDraw(Engine* game) override { RGSDL::UI::onDraw(game); }

        RGSDL::Scene* onEnd(Engine* game) override {
            game->destroyTextlayer(button_label);
            game->destroyTexture(font);

            return nullptr;
        }


    private:
        std::string prefix;
        std::unordered_map<int, std::string> commands;
        std::function<void(int, int, int, bool)> onButtonHandler=[this](int id, int x, int y, bool pressed) {
            Debug("Button Event " << x << "x" << y << " = " << pressed);
            if(pressed && commands.find(id) != commands.end())
                system((prefix + commands[id]).c_str());
        };

        RGSDL::Texture font;
        RGSDL::UI::Textlayer button_label;
};

int main(int argc, char** argv)
{
    if(argc != 2) {
        Error("Usage: " << argv[0] << " window_class");
        return 0;
    }
    std::ifstream file((std::string("assets/") + std::string(argv[1]) + std::string(".txt")).c_str());
    char buffer[1024];
    file.getline(buffer, 1024); int winWidth = atoi(buffer);
    file.getline(buffer, 1024); int winHeight = atoi(buffer);

    MainScene ms = MainScene(argv[1], file, (float)winWidth / 100.0f, (float)winHeight / 100.0f);

    file.close();

    Engine game;

    return game.start(argc, argv, winWidth, winHeight, 96.0f, "MyArtDock", &ms, 0, SDL_WINDOW_ALWAYS_ON_TOP);
}
