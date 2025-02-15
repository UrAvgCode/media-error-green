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
}

void TrackingScene::update() {}

void TrackingScene::render() {
    const std::size_t k_max_bodies = 6;
    const auto &body_skeletons = kinect_device->getBodySkeletons();

    auto body_ids = std::vector<int>(k_max_bodies, 0);

    std::vector<std::vector<ofPoint>> convex_hulls;
    for (std::size_t i = 0; i < std::min(body_skeletons.size(), k_max_bodies); ++i) {
        body_ids[i] = static_cast<int>(body_skeletons[i].id);
        convex_hulls.emplace_back(calculate_convex_hull(body_skeletons[i]));
    }

    // Berechnung der Shake-Amplituden pro K�rper
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
                    render_shader.setUniform1f("screen_shake_amplitude", screen_shake_amplitude);

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
        pixel_shader.begin();
        {
            pixel_shader.setUniform1f("block_size", pixel_block_size);
            pixel_shader.setUniform1f("quality", 0.5f);
            pixel_shader_fbo.draw(0, 0);
        }
        pixel_shader.end();

        // Zeichne die roten Umrisse der Convex Hulls
        camera.begin();
        ofPushMatrix();
        {
            ofRotateXDeg(180);
            for (const auto &hull: convex_hulls) {
                ofPushStyle();
                ofSetColor(255, 0, 0);
                ofNoFill();
                ofBeginShape();
                for (const auto &point: hull) {
                    ofVertex(point);
                }
                ofEndShape(true);
                ofPopStyle();
            }
        }
        ofPopMatrix();
        camera.end();
    }
    frame_buffer.end();
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

void TrackingScene::draw_bounding_box() {
    const float offset_distance = 0.1f; // Offset in meters (10 cm)

    ofPushMatrix();
    {
        camera.begin();
        {
            ofPushMatrix();
            {
                ofRotateXDeg(180);

                const auto &body_skeletons = kinect_device->getBodySkeletons();

                for (const auto &skeleton: body_skeletons) {
                    // Initialize bounding box limits
                    ofVec3f min_bounds(FLT_MAX, FLT_MAX, FLT_MAX);
                    ofVec3f max_bounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

                    bool valid_skeleton = false;

                    // Iterate through all joints in the skeleton
                    for (const auto &joint: skeleton.joints) {
                        // Ensure joint position is valid
                        if (std::isfinite(joint.position.x) && std::isfinite(joint.position.y) &&
                            std::isfinite(joint.position.z)) {
                            valid_skeleton = true; // At least one valid joint found

                            // Update bounding box
                            min_bounds.x = std::min(min_bounds.x, joint.position.x);
                            min_bounds.y = std::min(min_bounds.y, joint.position.y);
                            min_bounds.z = std::min(min_bounds.z, joint.position.z);

                            max_bounds.x = std::max(max_bounds.x, joint.position.x);
                            max_bounds.y = std::max(max_bounds.y, joint.position.y);
                            max_bounds.z = std::max(max_bounds.z, joint.position.z);
                        }
                    }

                    // If at least one valid joint was found, draw the bounding box
                    if (valid_skeleton) {
                        ofPushStyle();
                        ofNoFill();
                        ofSetColor(255, 0, 0); // Red line
                        ofDrawBox((min_bounds + max_bounds) / 2, // Center of the box
                                  max_bounds.x - min_bounds.x, // Width
                                  max_bounds.y - min_bounds.y, // Height
                                  max_bounds.z - min_bounds.z); // Depth
                        ofPopStyle();
                    }
                }
            }
            ofPopMatrix();
        }
        camera.end();
    }
    ofPopMatrix();
}
