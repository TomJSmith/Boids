#ifndef BOID_H
#define BOID_H

#include "Vec3f.h"
#include "Mat4f.h"

class Boid
{
private:
	Vec3f position;
	Vec3f accumulator;
	Vec3f velocity;
	Mat4f model;
	float maxAc = 0.01;
	float maxVel = 0.20;
	Vec3f gravity = Vec3f(0.0, 1.0, 0.0);

public:
	Boid();
	Vec3f pos();
	void pos(Vec3f aPos);
	void resetAc();
	void addForce(Vec3f aForce);
	Mat4f getModel();
	void move(float deltaT);
	Vec3f vel();
	void updateModel();
};

#endif //BOID_H
