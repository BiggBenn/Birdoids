#pragma once
#include <string>
#include "raylib.h"
#include "raymath.h"

class GameObject
{
	//
	// Public Variables
	//
public:

	//the drag is multiplied by this, if its lower then drag will be reduced
	const float DRAG_FACTOR = 0.0003;

	//rotation of object
	float rotation = 0;

	//deletionFlag is true if the object wants to be deleted
	bool deletionFlag = false;

	//type says what kind of object this is
	std::string type;

	//whether or not drag applies to this object
	bool drag = true;

	//size is used for collision logic mostly
	float size = 1;

	//
	// Protected Variables
	//
protected:

	//position in the world, x and y coordinates
	Vector2 position = { 0,0 };

	//velocity in x and y direction
	Vector2 velocity = { 0,0 };

	//acceleration in x and y, used for steering in the end
	Vector2 acceleration = { 0,0 };

	//the maximum acceleration this object is capable of, in pixels/second
	float maxAcceleration = 160;

	//previous position, saved for quadrant change calculations
	Vector2 prevPosition = { 0,0 };

	//
	// Public Functions
	//
public:

	//constructor
	GameObject();

	//destructor
	virtual ~GameObject();

	//update handles any and all real logic 
	virtual void Update(float frameTime);

	//render handles the drawing of the object to the screen
	virtual void Render();

	//Getters and setters for various values 
	Vector2 getPosition();
	void setPosition(Vector2 pos);		//ONLY USE THIS FOR INITIAL SETUP
	Vector2 getVelocity();
	Vector2 getAcceleration();

	//callback to let obj know it was collided with by a bird, purely virtual
	virtual void Collided();

	//
	// Protected Functions
	//
protected:

	//Accelerate Game Object towards the direction, with accelerationPercentage percent of maximum acceleration
	void AccelerateTowards(Vector2 direction, float accelerationPercentage);

	
	

	//
	// Private Functions
	//
private:

	void Init();
	
};

