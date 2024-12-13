#pragma once
#include "ofMain.h"
#include <ofMath.h>
#include <ofVec2f.h>
#include <ofMesh.h>

class Particle {
public:
	const float max_speed = 4;
	const int max_trail_length = 50;

	ofVec2f position;
	ofVec2f velocity;
	ofVec2f acceleration;

	Particle(float x, float y);

	void update();
	void draw();

	void apply_force(ofVec2f force);

	bool is_outside_of_screen() const;

protected:
	ofMesh mesh;

};
