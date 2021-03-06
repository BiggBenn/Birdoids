#include "Bird.hpp"
#include "Game.hpp"


Bird::Bird() : GameObject()
{
	type = "Bird";
	size = rand() % 20 / 10;
	size++;
	size *= 3;

	Game::getInstance()->birdCounter++;
}


Bird::~Bird()
{
	//clear vectors
	neighbors.clear();
	obstacles.clear();

	//increase score and decrement counter of birds
	Game::getInstance()->score += 50;
	Game::getInstance()->birdCounter--;
}


//override of update function
void Bird::Update(float frameTime)
{
	//update the object
	GameObject::Update(frameTime);

	//first, find all nearby birds
	FindNearby(range);

	//set up target direction
	Vector2 direction = Vector2Zero();
	float accelWeight = 1;

	//random deviation
	Vector2 random = { rand() % 5 - 2, rand() % 5 - 2 };
	float randomWeight = 5;
	if (Vector2Length(getVelocity()) < 1)
		randomWeight = 15;
	direction = direction + random * randomWeight;

	//desire to keep flying where the bird is already flying
	Vector2 currentDirection = getVelocity();
	if (currentDirection.x != 0 || currentDirection.y != 0)
		Vector2Normalize(currentDirection);
	float continueWeight = 0.5;
	direction = direction + currentDirection * continueWeight;

	//then get their average direction/speed
	Vector2 speedDifference = AverageSpeeds() - getVelocity();
	if (speedDifference != Vector2Zero())
	{
		//normalize vector
		speedDifference = Vector2Normalize(speedDifference);
		//add this with weight to the direction
		float speedWeight = 5;
		direction = direction + speedDifference * speedWeight;
	}
	//find the position to cluster to
	Vector2 targetPosition = AverageNeighborPositions();
	if (targetPosition != Vector2Zero())
	{
		//normalize vector
		targetPosition = Vector2Normalize(targetPosition);
		//add this with weight to the direction
		float centerWeight = 2;
		direction = direction + targetPosition * centerWeight;
	}

	//find the vector to the nearest neighbor
	Vector2 nearestNeighbor = NearestNeighbor();
	if (nearestNeighbor != Vector2Zero())
	{
		float distance = Vector2Length(nearestNeighbor);
		//normalize vector
		nearestNeighbor = Vector2Normalize(nearestNeighbor);
		//add this with weight to the direction
		float avoidNeighborWeight = 4;
		//increase the acceleration and weight if the boids are too close together
		float minimumDistance = size * 8;
		if (distance < minimumDistance)
		{
			avoidNeighborWeight /= (distance/ minimumDistance);
			//accelWeight += 0.1 / (distance / minimumDistance);
		}
		direction = direction + nearestNeighbor * -avoidNeighborWeight;
	}
	

	//find the vector to nearest obstacle, if any
	Vector2 nearestObstacle = NearestObstacle();
	if (nearestObstacle != Vector2Zero())
	{
		float distance = Vector2Length(nearestObstacle);
		//normalize vector
		nearestObstacle = Vector2Normalize(nearestObstacle);
		//add this with weight to the direction
		float avoidObstacleWeight = 1;
		//increase weight and acceleration the closer the boid is to an obstacle
		avoidObstacleWeight *= (range * range/ (distance));
		//accelWeight += 0.1 * avoidObstacleWeight;
		direction = direction + nearestObstacle * -avoidObstacleWeight;
	}


	//accelerate in the direction of this weighted vector
	GameObject::AccelerateTowards(direction, accelWeight);

	//update rotation for rendering
	rotation = Vector2Angle(getVelocity(), Vector2Down());

	//check for collision with obstacles
	CheckForCollision();
}


void Bird::Render()
{
	DrawPolyLines(getPosition(), 3, size, rotation, GREEN);
	//DrawPolyLines(getPosition() + getVelocity(), 1, 1, 0, RED);		//velocity debug draw
	//DrawPolyLines(getPosition() + getAcceleration(), 1, 1, 0, BLUE);		//acceleration debug draw
}


void Bird::FindNearby(float range)
{
	//first, clear vector
	neighbors.clear();
	obstacles.clear();

	//get all nearby objects
	//because this list is sorted by distance, and we do not mess with the order, the neighbors and obstacles are sorted by distance too
	vector<DistObj> allNearby = Game::getInstance()->FindNearbyObjects(getPosition(), range);
	
	//sort the objects into two types, neighbors (birds) and obstacles (player, projectiles, boundaries)
	for (DistObj object : allNearby)
	{
		GameObject* obj = object.obj;
		if (obj == this)
		{
			//don't add to any list
		}
		else if (obj->type == this->type)
		{
			neighbors.push_back((Bird*)obj);
		}
		else
		{
			obstacles.push_back(obj);
		}
	}

	allNearby.clear();
}


