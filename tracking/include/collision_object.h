#pragma once

#include <string>
#include <vector>

#include <ofEasyCam.h>
#include <ofImage.h>
#include <ofxAzureKinect.h>
#include <player.h>

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename);

    void draw() const;

    float width() const;
    float height() const;

    void update(std::map<std::uint32_t, Player> &players, const ofEasyCam &camera);

  protected:
    bool check_collision_with_bodies(std::map<std::uint32_t, Player> &players,
                                     const ofEasyCam &camera) const;

    ofImage image;

    glm::vec2 position;
    glm::vec2 velocity;

    bool can_collide;
};
