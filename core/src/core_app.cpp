#include "core_app.h"

#include <cmath>

//--------------------------------------------------------------
void CoreApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    transition_frame = 0;

    current_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    inactive_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    transition_shader.load("shaders/transition");

    intro_app.setup();
    tracking_app.setup();
    current_app = &intro_app;
    inactive_app = &tracking_app;
}

//--------------------------------------------------------------
void CoreApp::update() { current_app->update(); }

//--------------------------------------------------------------
void CoreApp::draw() {
    current_app_fbo.begin();
    current_app->draw();
    current_app_fbo.end();

    if (transition_frame > max_transition_frames) {
        current_app_fbo.draw(0, 0);
    } else {
        ++transition_frame;
        auto progress = std::pow(static_cast<float>(transition_frame) / static_cast<float>(max_transition_frames), 10);

        inactive_app_fbo.begin();
        inactive_app->draw();
        inactive_app_fbo.end();

        transition_shader.begin();
        {
            transition_shader.setUniformTexture("transition_tex", inactive_app_fbo.getTexture(), 1);
            transition_shader.setUniform1f("progress", progress);
            transition_shader.setUniform1f("rand1",
                                           static_cast<float>(rand() % static_cast<int>(current_app_fbo.getHeight())));
            transition_shader.setUniform1f("rand2",
                                           static_cast<float>(rand() % static_cast<int>(current_app_fbo.getHeight())));
            current_app_fbo.draw(0, 0);
        }
        transition_shader.end();
    }

    draw_fps_counter();
}

void CoreApp::draw_fps_counter() {
    std::ostringstream oss;
    oss << ofToString(ofGetFrameRate(), 2) + " FPS" << std::endl;
    ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

//--------------------------------------------------------------
void CoreApp::keyPressed(int key) {
    if (key == 's') {
        transition_frame = 0;

        auto temp = current_app;
        current_app = inactive_app;
        inactive_app = temp;
    }
}

//--------------------------------------------------------------
void CoreApp::keyReleased(int key) {}

//--------------------------------------------------------------
void CoreApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void CoreApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void CoreApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void CoreApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void CoreApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void CoreApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void CoreApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void CoreApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void CoreApp::dragEvent(ofDragInfo dragInfo) {}
