#include "tracking_app.h"

#include <algorithm>
#include <vector>

//--------------------------------------------------------------
void TrackingApp::setup() {
    // ofSetLogLevel(OF_LOG_VERBOSE);

    ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

    if (kinect_device.open()) {
        auto device_settings = ofxAzureKinect::DeviceSettings();
        device_settings.syncImages = true;
        device_settings.depthMode = K4A_DEPTH_MODE_NFOV_UNBINNED;
        device_settings.updateIr = false;
        device_settings.updateColor = true;
        device_settings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
        device_settings.updateWorld = true;
        device_settings.updateVbo = false;
        kinect_device.startCameras(device_settings);

        auto body_tracker_settings = ofxAzureKinect::BodyTrackerSettings();
        body_tracker_settings.sensorOrientation = K4ABT_SENSOR_ORIENTATION_DEFAULT;
        body_tracker_settings.processingMode = K4ABT_TRACKER_PROCESSING_MODE_CPU;
        kinect_device.startBodyTracker(body_tracker_settings);
    }

    // Load shaders.
    auto shader_settings = ofShaderSettings();
    shader_settings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render.vert";
    shader_settings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render.frag";
    shader_settings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
    shader_settings.bindDefaults = true;
    if (shader.setup(shader_settings)) {
        ofLogNotice(__FUNCTION__) << "Success loading shader!";
    }

    chromatic_shader.load("shaders/chromatic");

    // Setup vbo.
    std::vector<glm::vec3> verts(1);
    points_vbo.setVertexData(verts.data(), static_cast<int>(verts.size()), GL_STATIC_DRAW);

    // Setup fbo
    fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void TrackingApp::exit() { kinect_device.close(); }

//--------------------------------------------------------------
void TrackingApp::update() {}

//--------------------------------------------------------------
void TrackingApp::draw() {
    fbo.begin();
    {
        ofClear(0);
        ofBackground(0);

        camera.begin();
        {
            ofPushMatrix();
            {
                ofRotateXDeg(180);

                ofEnableDepthTest();

                const auto &body_skeletons = kinect_device.getBodySkeletons();

                const std::size_t k_max_bodies = 6;
                auto body_ids = std::vector<int>(k_max_bodies, 0);
                for (std::size_t i = 0; i < std::min(body_skeletons.size(), k_max_bodies); ++i) {
                    body_ids[i] = static_cast<int>(body_skeletons[i].id);
                }

                shader.begin();
                {
                    const auto frame_width = static_cast<int>(kinect_device.getDepthTex().getWidth());
                    const auto frame_height = static_cast<int>(kinect_device.getDepthTex().getWidth());

                    shader.setUniformTexture("uDepthTex", kinect_device.getDepthTex(), 1);
                    shader.setUniformTexture("uBodyIndexTex", kinect_device.getBodyIndexTex(), 2);
                    shader.setUniformTexture("uWorldTex", kinect_device.getDepthToWorldTex(), 3);
                    shader.setUniform2i("uFrameSize", frame_width, frame_height);
                    shader.setUniform1iv("uBodyIDs", body_ids.data(), k_max_bodies);

                    const int num_points = frame_width * frame_height;
                    points_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
                }
                shader.end();

                ofDisableDepthTest();

                // draw_skeleton(body_skeletons);
            }
            ofPopMatrix();
        }
        camera.end();
    }
    fbo.end();

    ofPushMatrix();
    {
        ofTranslate(fbo.getWidth(), 0);
        ofScale(-1, 1);

        chromatic_shader.begin();
        {
            const float aberration = 20;
            chromatic_shader.setUniform1f("aberration_amount", aberration);
            chromatic_shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
            chromatic_shader.setUniform1f("rand1", static_cast<float>(rand() % static_cast<int>(fbo.getHeight())));
            chromatic_shader.setUniform1f("rand2", static_cast<float>(rand() % static_cast<int>(fbo.getHeight())));

            fbo.draw(0, 0);
        }
        chromatic_shader.end();
    }
    ofPopMatrix();
}

void TrackingApp::draw_skeleton(const std::vector<ofxAzureKinect::BodySkeleton> &body_skeletons) {
    for (const auto &skeleton: body_skeletons) {
        // Draw joints.
        for (const auto &joint: skeleton.joints) {
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

//--------------------------------------------------------------
void TrackingApp::keyPressed(int key) {}

//--------------------------------------------------------------
void TrackingApp::keyReleased(int key) {}

//--------------------------------------------------------------
void TrackingApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::mouseDragged(int x, int y, int button) {
    if (button == 1) {
        kinect_device.getBodyTracker().jointSmoothing = ofMap(x, 0, ofGetWidth(), 0.0f, 1.0f, true);
    }
}

//--------------------------------------------------------------
void TrackingApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void TrackingApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void TrackingApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void TrackingApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void TrackingApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void TrackingApp::dragEvent(ofDragInfo dragInfo) {}

ofxAzureKinect::Device *TrackingApp::get_kinect_device() { return &kinect_device; }
