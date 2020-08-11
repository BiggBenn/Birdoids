#include "Projectile.hpp"
#include "Game.hpp"



Projectile::Projectile(float maxLife, Vector2 speed, int HP, float projectile_size) : GameObject()
{
	type = "Projectile";
	maxLifeTime = maxLife;
	constVelocity = speed;
	drag = false;
	size = projectile_size;
	healthPoints = HP;
}


Projectile::~Projectile()
{

}


void Projectile::Update(float frameTime)
{
	velocity = constVelocity;
	GameObject::Update(frameTime);;
	lifeTime += frameTime;
	if (maxLifeTime != 0 && lifeTime > maxLifeTime)
		deletionFlag = true;

	//too far left
	if (position.x < Game::Instance->worldLeftBorder - 50)
	{
		deletionFlag = true;
	}
	//too far right
	else if (position.x > Game::Instance->worldRightBorder + 50)
	{
		deletionFlag = true;
	}

	//too far up
	if (position.y < Game::Instance->worldTopBorder - 50)
	{
		deletionFlag = true;
	}
	//too far down
	else if (position.y > Game::Instance->worldBottomBorder + 50)
	{
		deletionFlag = true;
	}
}


void Projectile::Render()
{
	DrawCircle(position.x, position.y, size, RED);
}


void Projectile::Collided()
{
	//setup in this way specifically so that -1 healthpoints would be infinite. Not currently used that way.
	if (healthPoints > 0)
	{
		healthPoints--;

		if (healthPoints == 0)
		{
			//remove this thing then
			deletionFlag = true;
		}
	}
}