#include "tracking_scene.h"

#include <ofxConvexHull.h>

#include <algorithm>
#include <vector>

TrackingScene::TrackingScene(ofxAzureKinect::Device *device) : kinect_device(device) {
    // load shaders
    auto shader_settings = ofShaderSettings();
    shader_settings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render.vert";
    shader_settings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render.frag";
    shader_settings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
    shader_settings.bindDefaults = true;
    if (render_shader.setup(shader_settings)) {
        ofLogNotice(__FUNCTION__) << "Success loading shader!";
    }

    pixel_shader.load("shaders/pixel");
    pixel_shader_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    // setup vbo
    std::vector<glm::vec3> verts(1);
    points_vbo.setVertexData(verts.data(), static_cast<int>(verts.size()), GL_STATIC_DRAW);

    // setup random generator
    const auto min_random_value = -1000;
    const auto max_random_value = 1000;

    std::random_device random;
    generator = std::mt19937(random());
    distribution = std::uniform_real_distribution<float>(min_random_value, max_random_value);

    //creates all collisionobjects with shaders and images
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

    for (auto &player : players) {
        for (const auto &skeleton: body_skeletons) {
            if (skeleton.id == player.id) {
                player.set_skeleton(skeleton);
                break;
            }
        }
    }

    for (const auto &skeleton: body_skeletons) {
        if (!std::any_of(players.cbegin(), players.cend(), [&](auto &player) {
            return player.id == skeleton.id;
            })) {
            players.emplace_back(skeleton.id, &camera);
        }
    }

    std::vector<std::vector<ofPoint>> convex_hulls;

    for (const auto &skeleton: body_skeletons) {
        convex_hulls.emplace_back(calculate_convex_hull(skeleton));
    }

    //updates all collisionobjects
    for (auto &obj: collision_objects) {
        obj.update(players, camera);
    }
}

void TrackingScene::render() {
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    auto body_ids = std::vector<int>(k_max_bodies, 0);

    std::vector<std::vector<ofPoint>> convex_hulls;
    for (std::size_t i = 0; i < std::min(body_skeletons.size(), k_max_bodies); ++i) {
        body_ids[i] = static_cast<int>(body_skeletons[i].id);
        convex_hulls.emplace_back(calculate_convex_hull(body_skeletons[i]));
    }

    // Berechnung der Shake-Amplituden pro Körper
    std::vector<float> shake_amplitudes(k_max_bodies, 0.0f);
    float screen_shake_amplitude = 0.0f;
    float max_shake_amplitude = 0.0f;
    float pixel_block_size = 0;

    for (std::size_t i = 0; i < convex_hulls.size(); ++i) {
        float area = 0.0f;
        const auto &hull = convex_hulls[i];

        for (std::size_t j = 0; j < hull.size(); ++j) {
            auto k = (j + 1) % hull.size();
            area += hull[j].x * hull[k].y - hull[k].x * hull[j].y;
        }

        area = std::abs(area) / 2.0f;
        const float min_area = 496604;
        const float max_area = 1.51127e+06;

        screen_shake_amplitude = ofMap(area, min_area, max_area, 0, 75, true);
        pixel_block_size = std::max(pixel_block_size, ofMap(area, min_area, max_area, 0, 20, true));
    }

    pixel_shader_fbo.begin();
    {
        ofClear(0);
        ofBackground(0);

        camera.begin();
        {
            ofPushMatrix();
            {
                ofRotateXDeg(180);
                ofEnableDepthTest();

                render_shader.begin();
                {
                    const auto frame_width = static_cast<int>(kinect_device->getDepthTex().getWidth());
                    const auto frame_height = static_cast<int>(kinect_device->getDepthTex().getHeight());

                    render_shader.setUniformTexture("depth_texture", kinect_device->getDepthTex(), 1);
                    render_shader.setUniformTexture("body_index_texture", kinect_device->getBodyIndexTex(), 2);
                    render_shader.setUniformTexture("world_texture", kinect_device->getDepthToWorldTex(), 3);
                    render_shader.setUniform2i("frame_size", frame_width, frame_height);
                    render_shader.setUniform1iv("body_ids", body_ids.data(), k_max_bodies);

                    render_shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
                    render_shader.setUniform1f("random_offset_one", distribution(generator));
                    render_shader.setUniform1f("random_offset_two", distribution(generator));

                    const int num_points = frame_width * frame_height;
                    points_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
                }
                render_shader.end();
                ofDisableDepthTest();
            }
            ofPopMatrix();
        }
        camera.end();
    }
    pixel_shader_fbo.end();

    frame_buffer.begin();
    {
        ofClear(0, 0, 0, 0);
        pixel_shader_fbo.draw(0, 0);
        /*
        pixel_shader_fbo.draw(0, 0);
        pixel_shader.begin();
        {
            pixel_shader.setUniform1f("block_size", pixel_block_size);
            pixel_shader.setUniform1f("quality", 0.5f);
            pixel_shader_fbo.draw(0, 0);
        }
        pixel_shader.end();
        */

        // draws all collisionobjects
        for (const auto &obj: collision_objects) {
            obj.draw();
        }

        draw_fake_shaders();

        draw_players(players);
    }
    frame_buffer.end();
}

