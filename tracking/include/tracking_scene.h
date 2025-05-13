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

    void toggle_skeletons();

    void reset_camera();

  private:
    void draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons);

    ofxAzureKinect::Device *_kinect_device;
    ofEasyCam _camera;

    ofFbo _screen_fbo;

    const std::size_t _max_bodies = 6;
    std::vector<Player> _players;

    bool _skeletons_enabled;
    ofVboMesh _skeleton_mesh;

    const std::size_t _number_of_objects;
    std::vector<CollisionObject> _collision_objects;
    std::vector<std::shared_ptr<EffectShader>> _effect_shaders;

    const std::uint64_t _global_effect_duration;
    std::uint64_t _global_effect_trigger_time;
    ofShader _global_effect_shader;
    glm::vec2 _global_effect_position;

    ofSoundPlayer _global_effect;

    int _line_position;
};
