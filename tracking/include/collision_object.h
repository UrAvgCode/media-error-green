#pragma once

#include <string>
#include <vector>

#include <ofEasyCam.h>
#include <ofImage.h>
#include <ofxAzureKinect.h>

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename);

    void draw() const;

    float width() const;
    float height() const;

    void update(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons, const ofEasyCam &camera);

  protected:
    bool check_collision_with_bodies(const std::vector<ofxAzureKinect::BodySkeleton> &skeletons,
                                     const ofEasyCam &camera) const;

    ofImage image;

    glm::vec2 position;
    glm::vec2 velocity;

    bool can_collide;
};
