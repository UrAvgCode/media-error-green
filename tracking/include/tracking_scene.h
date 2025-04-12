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

    const std::size_t k_max_bodies = 6;

    std::vector<Player> players;
    std::vector<EffectShader> collision_objects;

    const vector<string> effect_shader_paths = {"shaders/effect/effect_shader1", "shaders/effect/effect_shader2"};
    const vector<string> collision_object_image_paths = {"resources/dvd-logo.png", "resources/me-logo-green.png"};

    EffectShader dvd_logo;
    EffectShader me_logo;

    ofVboMesh skeleton_mesh;

    void draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons);

    std::vector<EffectShader> createCollisionObjects();
    void draw_fake_shaders();
};
