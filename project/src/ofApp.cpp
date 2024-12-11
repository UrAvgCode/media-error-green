#include "ofApp.h"
#include "ofxSvg.h"

ofShader chromatic_shader;
ofShader blur_shader;

ofFbo fbo_logo;
ofFbo fbo_intermediate;
ofxSvg logo;

int kernel_size = 0;
float aberration = 10;

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(0);

	chromatic_shader.load("shaders/chromatic");
    blur_shader.load("shaders/blur");

    logo.load("res/logo.svg");
    fbo_logo.allocate(ofGetHeight(), ofGetWidth(), GL_RGBA);
    fbo_intermediate.allocate(ofGetHeight(), ofGetWidth(), GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){
    aberration = ofMap(mouseX, 0, ofGetWidth(), 0, 100);
    kernel_size = ofMap(mouseY, 0, ofGetHeight(), 1, 15);
	kernel_size += kernel_size % 2 == 0 ? 1 : 0;
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo_logo.begin();
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2.0f - logo.getWidth() / 2, ofGetHeight() / 2.0f - logo.getHeight() / 2);
    logo.draw();
    ofPopMatrix();
    fbo_logo.end();

	fbo_intermediate.begin();
    chromatic_shader.begin();

    chromatic_shader.setUniform1f("aberration_amount", aberration);
    chromatic_shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
    chromatic_shader.setUniform1f("rand1", static_cast<float>(rand() % static_cast<int>(fbo_intermediate.getHeight())));
    chromatic_shader.setUniform1f("rand2", static_cast<float>(rand() % static_cast<int>(fbo_intermediate.getHeight())));
    fbo_logo.draw(0, 0);

    chromatic_shader.end();
	fbo_intermediate.end();

	blur_shader.begin();

	blur_shader.setUniform1i("kernel_size", kernel_size);
    blur_shader.setUniform1f("sigma", static_cast<float>(kernel_size) / 3.0f);
    fbo_intermediate.draw(0, 0);

	blur_shader.end();
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