void TrackingScene::draw_players(std::vector<Player> &players) {
    camera.begin();
    {
        ofPushMatrix();
        {
            ofRotateXDeg(180);
            for ( auto &player: players) {
                ofxAzureKinect::BodySkeleton skeleton = player.get_skeleton();
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

                draw_skeleton_connections(skeleton, player);
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

std::vector<CollisionObject> TrackingScene::createCollisionObjects() {
    if (effect_shader_paths.size() != collision_object_image_paths.size()) {
        ofLogError(__FUNCTION__) << "Effect shader paths and collision object image paths must have the same size!";
        return std::vector<CollisionObject>();
    } else if (effect_shader_paths.size() == 0 && collision_object_image_paths.size() == 0) {
        ofLogError(__FUNCTION__) << "Effect shader paths and collision object image paths must not be empty!";
        return std::vector<CollisionObject>();
    } else {
        std::vector<CollisionObject> collision_objects;
        for (std::size_t i = 0; i < effect_shader_paths.size(); ++i) {
            auto effect_shader = ofShader();
            effect_shader.load(effect_shader_paths[i]);
            auto position = glm::vec2(ofRandom(5, 1000), ofRandom(5, 500));
            auto velocity = glm::vec2(ofRandom(-100, 100), ofRandom(-100, 100));
            velocity = 8 * glm::normalize(velocity);
            string fake_shader = "No. " + std::to_string(i);
            collision_objects.emplace_back(position, velocity, collision_object_image_paths[i], fake_shader, effect_shader);
        }
        return collision_objects;
    }
}

void TrackingScene::draw_skeleton_connections(const ofxAzureKinect::BodySkeleton &skeleton, Player &player) {
    skeleton_mesh.setMode(OF_PRIMITIVE_LINES);
    auto &vertices = skeleton_mesh.getVertices();
    vertices.resize(50);
    int vdx = 0;

    // Spine
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_NAVEL].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SPINE_CHEST].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HEAD].position);

    // Head
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

    // Left Leg
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_LEFT].position);

    // Right Leg
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_PELVIS].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_HIP_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_KNEE_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ANKLE_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_FOOT_RIGHT].position);

    // Left Arm
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_LEFT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_LEFT].position);

    // Right Arm
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_NECK].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_CLAVICLE_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_SHOULDER_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);

    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_ELBOW_RIGHT].position);
    vertices[vdx++] = toGlm(skeleton.joints[K4ABT_JOINT_WRIST_RIGHT].position);

    player.set_skeleton_vertices(vertices);

    skeleton_mesh.draw();
}

std::vector<ofPoint> TrackingScene::calculate_convex_hull(const ofxAzureKinect::BodySkeleton &skeleton) {
    ofxConvexHull convex_hull_calculator; // Instantiate the convex hull object
    const float offset_distance = 0.1f; // Offset in meters (10 cm)

    std::vector<ofPoint> projected_points; // Use ofPoint for compatibility
    float front_depth = FLT_MAX; // Start with the maximum possible value

    // Collect valid joint positions and calculate the front depth
    for (const auto &joint: skeleton.joints) {
        if (std::isfinite(joint.position.x) && std::isfinite(joint.position.y) && std::isfinite(joint.position.z)) {
            ofVec3f world_position(joint.position.x, joint.position.y, joint.position.z);
            ofVec3f screen_position = camera.worldToScreen(world_position);

            // Add to the projected_points vector as ofPoint
            projected_points.emplace_back(screen_position.x, screen_position.y);

            // Track the minimum depth value (front of the bounding box)
            front_depth = std::min(front_depth, world_position.z);
        }
    }

    auto output_hull = std::vector<ofPoint>();
    if (projected_points.size() > 2) {
        // Compute the convex hull of the projected points
        std::vector<ofPoint> hull = convex_hull_calculator.getConvexHull(projected_points);

        // Expand the convex hull outward by offset_distance
        std::vector<ofPoint> expanded_hull;
        for (size_t i = 0; i < hull.size(); ++i) {
            const ofPoint &current = hull[i];
            const ofPoint &next = hull[(i + 1) % hull.size()];

            // Compute the edge normal
            ofVec2f edge = ofVec2f(next.x - current.x, next.y - current.y);
            ofVec2f normal(-edge.y, edge.x); // Perpendicular to the edge
            normal.normalize();

            // Move the point outward by the offset distance
            expanded_hull.emplace_back(current.x + normal.x * offset_distance, current.y + normal.y * offset_distance);
        }

        // Project the 2D hull points back into 3D using the front depth
        for (const auto &point: expanded_hull) {
            output_hull.emplace_back(camera.screenToWorld(
                    ofVec3f(point.x, point.y, camera.worldToScreen(ofVec3f(0, 0, front_depth)).z)));
        }
    }
    return output_hull;
}
