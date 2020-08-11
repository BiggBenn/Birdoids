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
	bulletCooldown -= frameTime;
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

	//F1 button for help screen
	if (IsKeyPressed(KEY_F1))
	{
		//switch help flag
		Game::Instance->helpFlag = !Game::Instance->helpFlag;
	}
	
}


void Player::CheckForOutOfBounds()
{
	Vector2 pos = getPosition();

	//too far left
	if (pos.x < Game::Instance->worldLeftBorder)
	{
		pos.x += Game::Instance->worldWidth;
	}
	//too far right
	else if (pos.x > Game::Instance->worldRightBorder) 
	{
		pos.x -= Game::Instance->worldWidth;
	}

	//too far up
	if (pos.y < Game::Instance->worldTopBorder)
	{
		pos.y += Game::Instance->worldHeight;
	}
	//too far down
	else if (pos.y > Game::Instance->worldBottomBorder)
	{
		pos.y -= Game::Instance->worldHeight;
	}

	//apply these changes
	setPosition(pos);
}


void Player::FireBullet(Vector2 direction)
{
	//check for cooldown timer
	if (bulletCooldown <= 0)
	{
		//create projectile with proper trajectory and parameters
		Vector2 fireVector = Vector2Normalize(direction);
		fireVector = fireVector * bulletSpeed;
		Projectile* bullet = new Projectile(bulletLifeTime, fireVector, bulletHealth, bulletSize);
		bullet->setPosition(position);
		Game::Instance->AddObject(bullet, true);
		//reset cooldown
		bulletCooldown = 1 / bulletFireRate;
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
		Game::Instance->AddObject(mine, true);
		//reset cooldown
		mineCooldown = 1 / mineFireRate;
	}
}