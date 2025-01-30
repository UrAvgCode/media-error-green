#pragma once

#include <ofMain.h>

class Scene {
  public:
    Scene() { frame_buffer.allocate(ofGetWidth(), ofGetHeight(), GL_RGB); }

    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void render() = 0;

    virtual void draw() {
        render();
        frame_buffer.draw(0, 0);
    };

    virtual ofFbo &get_frame_buffer() { return frame_buffer; };

  protected:
    ofFbo frame_buffer;
};
