#pragma once

#include "GameObject.hpp"
#include "Projectile.hpp"

class Player : public GameObject
{
	//
	// Public Variables
	//
public:



	//
	// Private Variables
	//
private:

	//speed at which rotation will change per second
	float maxRotationalVelocity = 90;

	//rate per second at which bullets can be fired
	float bulletFireRate = 0.3;

	//how fast bullets go
	float bulletSpeed = 101;

	//how long bullets last
	float bulletLifeTime = 30;

	//how many birds a bullet could maximally kill
	int bulletHealth = 4;

	//cooldown for firing, starts at interval and goes down
	float bulletCooldown = 0;

	//size of bullet
	float bulletSize = 18;

	//Mine laying rate per second
	float mineFireRate = 4;

	//mine lifetime
	float mineLifeTime = 120;

	//mine cooldown
	float mineCooldown = 0;

	//Maximal kill amount of mines, -1 for infinite
	int mineHealth = 5;

	//mine size
	float mineSize = 5;

	//flag whether minelaying is active
	bool autoMine = true;

	//
	// Public Functions
	//
public:

	Player();

	~Player();

	//override of update
	void Update(float frameTime);

	//render override
	void Render();

	//
	// Protected Functions
	//
protected:

	//Calculate the direction vector the player is pointing
	Vector2 CalculateForwardDirection();

	//Input handling function
	void HandleInput(float frameTime);

	//Checks whether the player is out of bounds, and wraps them around instead
	void CheckForOutOfBounds();

	//fire a bullet
	void FireBullet(Vector2 direction);

	//lay a mine
	void DropMine();


	//
	// Private Functions
	//
private:
};

