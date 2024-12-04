#include "ofApp.h"

ofShader shader;
ofFbo fbo;
ofImage image;
float aberration = 10;
float shader_time = 0;

//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("shaders/chromatic");

    image.load("res/logo.png");
    fbo.allocate(image.getWidth() + 100, image.getHeight() + 100, GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){
    aberration = ofMap(mouseX, 0, ofGetWidth(), 0, 100);
    shader_time += 0.5f;
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    image.draw(50, 50);
    fbo.end();

    shader.begin();
    shader.setUniformTexture("tex0", fbo.getTexture(), 0);
    shader.setUniform1f("aberrationAmount", aberration);
	shader.setUniform1f("time", shader_time);

    fbo.draw(0, 0);

    shader.end();
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
