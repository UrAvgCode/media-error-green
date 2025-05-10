#include "tracking_scene.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "chromatic_effect_shader.h"
#include "effect_shader.h"
#include "glitch_effect_shader.h"
#include "pixel_effect_shader.h"
#include "signalloss_effect_shader.h"
#include "skeleton_utility.h"
#include "warp_effect_shader.h"

TrackingScene::TrackingScene(ofxAzureKinect::Device *device) :
    _kinect_device(device), _number_of_objects(3), _global_effect_duration(1000) {
    _screen_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    _global_effect_shader.load("shaders/global_effect");
    _global_effect_position = {0, 0};
    _global_effect_trigger_time = 0;

    _global_effect.load("resources/audio/gruen_globalEffect.wav");
    _global_effect.setMultiPlay(false);

    _effect_shaders = {std::make_shared<EffectShader>(),           std::make_shared<PixelEffectShader>(),
                       std::make_shared<GlitchEffectShader>(),     std::make_shared<WarpEffectShader>(),
                       std::make_shared<SignallossEffectShader>(), std::make_shared<ChromaticEffectShader>()};

    for (std::size_t i = 0; i < _number_of_objects; ++i) {
        auto position = glm::vec2(ofRandom(5, 1000), ofRandom(5, 500));
        auto velocity = glm::vec2(ofRandom(-100, 100), ofRandom(-100, 100));
        velocity = 8 * glm::normalize(velocity);

        const auto effect_shader = _effect_shaders.front();
        _effect_shaders.erase(_effect_shaders.begin());

        _collision_objects.emplace_back(position, velocity, "resources/dvd-logo.png", effect_shader);
    }

    _skeletons_enabled = false;
    _line_position = 0;
}

void TrackingScene::update() {
    const auto &body_skeletons = _kinect_device->getBodySkeletons();

    std::erase_if(_players, [&body_skeletons](const Player &player) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id()) {
                return false;
            }
        }
        return true;
    });

    for (auto &player: _players) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id()) {
                player.set_skeleton(skeleton);
                break;
            }
        }
    }

    for (const auto &skeleton: body_skeletons) {
        if (!std::any_of(_players.cbegin(), _players.cend(),
                         [&](auto &player) { return player.id() == skeleton.id; })) {
            _players.emplace_back(skeleton.id, skeleton, &_camera);
        }
    }

    for (auto &player: _players) {
        player.calculate_skeleton_lines();
    }

    for (auto &collision_object: _collision_objects) {
        collision_object.update(_players, _camera);

        if (ofGetSystemTimeMillis() - _global_effect_trigger_time > _global_effect_duration) {
            auto [triggered, position] = collision_object.global_effect_triggered();

            if (triggered) {
                trigger_global_effect(position);

                auto index = static_cast<std::size_t>(ofRandom(0, _effect_shaders.size() - 1));

                const auto effect_shader = _effect_shaders[index];
                _effect_shaders.erase(_effect_shaders.begin() + index);
                _effect_shaders.push_back(collision_object.effect_shader());

                collision_object.set_effect_shader(effect_shader);
            }
        }
    }
}

void TrackingScene::render() {
    const auto &body_skeletons = _kinect_device->getBodySkeletons();

    auto body_ids = std::vector<int>(_max_bodies, 0);
    for (std::size_t i = 0; i < std::min(body_skeletons.size(), _max_bodies); ++i) {
        body_ids[i] = static_cast<int>(body_skeletons[i].id);
    }

    for (auto &player: _players) {
        player.render(_kinect_device->getDepthTex(), _kinect_device->getBodyIndexTex(),
                      _kinect_device->getDepthToWorldTex(), body_ids);
    }

    _screen_fbo.begin();
    {
        ofClear(0, 0, 0, 0);

        for (auto &player: _players) {
            player.draw();
        }

        for (const auto &obj: _collision_objects) {
            obj.draw();
        }

        if (_skeletons_enabled) {
            draw_skeletons(body_skeletons);
        }
    }
    _screen_fbo.end();

    frame_buffer.begin();
    {
        ofClear(0, 0, 0, 1);

        _global_effect_shader.begin();
        _global_effect_shader.setUniform2f("effect_position", _global_effect_position);
        _global_effect_shader.setUniform2f("texture_size", ofGetWidth(), ofGetHeight());

        _global_effect_shader.setUniform1i("duration", _global_effect_duration);
        _global_effect_shader.setUniform1i("elapsed_time", ofGetSystemTimeMillis() - _global_effect_trigger_time);

        _global_effect_shader.setUniform2f("aspect", 1, ofGetWidth() / ofGetHeight());
        _global_effect_shader.setUniform1f("time", ofGetElapsedTimef());
        _global_effect_shader.setUniform1f("effectAmount", 1.0f);
        _global_effect_shader.setUniform1f("radius", 10.0f);

        _line_position = (_line_position + 1) % ofGetHeight();
        _global_effect_shader.setUniform1i("line_position", _line_position);

        ofPushMatrix();
        ofTranslate(_screen_fbo.getWidth(), 0);
        ofScale(-1, 1);

        _screen_fbo.draw(0, 0);

        ofPopMatrix();

        _global_effect_shader.end();
    }
    frame_buffer.end();
}

void TrackingScene::trigger_global_effect(glm::vec2 position) {
    _global_effect.setVolume(0.06f);
    _global_effect.play();

    _global_effect_position = position;
    _global_effect_trigger_time = ofGetSystemTimeMillis();
}

void TrackingScene::toggle_skeletons() { _skeletons_enabled = !_skeletons_enabled; }

void TrackingScene::draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons) {
    _camera.begin();
    ofPushMatrix();
    ofRotateXDeg(180);
    {
        for (const auto &skeleton: skeletons) {
            for (const auto joint: skeleton.joints) {
                ofPushMatrix();
                {
                    glm::mat4 transform = glm::translate(joint.position) * glm::toMat4(joint.orientation);
                    ofMultMatrix(transform);

                    ofDrawAxis(50.0f);

                    if (joint.confidenceLevel >= K4ABT_JOINT_CONFIDENCE_MEDIUM) {
                        ofSetColor(ofColor::green);
                    } else if (joint.confidenceLevel >= K4ABT_JOINT_CONFIDENCE_LOW) {
                        ofSetColor(ofColor::yellow);
                    } else {
                        ofSetColor(ofColor::red);
                    }

                    ofDrawSphere(10.0f);
                }
                ofPopMatrix();
            }

            _skeleton_mesh.setMode(OF_PRIMITIVE_LINES);
            auto &vertices = _skeleton_mesh.getVertices();
            vertices.resize(50);

            int vdx = 0;
            for (const auto &[start, end]: skeleton::connections) {
                vertices[vdx++] = skeleton.joints[start].position;
                vertices[vdx++] = skeleton.joints[end].position;
            }

            _skeleton_mesh.draw();
        }
    }
    ofPopMatrix();
    _camera.end();
}
