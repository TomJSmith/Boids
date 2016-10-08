#include "Boid.h"

Boid::Boid()
{
	position = Vec3f(0.0, 0.0, 0.0);
	resetAc();
	model = Mat4f({ 1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f });
	velocity = Vec3f(0.0, 0.0, 0.0);
}

Vec3f Boid::pos()
{
	return position;
}

void Boid::pos(Vec3f aPos)
{
	position = aPos;
	updateModel();
}

Mat4f Boid::getModel()
{
	return model;
}

void Boid::resetAc()
{
	accumulator = Vec3f(0.0f, 0.0f, 0.0f);
}

void Boid::addForce(Vec3f aForce)
{
	accumulator = accumulator + aForce;
}

void Boid::move(float deltaT)
{
	if (accumulator.length() > maxAc)
	{
		accumulator.normalize();
		accumulator = accumulator * maxAc;
	}
	velocity = velocity + (accumulator * deltaT);
	if (velocity.length() > maxVel)
	{
		velocity.normalize();
		velocity = velocity * maxVel;
	}
	position = position + (velocity * deltaT);
	updateModel();
}

void Boid::updateModel()
{
	Vec3f x;
	Vec3f y;
	Vec3f z;
	Vec3f tvel = velocity;
	if (tvel.length() == 0.0)
	{
		x = Vec3f(1.0, 0.0, 0.0);
	}
	else
	{
		x = velocity.normalized();
	}

	Vec3f AP = accumulator + gravity;
	y = velocity * (AP.dotProduct(velocity)/velocity.dotProduct(velocity));
	y = AP - y;
	y.normalize();
	z = x.crossProduct(y);
	z.normalize();
	model = { x.x(), y.x(), z.x(), position.x(),
			x.y(), y.y(), z.y(), position.y(),
			x.z(), y.z(), z.z(), position.z(),
			0.0, 0.0, 0.0, 1.0 };
}

Vec3f Boid::vel()
{
	return velocity;
}
