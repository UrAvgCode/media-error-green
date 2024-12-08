#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setVerbose(true);
    vidGrabber.setup(640, 480);

    colorImg.allocate(640, 480);
    grayImg.allocate(640, 480);
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();

    if (vidGrabber.isFrameNew()) {
        colorImg.setFromPixels(vidGrabber.getPixels());
        grayImg = colorImg;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    colorImg.draw(0, 0);
    grayImg.draw(640, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
