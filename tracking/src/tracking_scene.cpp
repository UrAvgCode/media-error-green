#include "tracking_scene.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "effect_shader.h"
#include "glitch_effect_shader.h"
#include "pixel_effect_shader.h"
#include "signalloss_effect_shader.h"
#include "skeleton_utility.h"
#include "warp_effect_shader.h"

TrackingScene::TrackingScene(ofxAzureKinect::Device *device) :
    kinect_device(device), number_of_objects(3), global_effect_duration(3000) {
    screen_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    global_effect_shader.load("shaders/global_effect");
    global_effect_position = {0, 0};
    global_effect_trigger_time = 0;

    effect_shaders = {std::make_shared<EffectShader>(), std::make_shared<PixelEffectShader>(),
                      std::make_shared<GlitchEffectShader>(), std::make_shared<WarpEffectShader>(),
                      std::make_shared<SignallossEffectShader>()};

    for (std::size_t i = 0; i < number_of_objects; ++i) {
        auto position = glm::vec2(ofRandom(5, 1000), ofRandom(5, 500));
        auto velocity = glm::vec2(ofRandom(-100, 100), ofRandom(-100, 100));
        velocity = 8 * glm::normalize(velocity);

        const auto effect_shader = effect_shaders.front();
        effect_shaders.erase(effect_shaders.begin());

        collision_objects.emplace_back(position, velocity, "resources/dvd-logo.png", effect_shader);
    }

    _skeletons_enabled = false;
}

void TrackingScene::update() {
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    std::erase_if(players, [&body_skeletons](const Player &player) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id()) {
                return false;
            }
        }
        return true;
    });

    for (auto &player: players) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id()) {
                player.set_skeleton(skeleton);
                break;
            }
        }
    }

    for (const auto &skeleton: body_skeletons) {
        if (!std::any_of(players.cbegin(), players.cend(), [&](auto &player) { return player.id() == skeleton.id; })) {
            players.emplace_back(skeleton.id, skeleton, &camera);
        }
    }

    for (auto &player: players) {
        player.calculate_skeleton_lines();
    }

    for (auto &collision_object: collision_objects) {
        collision_object.update(players, camera);

        auto [triggered, position] = collision_object.global_effect_triggered();

        if (triggered) {
            auto global_effect_elapsed_time = ofGetSystemTimeMillis() - global_effect_trigger_time;
            if (global_effect_elapsed_time > global_effect_duration) {
                trigger_global_effect(position);

                auto index = static_cast<std::size_t>(ofRandom(0, effect_shaders.size() - 1));

                const auto effect_shader = effect_shaders[index];
                effect_shaders.erase(effect_shaders.begin() + index);
                effect_shaders.push_back(collision_object.effect_shader());

                collision_object.set_effect_shader(effect_shader);
            }
        }
    }
}

void TrackingScene::render() {
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    auto body_ids = std::vector<int>(k_max_bodies, 0);
    for (std::size_t i = 0; i < std::min(body_skeletons.size(), k_max_bodies); ++i) {
        body_ids[i] = static_cast<int>(body_skeletons[i].id);
    }

    for (auto &player: players) {
        player.render(kinect_device->getDepthTex(), kinect_device->getBodyIndexTex(),
                      kinect_device->getDepthToWorldTex(), body_ids);
    }

    screen_fbo.begin();
    {
        ofClear(0, 0, 0, 0);

        for (auto &player: players) {
            player.draw();
        }

        for (const auto &obj: collision_objects) {
            obj.draw();
        }

        if (_skeletons_enabled) {
            draw_skeletons(body_skeletons);
        }
    }
    screen_fbo.end();

    frame_buffer.begin();
    {
        ofClear(0, 0, 0, 0);

        auto global_effect_elapsed_time = ofGetSystemTimeMillis() - global_effect_trigger_time;
        if (global_effect_elapsed_time < global_effect_duration) {
            global_effect_shader.begin();
            global_effect_shader.setUniform2f("effect_position", global_effect_position);
            global_effect_shader.setUniform2f("texture_size", ofGetWidth(), ofGetHeight());

            global_effect_shader.setUniform1i("duration", global_effect_duration);
            global_effect_shader.setUniform1i("elapsed_time", global_effect_elapsed_time);

            global_effect_shader.setUniform2f("aspect", 1, ofGetWidth() / ofGetHeight());
            global_effect_shader.setUniform1f("time", ofGetElapsedTimef());
            global_effect_shader.setUniform1f("effectAmount", 1.0f);
            global_effect_shader.setUniform1f("radius", 10.0f);
        }

        ofPushMatrix();
        ofTranslate(screen_fbo.getWidth(), 0);
        ofScale(-1, 1);

        screen_fbo.draw(0, 0);

        ofPopMatrix();

        if (global_effect_elapsed_time < global_effect_duration) {
            global_effect_shader.end();
        }
    }
    frame_buffer.end();
}

void TrackingScene::trigger_global_effect(glm::vec2 position) {
        global_effect_position = position;
        global_effect_trigger_time = ofGetSystemTimeMillis();
}

void TrackingScene::toggle_skeletons() { _skeletons_enabled = !_skeletons_enabled; }

void TrackingScene::draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons) {
    camera.begin();
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

            skeleton_mesh.setMode(OF_PRIMITIVE_LINES);
            auto &vertices = skeleton_mesh.getVertices();
            vertices.resize(50);

            int vdx = 0;
            for (const auto &[start, end]: skeleton::connections) {
                vertices[vdx++] = skeleton.joints[start].position;
                vertices[vdx++] = skeleton.joints[end].position;
            }

            skeleton_mesh.draw();
        }
    }
    ofPopMatrix();
    camera.end();
}
