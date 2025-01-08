#include "tracking_app.h"
#include "ofMain.h"

#include <memory>

//========================================================================
int main() {
    ofGLWindowSettings settings;
    settings.setSize(1024, 768);
    settings.windowMode = OF_WINDOW;

    auto window = ofCreateWindow(settings);

    ofRunApp(window, make_shared<TrackingApp>());
    ofRunMainLoop();
}
