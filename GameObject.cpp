#include "GameObject.hpp"
#include "Game.hpp"

//constructor
GameObject::GameObject()
{
	type = "GameObject";
	Game::Instance->AddToQuadrantSystem(this);
}


//destructor
GameObject::~GameObject()
{

}


//Update function handles all kinds of logic
void GameObject::Update(float frameTime)
{
	//accelerate the object
	velocity = velocity + (acceleration * frameTime);
	acceleration = Vector2Zero();

	if (drag)
	{
		//apply drag to keep velocity in check
		float slowdown = Vector2LengthSquared(velocity) * DRAG_FACTOR * frameTime;
		velocity = velocity + (velocity * -slowdown);
	}

	//save previous position for check of quadrant change
	prevPosition = position;

	//apply velocity as location change
	position = position + (velocity * frameTime);

	if (velocity != Vector2Zero())
	{
		Game::Instance->OnObjectMoved(this, prevPosition);
	}
}

//render function takes care of drawing to screen
void GameObject::Render()
{
	DrawPolyLines(position, 4, 2, 0, RAYWHITE);
}


void GameObject::AccelerateTowards(Vector2 direction, float accelerationPercentage)
{
	//sanitize acceleration Percentage input to 0-100%
	if (accelerationPercentage < 0)
		accelerationPercentage = 0;
	else if (accelerationPercentage > 1.00)
		accelerationPercentage = 1.00;

	//ensure that direction is not nullvector
	if (direction == Vector2Zero())
		return;

	//normalize vector
	Vector2 dir = Vector2Normalize(direction);
	
	//add this acceleration to the objects
	acceleration = acceleration + (dir * accelerationPercentage * maxAcceleration);
}


Vector2 GameObject::getPosition()
{
	return position;
}


void GameObject::setPosition(Vector2 pos)
{
	prevPosition = position;
	position = pos;
	Game::Instance->OnObjectMoved(this, prevPosition);
}


Vector2 GameObject::getVelocity()
{
	return velocity;
}


Vector2 GameObject::getAcceleration()
{
	return acceleration;
}


void GameObject::Collided()
{
	//nothing
}