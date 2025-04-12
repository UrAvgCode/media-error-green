#include "core_app.h"

#include <utility>

CoreApp::CoreApp() : tracking_scene({&kinect_device}), current_scene(&intro_scene), inactive_scene(&tracking_scene) {}

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
        body_tracker_settings.processingMode = K4ABT_TRACKER_PROCESSING_MODE_CPU;
        body_tracker_settings.updateBodiesImage = true;
        kinect_device.startBodyTracker(body_tracker_settings);
    }

    std::random_device random;
    generator = std::mt19937(random());
    distribution = std::uniform_int_distribution<int>(0, ofGetHeight());

    current_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    inactive_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    transition_shader.load("shaders/transition");

    ambient_sound.load("resources/audio/gruen_ambient.wav");
    ambient_sound.play();
    ambient_sound.setLoop(true);
}

//--------------------------------------------------------------
void CoreApp::exit() {
    kinect_device.close();
    ofSoundStopAll();
}

//--------------------------------------------------------------
void CoreApp::update() {
    const auto &body_skeletons = kinect_device.getBodySkeletons();

    if ((current_scene == &intro_scene && !body_skeletons.empty()) ||
        (current_scene == &tracking_scene && body_skeletons.empty())) {
        transition_start_time = std::chrono::steady_clock::now();
        std::swap(current_scene, inactive_scene);
    }

    current_scene->update();
}

//--------------------------------------------------------------
void CoreApp::draw() {
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
            transition_shader.setUniform1i("random_offset_one", distribution(generator));
            transition_shader.setUniform1i("random_offset_two", distribution(generator));
            current_scene->get_frame_buffer().draw(0, 0);
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
