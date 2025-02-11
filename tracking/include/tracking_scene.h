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
    std::vector<ofPoint> calculate_convex_hull(const ofxAzureKinect::BodySkeleton &skeleton);

    ofxAzureKinect::Device *kinect_device;

    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;

    ofFbo pixel_shader_fbo;
    ofFbo degauss_fbo;

    ofShader pixel_shader;
    ofShader render_shader;
    ofShader degaussing_shader;

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;
};
