#include "collision_object.h"

#include <memory>
#include <string>
#include <vector>

#include "ofAppRunner.h"
#include "ofFbo.h"


CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", std::make_shared<EffectShader>()) {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::shared_ptr<EffectShader> effect_shader) :
    position(position), velocity(velocity), can_collide(false), effect_shader(effect_shader) {
    image.load(filename);
}

void CollisionObject::update(std::vector<Player> &players, const ofEasyCam &camera) {
    if (position.x <= 0 || position.x + width() >= ofGetWidth()) {
        velocity.x *= -1;
    }

    if (position.y <= 0 || position.y + height() >= ofGetHeight()) {
        velocity.y *= -1;
    }

    if (can_collide && check_collision_with_bodies(players, camera)) {
        velocity *= -1;
        can_collide = false;
    } else {
        can_collide = true;
    }

    position += velocity;
}

void CollisionObject::draw() const {
    effect_shader->begin();
    image.draw(position.x, position.y, 0);
    effect_shader->end();
}

bool CollisionObject::check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) const {
    for (auto &player: players) {
        // Zugriff auf die Skeleton-Vertices des Spielers
        const auto &vertices = player.get_skeleton_vertices();

        ofRectangle bounding_box(position.x, position.y, width(), height());
        for (const auto &vertex: vertices) {
            // Prüfe, ob der Vertex das Objekt berührt
            if (bounding_box.intersects(vertex[0], vertex[1])) {
                
                player.set_shader(effect_shader);

                return true;
            }
        }
    }

    return false;
}

float CollisionObject::width() const { return image.getWidth(); }

float CollisionObject::height() const { return image.getHeight(); }
