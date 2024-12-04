#include "ofApp.h"

ofShader shader;
ofFbo fbo;
ofImage image;

//--------------------------------------------------------------
void ofApp::setup(){
	shader.load("shaders/chromatic");

    image.load("res/logo.png");
    fbo.allocate(image.getWidth(), image.getHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    image.draw(0, 0);
    fbo.end();

    shader.begin();
    shader.setUniformTexture("tex0", fbo.getTexture(), 0);
    shader.setUniform1f("aberrationAmount", 10);
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
