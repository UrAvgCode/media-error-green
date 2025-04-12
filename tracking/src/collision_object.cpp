﻿#include "collision_object.h"

#include <memory>
#include <string>
#include <vector>

#include <ofAppRunner.h>
#include <ofFbo.h>
#include <ofMath.h>

CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", std::make_shared<EffectShader>()) {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::shared_ptr<EffectShader> effect_shader) :
    position(position), velocity(velocity), can_collide(false), effect_shader(effect_shader) {
    image.load(filename);

    pluck_b.load("resources/audio/gruen_pluck_b.wav");
    pluck_d.load("resources/audio/gruen_pluck_d.wav");
    pluck_e.load("resources/audio/gruen_pluck_e.wav");
    pluck_g.load("resources/audio/gruen_pluck_g.wav");
    globalEffect.load("resources/audio/gruen_globalEffect.wav");

    pluck_b.setMultiPlay(true);
    pluck_d.setMultiPlay(true);
    pluck_e.setMultiPlay(true);
    pluck_g.setMultiPlay(true);
    globalEffect.setMultiPlay(false);
}

void CollisionObject::update(std::vector<Player> &players, const ofEasyCam &camera) {
    if (position.x <= 0 || position.x + width() >= ofGetWidth()) {
        play_random_pluck();
        velocity.x *= -1;
    }

    if (position.y <= 0 || position.y + height() >= ofGetHeight()) {
        play_random_pluck();
        velocity.y *= -1;
    }

    if (check_collision_with_bodies(players, camera)) {
        if (can_collide) {
            velocity *= -1;
            can_collide = false;
        }
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

void CollisionObject::play_random_pluck() {
    int random = std::round(ofRandom(0, 4));
    switch (random) {
        case 0:
            pluck_b.play();
            break;
        case 1:
            pluck_d.play();
            break;
        case 2:
            pluck_e.play();
            break;
        case 3:
            pluck_g.play();
            break;
        default:
            break;
    }
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
