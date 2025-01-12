#include <memory>

#include <ofMain.h>

#include "core_app.h"

int main() {
    ofGLWindowSettings settings;
    settings.setSize(1024, 768);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_FULLSCREEN;

    auto window = ofCreateWindow(settings);

    ofRunApp(window, make_shared<CoreApp>());
    ofRunMainLoop();
}
