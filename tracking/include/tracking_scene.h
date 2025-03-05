#pragma once

#include <vector>

#include <ofxSvg.h>

#include <ofMain.h>
#include <ofxAzureKinect.h>

#include <scene.h>

class TrackingScene : public Scene {
  public:
    explicit TrackingScene(ofxAzureKinect::Device *device);

    void update() override;
    void render() override;
    bool isCollidingWithBody(glm::vec2);

  private:
    void draw_bounding_box();
    std::vector<ofPoint> calculate_convex_hull(const ofxAzureKinect::BodySkeleton &skeleton);

    ofxAzureKinect::Device *kinect_device;

    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;

    ofFbo pixel_shader_fbo;

    ofShader pixel_shader;
    ofShader render_shader;

    ofRectangle svgBoundingBox;
    ofxSvg floatingSvg;
    float logoScale;
    ofVec2f svgPos;
    ofVec2f svgVel;
    float timeCounter;

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;
};
