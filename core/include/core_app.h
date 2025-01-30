#pragma once

#include <intro_scene.h>
#include <scene.h>
#include <tracking_scene.h>

#include <chrono>
#include <random>

#include <ofMain.h>

class CoreApp : public ofBaseApp {
  public:
    CoreApp();

    void setup() override;
    void exit() override;

    void update() override;
    void draw() override;

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;

  private:
    static void draw_fps_counter();

    ofxAzureKinect::Device kinect_device;

    IntroScene intro_scene;
    TrackingScene tracking_scene;
    Scene *current_scene;
    Scene *inactive_scene;

    ofFbo current_app_fbo;
    ofFbo inactive_app_fbo;
    ofShader transition_shader;

    const std::chrono::milliseconds transition_duration = 500ms;
    std::chrono::steady_clock::time_point transition_start_time;

    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;
};
