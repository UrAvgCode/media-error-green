#include <memory>

#include <ofWindowSettings.h>
#include <ofAppRunner.h>

#include "core_app.h"

int main() {
    auto settings = ofGLWindowSettings();
    settings.setSize(1920, 1080);
    settings.setGLVersion(4, 6);
    settings.windowMode = OF_GAME_MODE;

    const auto window = ofCreateWindow(settings);
    ofRunApp(window, make_shared<CoreApp>());
    ofRunMainLoop();
}
