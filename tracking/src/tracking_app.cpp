#include "tracking_app.h"

//--------------------------------------------------------------
void TrackingApp::setup() {
    // ofSetLogLevel(OF_LOG_VERBOSE);

    ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

    if (kinectDevice.open()) {
        auto kinectSettings = ofxAzureKinect::DeviceSettings();
        kinectSettings.syncImages = false;
        kinectSettings.updateWorld = false;
        kinectDevice.startCameras(kinectSettings);
    }
}

//--------------------------------------------------------------
void TrackingApp::exit() { kinectDevice.close(); }

//--------------------------------------------------------------
void TrackingApp::update() {
    if (kinectDevice.isFrameNew()) {
        kinectFps.newFrame();
    }
}

//--------------------------------------------------------------
void TrackingApp::draw() {
    ofBackground(128);

    if (kinectDevice.isStreaming()) {
        kinectDevice.getColorTex().draw(0, 0, 1280, 720);
        kinectDevice.getDepthTex().draw(1280, 0, 360, 360);
        kinectDevice.getIrTex().draw(1280, 360, 360, 360);
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "APP: " << ofGetFrameRate() << " FPS" << std::endl
        << "K4A: " << kinectFps.getFps() << " FPS";
    ofDrawBitmapStringHighlight(oss.str(), 10, 20);
}

//--------------------------------------------------------------
void TrackingApp::keyPressed(int key) {}

//--------------------------------------------------------------
void TrackingApp::keyReleased(int key) {}

//--------------------------------------------------------------
void TrackingApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void TrackingApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void TrackingApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void TrackingApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void TrackingApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void TrackingApp::dragEvent(ofDragInfo dragInfo) {}
