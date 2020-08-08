#pragma once

#include <vector>
#include "GameObject.hpp"

using namespace std;

class Bird : public GameObject
{
	//
	// Public Variables
	//
public:


	//
	// Private Variables
	//
private:

	//nearby other birds
	vector<Bird*> neighbors;

	//nearby obstacles
	vector<GameObject*> obstacles;

	//range of the birds sensory perception
	float range = 100;

	//
	// Public Functions
	//
public:

	//constructor
	Bird();

	//destructor
	~Bird();

	//override update function
	void Update(float frameTime);

	//override render function
	void Render();
	

	//
	// Private Functions
	//
private:

	//Finds all nearby neighbors
	void FindNearby(float range);

	//Averages the speeds/directions of neighbors
	Vector2 AverageSpeeds();

	//Finds the nearest neighbor and the vector pointing to it
	Vector2 NearestNeighbor();

	//Finds nearest obstacle and the vector pointing to it
	Vector2 NearestObstacle();

	//Averages all the positions of the neighbors to find the center of the "flock" to adhere to
	Vector2 AverageNeighborPositions();

	//Checks all obstacles nearby to see if any are closer than size, which counts as a collision and causes deletion
	void CheckForCollision();
};

