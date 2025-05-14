#include "core_app.h"

#include <utility>

CoreApp::CoreApp() :
    tracking_scene({&kinect_device}), current_scene(&intro_scene), inactive_scene(&tracking_scene),
    keyboard_triggered_scene(nullptr) {}

//--------------------------------------------------------------
void CoreApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

    if (kinect_device.open()) {
        auto device_settings = ofxAzureKinect::DeviceSettings();
        device_settings.syncImages = true;
        device_settings.depthMode = K4A_DEPTH_MODE_NFOV_UNBINNED;
        device_settings.updateIr = false;
        device_settings.updateColor = true;
        device_settings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
        device_settings.updateWorld = true;
        device_settings.updateVbo = false;
        kinect_device.startCameras(device_settings);

        auto body_tracker_settings = ofxAzureKinect::BodyTrackerSettings();
        body_tracker_settings.sensorOrientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
        body_tracker_settings.processingMode = K4ABT_TRACKER_PROCESSING_MODE_GPU;
        body_tracker_settings.updateBodiesImage = true;
        kinect_device.startBodyTracker(body_tracker_settings);
    }

    current_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    inactive_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    transition_shader.load("shaders/transition");

    ambient_sound.load("resources/audio/gruen_ambient.wav");
    ambient_sound.play();
    ambient_sound.setLoop(true);

    transition_to_intro_sound.load("resources/audio/transition_to_intro.wav");
    transition_to_intro_sound.setVolume(0.5f);

    transition_to_tracking_sound.load("resources/audio/transition_to_tracking.wav");
    transition_to_tracking_sound.setVolume(0.5f);
}

//--------------------------------------------------------------
void CoreApp::exit() {
    kinect_device.close();
    ofSoundStopAll();
}

//--------------------------------------------------------------
void CoreApp::update() {
    if (keyboard_triggered_scene) {
        keyboard_triggered_scene->update();
        return;
    }

    const auto &body_skeletons = kinect_device.getBodySkeletons();

    if (current_scene == &intro_scene && !body_skeletons.empty()) {
        transition_to_tracking_sound.play();
        transition_start_time = std::chrono::steady_clock::now();
        current_scene = &tracking_scene;
    } else if (current_scene == &tracking_scene && body_skeletons.empty()) {
        transition_to_intro_sound.play();
        transition_start_time = std::chrono::steady_clock::now();
        current_scene = &intro_scene;
    }

    current_scene->update();
}

//--------------------------------------------------------------
void CoreApp::draw() {
    if (keyboard_triggered_scene) {
        keyboard_triggered_scene->draw();
        draw_fps_counter();
        return;
    }

    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - transition_start_time);
    auto progress = static_cast<float>(elapsed_time.count()) / static_cast<float>(transition_duration.count());

    if (progress < 1.0f) {
        current_scene->render();
        inactive_scene->render();

        transition_shader.begin();
        {
            transition_shader.setUniformTexture("transition_tex", inactive_scene->get_frame_buffer().getTexture(), 1);
            transition_shader.setUniform1f("progress", progress);
            transition_shader.setUniform1f("random_offset_one", ofRandomHeight());
            transition_shader.setUniform1f("random_offset_two", ofRandomHeight());
            current_scene->get_frame_buffer().draw(0, 0);
        }
        transition_shader.end();
    } else {
        current_scene->draw();
    }

    draw_fps_counter();
}

void CoreApp::draw_fps_counter() {
    if (!show_debug_info) {
        return;
    }

    std::ostringstream oss;
    oss << ofToString(ofGetFrameRate(), 2) + " FPS" << std::endl;
    ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

//--------------------------------------------------------------
void CoreApp::keyPressed(int key) {
    switch (key) {
        case 'e':
            tracking_scene.toggle_skeletons();
            break;
        case 'q':
            tracking_scene.trigger_global_effect({ofGetWidth(), 0});
            break;
        case 'w':
            tracking_scene.trigger_global_effect({0, 0});
            break;
        case 'a':
            tracking_scene.trigger_global_effect({ofGetWidth(), ofGetHeight()});
            break;
        case 's':
            tracking_scene.trigger_global_effect({0, ofGetHeight()});
            break;
        case 'r':
            tracking_scene.reset_camera();
            break;
        case 'f':
            show_debug_info = !show_debug_info;
            break;
        case '1':
            keyboard_triggered_scene = &intro_scene;
            break;
        case '2':
            keyboard_triggered_scene = &tracking_scene;
            break;
        case '3':
            keyboard_triggered_scene == nullptr;
            break;
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
