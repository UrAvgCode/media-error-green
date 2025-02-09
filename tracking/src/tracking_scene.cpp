#include "tracking_scene.h"
#include "ofxConvexHull.h"

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

//--------------------------------------------------------------
void TrackingScene::update() {}

void TrackingScene::render() {
    frame_buffer.begin();
    {
        ofClear(0);
        ofBackground(0);

        camera.begin();
        {
            ofPushMatrix();
            {
                ofRotateXDeg(180);

                ofEnableDepthTest();

                const auto &body_skeletons = kinect_device->getBodySkeletons();

                const std::size_t k_max_bodies = 6;
                auto body_ids = std::vector<int>(k_max_bodies, 0);
                for (std::size_t i = 0; i < std::min(body_skeletons.size(), k_max_bodies); ++i) {
                    body_ids[i] = static_cast<int>(body_skeletons[i].id);
                }

                render_shader.begin();
                {
                    const auto frame_width = static_cast<int>(kinect_device->getDepthTex().getWidth());
                    const auto frame_height = static_cast<int>(kinect_device->getDepthTex().getHeight());

                    render_shader.setUniformTexture("uDepthTex", kinect_device->getDepthTex(), 1);
                    render_shader.setUniformTexture("uBodyIndexTex", kinect_device->getBodyIndexTex(), 2);
                    render_shader.setUniformTexture("uWorldTex", kinect_device->getDepthToWorldTex(), 3);
                    render_shader.setUniform2i("uFrameSize", frame_width, frame_height);
                    render_shader.setUniform1iv("uBodyIDs", body_ids.data(), k_max_bodies);

                    render_shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
                    render_shader.setUniform1f("random_offset_one", distribution(generator));
                    render_shader.setUniform1f("random_offset_two", distribution(generator));

                    const int num_points = frame_width * frame_height;
                    points_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
                }
                render_shader.end();

                draw_body_outline_2D(kinect_device->getBodySkeletons(), camera);

                ofDisableDepthTest();
            }
            ofPopMatrix();
        }
        camera.end();
    }
    frame_buffer.end();
}

void TrackingScene::draw_body_outline_2D(const std::vector<ofxAzureKinect::BodySkeleton> &body_skeletons,
                                         const ofCamera &camera) {
    ofxConvexHull convex_hull_calculator; // Instantiate the convex hull object
    const float offset_distance = 0.1f; // Offset in meters (10 cm)

    for (const auto &skeleton: body_skeletons) {
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
                expanded_hull.emplace_back(current.x + normal.x * offset_distance,
                                           current.y + normal.y * offset_distance);
            }

            // Draw the expanded hull at the front depth
            ofPushStyle();
            ofSetColor(255, 0, 0); // Red outline
            ofNoFill();
            ofBeginShape();
            for (const auto &point: expanded_hull) {
                // Project the 2D hull points back into 3D using the front depth
                ofVec3f world_point = camera.screenToWorld(
                        ofVec3f(point.x, point.y, camera.worldToScreen(ofVec3f(0, 0, front_depth)).z));
                ofVertex(world_point.x, world_point.y, world_point.z);
            }
            ofEndShape(true); // Close the shape
            ofPopStyle();
        }
    }
}


std::vector<ofVec2f> TrackingScene::calculate_convex_hull(const std::vector<ofVec2f> &points) {
    // Ensure we have enough points to compute a hull
    if (points.size() < 3) {
        return points; // Convex hull is trivial for fewer than 3 points
    }

    // Find the point with the lowest Y-coordinate (and leftmost in case of a tie)
    auto min_point = *std::min_element(points.begin(), points.end(), [](const ofVec2f &a, const ofVec2f &b) {
        return (a.y < b.y) || (a.y == b.y && a.x < b.x);
    });

    // Sort points by polar angle with respect to `min_point`
    std::vector<ofVec2f> sorted_points = points;
    std::sort(sorted_points.begin(), sorted_points.end(), [&min_point](const ofVec2f &a, const ofVec2f &b) {
        ofVec2f vec_a = a - min_point;
        ofVec2f vec_b = b - min_point;
        float cross = vec_a.x * vec_b.y - vec_a.y * vec_b.x; // Cross product
        if (cross == 0) { // Collinear points: closer points come first
            return vec_a.lengthSquared() < vec_b.lengthSquared();
        }
        return cross > 0; // Clockwise order
    });

    // Construct the convex hull using a stack
    std::vector<ofVec2f> hull;
    hull.push_back(sorted_points[0]); // Push first point
    hull.push_back(sorted_points[1]); // Push second point

    for (size_t i = 2; i < sorted_points.size(); ++i) {
        while (hull.size() > 1) {
            const ofVec2f &top = hull.back();
            const ofVec2f &next_to_top = hull[hull.size() - 2];

            // Check for a right turn (non-left turn) using the cross product
            ofVec2f vec1 = top - next_to_top;
            ofVec2f vec2 = sorted_points[i] - top;
            if ((vec1.x * vec2.y - vec1.y * vec2.x) <= 0) { // Right turn or collinear
                hull.pop_back(); // Remove the top point
            } else {
                break; // Left turn
            }
        }
        hull.push_back(sorted_points[i]); // Push the current point
    }

    return hull;
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
