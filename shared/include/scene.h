#pragma once

#include <ofAppRunner.h>
#include <ofFbo.h>
#include <ofImage.h>
#include <ofPixels.h>

class Scene {
  public:
    Scene() { frame_buffer.allocate(ofGetWidth(), ofGetHeight(), GL_RGB); }

    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void render() = 0;

    virtual void draw() {
        render();
        frame_buffer.draw(0, 0);
    }

    virtual ofFbo &get_frame_buffer() { return frame_buffer; }

    void save_frame() {
        auto pixels = ofPixels();
        auto image = ofImage();

        frame_buffer.readToPixels(pixels);

        image.setFromPixels(pixels);
        image.save("frame_" + std::to_string(ofGetElapsedTimeMillis()) + ".png");
    }

  protected:
    ofFbo frame_buffer;
};
