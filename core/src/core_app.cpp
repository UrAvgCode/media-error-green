#include "core_app.h"

#include <utility>

CoreApp::CoreApp() :
    _tracking_scene({&_kinect_device}), _current_scene(&_intro_scene), _inactive_scene(&_tracking_scene),
    _keyboard_triggered_scene(nullptr), _show_debug_info(false) {}

void CoreApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

    if (_kinect_device.open()) {
        auto device_settings = ofxAzureKinect::DeviceSettings();
        device_settings.syncImages = true;
        device_settings.depthMode = K4A_DEPTH_MODE_NFOV_UNBINNED;
        device_settings.updateIr = false;
        device_settings.updateColor = true;
        device_settings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
        device_settings.updateWorld = true;
        device_settings.updateVbo = false;
        _kinect_device.startCameras(device_settings);

        auto body_tracker_settings = ofxAzureKinect::BodyTrackerSettings();
        body_tracker_settings.sensorOrientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
        body_tracker_settings.processingMode = K4ABT_TRACKER_PROCESSING_MODE_GPU;
        body_tracker_settings.updateBodiesImage = true;
        _kinect_device.startBodyTracker(body_tracker_settings);
    }

    _current_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    _inactive_app_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    _transition_shader.load("shaders/transition");

    _ambient_sound.load("resources/audio/gruen_ambient.wav");
    _ambient_sound.play();
    _ambient_sound.setLoop(true);

    _transition_to_intro_sound.load("resources/audio/transition_to_intro.wav");
    _transition_to_intro_sound.setVolume(0.25f);

    _transition_to_tracking_sound.load("resources/audio/transition_to_tracking.wav");
    _transition_to_tracking_sound.setVolume(0.25f);
}

void CoreApp::exit() {
    _kinect_device.close();
    ofSoundStopAll();
}

void CoreApp::update() {
    if (_keyboard_triggered_scene) {
        _keyboard_triggered_scene->update();
        return;
    }

    const auto &body_skeletons = _kinect_device.getBodySkeletons();

    if (_current_scene == &_intro_scene && !body_skeletons.empty()) {
        _transition_to_tracking_sound.play();
        _transition_start_time = std::chrono::steady_clock::now();
        _current_scene = &_tracking_scene;
    } else if (_current_scene == &_tracking_scene && body_skeletons.empty()) {
        _intro_scene.reset_particles();

        _transition_to_intro_sound.play();
        _transition_start_time = std::chrono::steady_clock::now();
        _current_scene = &_intro_scene;
    }

    _current_scene->update();
}

void CoreApp::draw() {
    const auto time = std::chrono::steady_clock::now();
    const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(time - _transition_start_time);
    const auto progress = static_cast<float>(elapsed_time.count()) / static_cast<float>(_transition_duration.count());

    if (_keyboard_triggered_scene) {
        _keyboard_triggered_scene->draw();
    } else if (progress < 1.0f) {
        _current_scene->render();
        _inactive_scene->render();

        _transition_shader.begin();
        _transition_shader.setUniformTexture("transition_tex", _inactive_scene->get_frame_buffer().getTexture(), 1);
        _transition_shader.setUniform1f("progress", progress);
        _transition_shader.setUniform1f("random_offset_one", ofRandomHeight());
        _transition_shader.setUniform1f("random_offset_two", ofRandomHeight());
        _current_scene->get_frame_buffer().draw(0, 0);
        _transition_shader.end();
    } else {
        _current_scene->draw();
    }

    draw_fps_counter();
    draw_green_frame();
}

void CoreApp::draw_green_frame() {
    ofPushStyle();

    ofNoFill();
    ofSetColor(0, 255, 0);
    ofDrawRectangle(0, 0, static_cast<float>(ofGetWidth()), static_cast<float>(ofGetHeight()));

    ofPopStyle();
}

void CoreApp::draw_fps_counter() const {
    if (!_show_debug_info) {
        return;
    }

    auto oss = std::ostringstream();
    oss << ofToString(ofGetFrameRate(), 2) + " FPS" << std::endl;
    ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

void CoreApp::keyPressed(int key) {
    switch (key) {
        case 'e':
            _tracking_scene.toggle_skeletons();
            break;
        case 'q':
            _tracking_scene.trigger_global_effect({ofGetWidth(), 0});
            break;
        case 'w':
            _tracking_scene.trigger_global_effect({0, 0});
            break;
        case 'a':
            _tracking_scene.trigger_global_effect({ofGetWidth(), ofGetHeight()});
            break;
        case 's':
            _tracking_scene.trigger_global_effect({0, ofGetHeight()});
            break;
        case 'r':
            _tracking_scene.reset_camera();
            break;
        case 'f':
            _show_debug_info = !_show_debug_info;
            break;
        case '1':
            _keyboard_triggered_scene = &_intro_scene;
            break;
        case '2':
            _keyboard_triggered_scene = &_tracking_scene;
            break;
        case '3':
            _keyboard_triggered_scene = nullptr;
            break;
        case 'p':
            _intro_scene.reset_particles();
            break;
        case OF_KEY_F1:
            if (_keyboard_triggered_scene) {
                _keyboard_triggered_scene->save_frame();
            } else {
                _current_scene->save_frame();
            }
        default:
            break;
    }
}

void CoreApp::keyReleased(int key) {}

void CoreApp::mouseMoved(int x, int y) {}

void CoreApp::mouseDragged(int x, int y, int button) {}

void CoreApp::mousePressed(int x, int y, int button) {}

void CoreApp::mouseReleased(int x, int y, int button) {}

void CoreApp::mouseEntered(int x, int y) {}

void CoreApp::mouseExited(int x, int y) {}

void CoreApp::windowResized(int w, int h) {}

void CoreApp::gotMessage(ofMessage msg) {}

void CoreApp::dragEvent(ofDragInfo dragInfo) {}
