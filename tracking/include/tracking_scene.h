#pragma once

#include <vector>

#include <ofMain.h>
#include <ofxAzureKinect.h>

#include <scene.h>

class TrackingScene : public Scene {
  public:
    explicit TrackingScene(ofxAzureKinect::Device *device);

    void update() override;
    void render() override;

  private:
    void draw_bounding_box();
    std::vector<ofVec2f> calculate_convex_hull(const std::vector<ofVec2f> &points);
    void draw_body_outline_2D();

    ofxAzureKinect::Device *kinect_device;

    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;

    ofShader render_shader;

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;
};
