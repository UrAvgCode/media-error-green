#pragma once

#include <string>
#include <vector>

#include <ofEasyCam.h>
#include <ofImage.h>
#include <ofxAzureKinect.h>
#include <player.h>
#include <string>

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename, std::string logo_shader);

    void draw() const;

    float width() const;
    float height() const;

    void update(std::map<std::uint32_t, Player> &players, const ofEasyCam &camera);
    std::string get_fake_shader();

  protected:
    bool check_collision_with_bodies(std::map<std::uint32_t, Player> &players, const ofEasyCam &camera) const;
    void affect_player(Player &player, std::string shader) const;

    ofImage image;

    std::string logo_shader = "No. 1";

    glm::vec2 position;
    glm::vec2 velocity;

    bool can_collide;
};
