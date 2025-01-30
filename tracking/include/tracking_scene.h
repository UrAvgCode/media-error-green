#pragma once

#include <vector>

#include <ofMain.h>
#include <ofxAzureKinect.h>

#include <scene.h>

class TrackingScene : public Scene {
  public:
    TrackingScene(ofxAzureKinect::Device *device);

    void update() override;
    void draw() override;

  private:
    void draw_skeleton(const std::vector<ofxAzureKinect::BodySkeleton> &body_skeletons);
    void draw_bounding_box();
    std::vector<ofVec2f> calculate_convex_hull(const std::vector<ofVec2f> &points);
    void draw_body_outline_2D(const std::vector<ofxAzureKinect::BodySkeleton> &body_skeletons, const ofCamera &camera);

    ofxAzureKinect::Device *kinect_device;

    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;

    ofShader shader;
    ofShader chromatic_shader;

    ofFbo fbo;

    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
};
