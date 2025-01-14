#pragma once

#include <vector>

#include <ofMain.h>
#include <ofxAzureKinect.h>

class TrackingApp : public ofBaseApp {
  public:
    void setup() override;
    void exit() override;

    void update() override;
    void draw() override;

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;

  private:
    void draw_skeleton(const std::vector<ofxAzureKinect::BodySkeleton> &body_skeletons);

    ofxAzureKinect::Device kinect_device;

    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;

    ofShader shader;

    ofFbo fbo;
};
