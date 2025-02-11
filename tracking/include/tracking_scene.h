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

    ofShader pixel_shader;
    ofShader render_shader;

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;

    ofImage bouncing_image; // Das Bild
    glm::vec2 image_position; // Position des Bildes
    glm::vec2 image_velocity; // Geschwindigkeit des Bildes

    float image_scale = 0.5;
    float image_width, image_height; // Bildgröße

    void update_bouncing_image(); // Funktion zur Aktualisierung der Bewegung
};