Vector2 Bird::AverageSpeeds()
{
	//prepare result variable
	Vector2 result = { 0,0 };
	//add up all the velocities of neighbors into that result
	for (GameObject* neighbor : neighbors)
	{
		result = result + (neighbor->getVelocity() * neighbor->size);
	}

	//divide result by amount of values
	if(neighbors.size() > 0)
		result = (result / neighbors.size());

	//return the birds own velocity if nothing is found, as thaat ensures it doesnt want to speed up or slow down
	if (result == Vector2Zero())
		return getVelocity();
	else
		return result;
}


Vector2 Bird::NearestNeighbor()
{
	//because the list of neighbors is already sorted, we can assume the first neighbor is the closest
	if (neighbors.size() > 0)
		return neighbors[0]->getPosition() - getPosition();
	else
		return Vector2Zero();
}


Vector2 Bird::NearestObstacle()
{
	Vector2 result = Vector2Zero();
	//because the list of obstacles is already sorted, we can assume the first obstacle is the closest
	float obstacleDistance = 2 * range;
	//calculate distance of obstacle
	if (obstacles.size() > 0)
		obstacleDistance = Vector2Distance(obstacles[0]->getPosition(), getPosition());

	Game* game = Game::getInstance();

	//check for being close to boundary
	Vector2 pos = getPosition();
	//calculate distances to borders
	float leftDistance = pos.x - game->worldLeftBorder;
	float rightDistance = game->worldRightBorder - pos.x;
	float topDistance = pos.y - game->worldTopBorder;
	float bottomDistance = game->worldBottomBorder - pos.y;

	//check if the borders are within sensory range
	if (leftDistance < range)
	{
		//check if the left border is closer than the nearest obstacle
		if (leftDistance < obstacleDistance)
			result.x = -leftDistance;
	}
	else if (rightDistance < range)
	{
		//check if the left border is closer than the nearest obstacle
		if (rightDistance < obstacleDistance)
			result.x = rightDistance;
	}

	if (topDistance < range)
	{
		//check if the top border is closer than the nearest obstacle
		if (topDistance < obstacleDistance)
			result.y = -topDistance;
	}
	else if (bottomDistance < range)
	{
		//check if the bottom border is closer than the nearest obstacle
		if (bottomDistance < obstacleDistance)
			result.y = bottomDistance;
	}

	//only if the borders arent closer, return the nearest obstacles coordinates, otherwise return the border values
	if (result == Vector2Zero() && obstacles.size() > 0)
		result = obstacles[0]->getPosition() - getPosition();
	else result = Vector2Normalize(result);

	return result;
}


Vector2 Bird::AverageNeighborPositions()
{
	//prepare result variable
	Vector2 result = { 0,0 };
	//add up all the velocities of neighbors into that result
	for (GameObject* neighbor : neighbors)
	{
		result = result + neighbor->getPosition() - getPosition();
	}
	//divide result by amount of values
	if(neighbors.size() != 0)
		result = result / neighbors.size();
	return result;
}


void Bird::CheckForCollision()
{
	//since the obstacle list is sorted, and the nearest one is #0, and if any obstacle is close enough to collide #0 also is, we just check with #0
	if (obstacles.size() > 0)
	{
		GameObject* obj = obstacles[0];
		float distance = Vector2Distance(obj->getPosition(), getPosition());
		//if they overlap, and the obstacle hasn't raised its deletion flag yet (it does this after X number of hits, so a single bullet can't kill dozens of enemies)
		if (distance <= size + obj->size && !obj->deletionFlag)
		{
			deletionFlag = true;
			//inform obstacle that it hit something
			obstacles[0]->Collided();
			return;
		}
	}

	//check if bird is out of bounds
	Vector2 pos = getPosition();
	if (pos.x < Game::getInstance()->worldLeftBorder || pos.x > Game::getInstance()->worldRightBorder || pos.y < Game::getInstance()->worldTopBorder || pos.y > Game::getInstance()->worldBottomBorder)
	{
		//trigger deletion flag
		deletionFlag = true;
	}
}