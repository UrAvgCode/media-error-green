#pragma once

#include <string>
#include <utility>
#include <vector>

#include <ofMain.h>
#include <ofxSvg.h>

#include <scene.h>

#include "particle.h"

class IntroScene : public Scene {
  public:
    IntroScene();

    void update() override;
    void render() override;

    void reset_particles();

  private:
    void create_logo_vectors();
    void create_logo_in_outs_vectors();

    // flow field
    std::vector<ofVec2f> flow_field;
    int flow_field_resolution;
    int flow_field_cols;
    int flow_field_rows;
    float flow_field_offset;

    // particles
    std::array<Particle, 2048> particles;
    const float repulsion_radius = 25;
    const float repulsion_strength = 5;

    const float attraction_radius = 30;
    const float attraction_strength = 1000;

    float logo_tolerance = 10.0f;

    // logo
    ofxSVG logo_svg;
    ofxSVG logo_in_outs_svg;

    std::vector<pair<ofVec2f, ofVec2f>> logo_vectors;
    std::vector<pair<ofVec2f, ofVec2f>> logo_in_outs_vectors;
    std::vector<std::pair<ofVec2f, ofVec2f>> all_logo_vectors;

    ofxSVG logo_picture;
    ofFbo logo_fbo;

    ofVec2f logo_position;
    ofVec2f logo_center;
    float logo_radius;
    int logo_margin;

    ofFbo particle_draw_fbo;

    ofShader particle_trail_shader;
    ofShader particle_pixel_shader;

    int line_position = 0;
};
