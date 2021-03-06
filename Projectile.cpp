#include "Projectile.hpp"
#include "Game.hpp"



Projectile::Projectile(float maxLife, Vector2 speed, int HP, float projectile_size) : GameObject()
{
	type = "Projectile";
	maxLifeTime = maxLife;
	velocity = speed;
	//disabling drag ensures that velocity doesnt decrease over time
	drag = false;
	size = projectile_size;
	healthPoints = HP;
}


Projectile::~Projectile()
{

}


void Projectile::Update(float frameTime)
{
	//update base
	GameObject::Update(frameTime);;
	
	//increment lifetime by frametime, and check for being over the limit
	lifeTime += frameTime;
	if (maxLifeTime != 0 && lifeTime > maxLifeTime)
		deletionFlag = true;

	Game* game = Game::getInstance();

	//too far left
	if (position.x < game->worldLeftBorder - 50)
	{
		deletionFlag = true;
	}
	//too far right
	else if (position.x > game->worldRightBorder + 50)
	{
		deletionFlag = true;
	}
	//too far up
	else if (position.y < game->worldTopBorder - 50)
	{
		deletionFlag = true;
	}
	//too far down
	else if (position.y > game->worldBottomBorder + 50)
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
		//decrement health
		healthPoints--;

		if (healthPoints == 0)
		{
			//remove this thing then
			deletionFlag = true;
		}
	}
}