#pragma once

#include <vector>
#include <map>

#include <ofMain.h>
#include <ofxAzureKinect.h>

#include <scene.h>

#include "player.h"
#include "collision_object.h"

class TrackingScene : public Scene {
  public:
    explicit TrackingScene(ofxAzureKinect::Device *device);

    void update() override;
    void render() override;

  private:
    ofxAzureKinect::Device *kinect_device;
    ofEasyCam camera;

    ofVbo points_vbo;
    ofVboMesh skeleton_mesh;
    const std::size_t k_max_bodies = 6;

    std::map<std::uint32_t, Player> players;

    ofFbo pixel_shader_fbo;

    ofShader pixel_shader;
    ofShader render_shader;

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;

    CollisionObject dvd_logo;

    // polyline for collision
    ofPolyline debug_polyline; // Speichert die letzte getestete Polyline

    std::vector<ofPoint> calculate_convex_hull(const ofxAzureKinect::BodySkeleton &skeleton);
    bool check_collision_with_bodies(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons);

    void draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons);

    void update_bouncing_image(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons); // Funktion zur Aktualisierung der Bewegung
    float of_dist_point_to_segment(const glm::vec2 &p, const glm::vec2 &a, const glm::vec2 &b);
    void draw_fake_shaders();
};
