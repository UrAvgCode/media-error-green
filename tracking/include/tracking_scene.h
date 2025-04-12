#pragma once

#include <vector>

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

    std::vector<Player> players;

    ofFbo pixel_shader_fbo;

    ofShader pixel_shader;
    ofShader render_shader;
    const vector<string> effect_shader_paths = {"shaders/effect/effect_shader1", "shaders/effect/effect_shader2"};

    vector<string> collision_object_image_paths = {"resources/dvd-logo.png", "resources/me-logo-green.png"};

    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;

    std::vector<CollisionObject> collision_objects;
    CollisionObject dvd_logo;
    CollisionObject me_logo;

    // polyline for collision
    ofPolyline debug_polyline; // Speichert die letzte getestete Polyline

    std::vector<ofPoint> calculate_convex_hull(const ofxAzureKinect::BodySkeleton &skeleton);

    void draw_players(const std::vector<Player> &players);

    std::vector<CollisionObject> createCollisionObjects();
    void draw_skeleton_connections(const ofxAzureKinect::BodySkeleton &skeleton);
    void draw_fake_shaders();
};
