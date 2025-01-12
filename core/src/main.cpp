#include <memory>

#include <ofMain.h>

#include "core_app.h"

int main() {
    ofGLWindowSettings settings;
    settings.setSize(1920, 1080);
    settings.setGLVersion(4, 6);
    settings.windowMode = OF_WINDOW;

    auto window = ofCreateWindow(settings);

    ofRunApp(window, make_shared<CoreApp>());
    ofRunMainLoop();
}
