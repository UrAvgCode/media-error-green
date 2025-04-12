#pragma once

#include <string>
#include <vector>

#include <ofEasyCam.h>
#include <ofImage.h>
#include <ofFbo.h>
#include <ofxAzureKinect.h>

#include "player.h"

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename, std::string logo_shader,
                    ofShader effect_shader);

    void draw() const;

    float width() const;
    float height() const;

    void update(std::vector<Player> &players, const ofEasyCam &camera);
    std::string get_fake_shader();

  protected:
    bool check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) const;
    void affect_player(Player &player, std::string shader) const;

    ofFbo fbo;
    ofImage image;

    std::string logo_shader = "No. 1";
    ofShader effect_shader;

    glm::vec2 position;
    glm::vec2 velocity;

    bool can_collide;
};
