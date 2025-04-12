#include "tracking_scene.h"

#include <ofxConvexHull.h>

#include <algorithm>
#include <vector>

TrackingScene::TrackingScene(ofxAzureKinect::Device *device) : kinect_device(device) {
    collision_objects = createCollisionObjects();
}

void TrackingScene::update() {
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    std::erase_if(players, [&body_skeletons](const Player &player) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id) {
                return false;
            }
        }
        return true;
    });

    for (auto &player: players) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id) {
                player.set_skeleton(skeleton);
                break;
            }
        }
    }

    for (const auto &skeleton: body_skeletons) {
        if (!std::any_of(players.cbegin(), players.cend(), [&](auto &player) { return player.id == skeleton.id; })) {
            players.emplace_back(skeleton.id, &camera);
        }
    }

    for (auto &collision_object: collision_objects) {
        collision_object.update(players, camera);
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

    frame_buffer.begin();
    {
        ofClear(0, 0, 0, 0);

        for (auto &player: players) {
            player.draw();
        }

        for (const auto &obj: collision_objects) {
            obj.draw();
        }

        draw_fake_shaders();

        draw_skeletons(body_skeletons);
    }
    frame_buffer.end();
}

void TrackingScene::draw_skeletons(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons) {
    camera.begin();
    {
        ofPushMatrix();
        {
            ofRotateXDeg(180);
            for (const auto &skeleton: skeletons) {
                // Draw joints.
                for (int i = 0; i < K4ABT_JOINT_COUNT; ++i) {
                    auto joint = skeleton.joints[i];
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

                // Draw connections.
                skeleton_mesh.setMode(OF_PRIMITIVE_LINES);
                auto &vertices = skeleton_mesh.getVertices();
                vertices.resize(50);
                int vdx = 0;

                // Spine.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HEAD].position);

                // Head.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HEAD].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EAR_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NOSE].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EYE_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_EAR_RIGHT].position);

                // Left Leg.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_LEFT].position);

                // Right leg.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_RIGHT].position);

                // Left arm.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_LEFT].position);

                // Right arm.
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);

                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);
                vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_RIGHT].position);

                skeleton_mesh.draw();
            }
        }
        ofPopMatrix();
    }
    camera.end();
}

void TrackingScene::draw_fake_shaders() {
    for (std::size_t i = 0; i < std::min(players.size(), k_max_bodies); ++i) {
        Player player = players[i];
        std::string player_id = player.get_id();
        ofDrawBitmapStringHighlight("Player " + player_id + ": " + player.get_fake_shader(), 100, 20 + (i * 20));
    }
}

std::vector<EffectShader> TrackingScene::createCollisionObjects() {
    if (effect_shader_paths.size() != collision_object_image_paths.size()) {
        ofLogError(__FUNCTION__) << "Effect shader paths and collision object image paths must have the same size!";
        return std::vector<EffectShader>();
    } else if (effect_shader_paths.size() == 0 && collision_object_image_paths.size() == 0) {
        ofLogError(__FUNCTION__) << "Effect shader paths and collision object image paths must not be empty!";
        return std::vector<EffectShader>();
    } else {
        std::vector<EffectShader> collision_objects;
        for (std::size_t i = 0; i < effect_shader_paths.size(); ++i) {
            auto effect_shader = ofShader();
            effect_shader.load(effect_shader_paths[i]);

            auto position = glm::vec2(ofRandom(5, 1000), ofRandom(5, 500));
            auto velocity = glm::vec2(ofRandom(-100, 100), ofRandom(-100, 100));
            velocity = 8 * glm::normalize(velocity);
            string fake_shader = "No. " + std::to_string(i);
            collision_objects.emplace_back(position, velocity, collision_object_image_paths[i], fake_shader,
                                           effect_shader);
        }
        return collision_objects;
    }
}
