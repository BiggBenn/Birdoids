#include "Player.hpp"
#include "Game.hpp"


Player::Player() : GameObject()
{
	type = "Player";
	size = 15;
}


Player::~Player()
{

}


//override of update function
void Player::Update(float frameTime)
{
	//update game object base
	GameObject::Update(frameTime);

	//wrap player around the screen
	CheckForOutOfBounds();

	//take care of inputs
	HandleInput(frameTime);

	//drop mine trail
	if (autoMine)
		DropMine();

	//reduce cooldowns
	fireCooldown -= frameTime;
	mineCooldown -= frameTime;
	//update rotation
	rotation = fmodf(rotation, 360);
}


//render override
void Player::Render()
{
	Vector2 forward = CalculateForwardDirection();
	DrawPoly(getPosition(), 4, 10, rotation + 45, DARKBLUE);	//draw central square
	DrawPoly(getPosition() - forward * 13, 4, 10, rotation + 45, DARKBLUE);		//draw rear square
	DrawPoly(getPosition() + forward * 10, 3, 8, rotation, DARKBLUE);		//draw top triangle
}


Vector2 Player::CalculateForwardDirection()
{
	Vector2 result;
	result = Vector2Rotate(Vector2Down(), rotation);
	return result;
}


void Player::HandleInput(float frameTime)
{
	//prep for acceleration and stuff
	Vector2 forward = CalculateForwardDirection();

	//Up arrow key
	if (IsKeyDown(KEY_UP))
	{
		//move forwards
		AccelerateTowards(forward, 0.5);
	}
	//down arrow key
	else if (IsKeyDown(KEY_DOWN))
	{
		//move backwards
		AccelerateTowards(-forward, 0.5);
	}

	//left arrow key
	if (IsKeyDown(KEY_LEFT))
	{
		//rotate counter-clockwise
		rotation -= maxRotationalVelocity * frameTime;
	}
	//right arrow key
	if (IsKeyDown(KEY_RIGHT))
	{
		//rotate clockwise
		rotation += maxRotationalVelocity * frameTime;
	}


	//spacebar
	if (IsKeyDown(KEY_SPACE))
	{
		//shoot a projectile
		FireBullet(forward);
	}

	//Alt, to drop mines
	if (IsKeyPressed(KEY_LEFT_ALT) || IsKeyPressed(KEY_RIGHT_ALT))
	{
		//toggle the auto-minelaying
		autoMine = !autoMine;
	}
	
	//control button
	if (IsKeyPressed(KEY_LEFT_CONTROL) || IsKeyPressed(KEY_RIGHT_CONTROL))
	{
		//switch firemode
		rapidfire = !rapidfire;
	}
}


void Player::CheckForOutOfBounds()
{
	Vector2 pos = getPosition();
	Game* game = Game::getInstance();

	//too far left
	if (pos.x < game->worldLeftBorder)
	{
		pos.x += game->worldWidth;
	}
	//too far right
	else if (pos.x > game->worldRightBorder)
	{
		pos.x -= game->worldWidth;
	}

	//too far up
	if (pos.y < game->worldTopBorder)
	{
		pos.y += game->worldHeight;
	}
	//too far down
	else if (pos.y > game->worldBottomBorder)
	{
		pos.y -= game->worldHeight;
	}

	//apply these changes
	setPosition(pos);
}


void Player::FireBullet(Vector2 direction)
{
	//check for cooldown timer
	if (fireCooldown <= 0)
	{
		//prep variables
		float speed, lifeTime, projectileSize, fireRate;
		int health;
		if (rapidfire)
		{
			speed = rapidFireSpeed;
			lifeTime = rapidFireLifetime;
			projectileSize = rapidFireSize;
			fireRate = rapidFireFireRate;
			health = rapidFireHealth;
		}
		else
		{
			speed = bulletSpeed;
			lifeTime = bulletLifeTime;
			projectileSize = bulletSize;
			fireRate = bulletFireRate;
			health = bulletHealth;
		}
	
		//create projectile with proper trajectory and parameters
		Vector2 fireVector = Vector2Normalize(direction);
		fireVector = fireVector * speed;
		Projectile* bullet = new Projectile(lifeTime, fireVector, health, projectileSize);
		bullet->setPosition(position);
		Game::getInstance()->AddObject(bullet, true);
		//reset cooldown
		fireCooldown = 1 / fireRate;
	}
}


void Player::DropMine()
{
	//check for cooldown timer
	if (mineCooldown <= 0)
	{
		//drop mine at player position with 0 speed (Yes it is just a projectile, the logic works for both)
		Projectile* mine = new Projectile(mineLifeTime, Vector2Zero(), mineHealth, mineSize);
		mine->setPosition(position);
		Game::getInstance()->AddObject(mine, true);
		//reset cooldown
		mineCooldown = 1 / mineFireRate;
	}
}