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
    bool check_collision_with_bodies(const std::vector<std::vector<ofPoint>> &convexHulls);

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
    string image_path = "dvd-logo-green.png";
    glm::vec2 image_position; // Position des Bildes
    glm::vec2 image_velocity; // Geschwindigkeit des Bildes

    //polyline for collision
    ofPolyline debug_polyline; // Speichert die letzte getestete Polyline

    float image_scale = 1;
    float image_width, image_height; // Bildgröße

    void update_bouncing_image(const std::vector<std::vector<ofPoint>> &convexHulls); // Funktion zur Aktualisierung der Bewegung
    float of_dist_point_to_segment(const glm::vec2 &p, const glm::vec2 &a, const glm::vec2 &b);
};
