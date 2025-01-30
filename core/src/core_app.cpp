#include "core_app.h"

//--------------------------------------------------------------
void CoreApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    std::random_device random;
    generator = std::mt19937(random());
    distribution = std::uniform_int_distribution<int>(0, ofGetHeight());

    current_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    inactive_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    transition_shader.load("shaders/transition");

    intro_scene.setup();
    tracking_scene.setup();
    current_scene = &intro_scene;
    inactive_scene = &tracking_scene;

    kinect_device = tracking_scene.get_kinect_device();
}

//--------------------------------------------------------------
void CoreApp::update() {
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    if (current_scene == &intro_scene && !body_skeletons.empty()) {
        transition_start_time = std::chrono::steady_clock::now();

        current_scene = &tracking_scene;
        inactive_scene = &intro_scene;
    } else if (current_scene == &tracking_scene && body_skeletons.empty()) {
        transition_start_time = std::chrono::steady_clock::now();

        current_scene = &intro_scene;
        inactive_scene = &tracking_scene;
    }

    current_scene->update();
}

//--------------------------------------------------------------
void CoreApp::draw() {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - transition_start_time);
    auto progress = static_cast<float>(elapsed_time.count()) / static_cast<float>(transition_duration.count());

    if (progress < 1.0f) {
        current_app_fbo.begin();
        current_scene->draw();
        current_app_fbo.end();

        inactive_app_fbo.begin();
        inactive_scene->draw();
        inactive_app_fbo.end();

        transition_shader.begin();
        {
            transition_shader.setUniformTexture("transition_tex", inactive_app_fbo.getTexture(), 1);
            transition_shader.setUniform1f("progress", progress);
            transition_shader.setUniform1i("rand1", distribution(generator));
            transition_shader.setUniform1i("rand2", distribution(generator));
            current_app_fbo.draw(0, 0);
        }
        transition_shader.end();
    } else {
        current_scene->draw();
    }

    draw_fps_counter();
}

void CoreApp::draw_fps_counter() {
    std::ostringstream oss;
    oss << ofToString(ofGetFrameRate(), 2) + " FPS" << std::endl;
    ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

//--------------------------------------------------------------
void CoreApp::keyPressed(int key) {}

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
