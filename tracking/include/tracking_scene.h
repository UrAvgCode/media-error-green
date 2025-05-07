#pragma once

#include <vector>

#include <ofMain.h>
#include <ofxAzureKinect.h>

#include <scene.h>

#include "collision_object.h"
#include "player.h"

class TrackingScene : public Scene {
  public:
    explicit TrackingScene(ofxAzureKinect::Device *device);

    void update() override;
    void render() override;

    void trigger_global_effect(glm::vec2);

  private:
    ofxAzureKinect::Device *kinect_device;
    ofEasyCam camera;

    const std::size_t k_max_bodies = 6;

    std::vector<Player> players;
    std::vector<CollisionObject> collision_objects;

    ofVboMesh skeleton_mesh;

    ofFbo screen_fbo;

    ofShader global_effect_shader;
    glm::vec2 global_effect_position;
    std::uint64_t global_effect_trigger_time;

    const std::uint64_t global_effect_duration;

    void draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons);
};
