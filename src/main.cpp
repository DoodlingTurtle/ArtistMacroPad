#include <RGSDL/Engine.h>
#include <RGSDL/Scene.h>

using namespace RGSDL;

class MainScene : public Scene {
    public:
        bool onStart(Engine* game) override {
            game->backgroundColor = {64, 16, 64, 255};
            return true;
        }
        bool onUpdate(Engine* game, float deltaTime) override { return true; };
        void onDraw(Engine* game) override {}
};

int main(int argc, char** argv) {

    MainScene ms;
    Engine game;

    return game.start(argc, argv, 256, 1024, 224.0f, "MyArtDock", &ms, 0, SDL_WINDOW_ALWAYS_ON_TOP);
}
