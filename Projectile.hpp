#pragma once
#include "GameObject.hpp"

class Projectile : public GameObject
{
private:
	//Lifetime of the projectile
	float lifeTime = 0;

	//maximum life of projectile
	float maxLifeTime;

	//Health points of projectile, i.e. how many collisions it can take before being destroyed
	int healthPoints;


public:

	//constructor
	Projectile(float maxLife, Vector2 speed, int HP, float projectile_size);

	//destructor
	~Projectile();

	//override of update
	void Update(float frameTime);

	//render override
	void Render();

	//collided override
	void Collided();
};

