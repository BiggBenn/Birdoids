#include "Bird.hpp"
#include "Game.hpp"


Bird::Bird() : GameObject()
{
	type = "Bird";
	size = rand() % 20 / 10;
	size++;
	size *= 3;

	Game::Instance->birdCounter++;
}


Bird::~Bird()
{
	neighbors.clear();
	obstacles.clear();

	Game::Instance->score += 50;
	Game::Instance->birdCounter--;
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
	direction = Vector2Add(direction, Vector2Scale(random, randomWeight));

	//desire to keep flying where the bird is already flying
	Vector2 currentDirection = getVelocity();
	if (currentDirection.x != 0 || currentDirection.y != 0)
		Vector2Normalize(currentDirection);
	float continueWeight = 0.5;
	direction = Vector2Add(direction, Vector2Scale(currentDirection, continueWeight));

	//then get their average direction/speed
	Vector2 speedDifference = Vector2Subtract(AverageSpeeds(), getVelocity());
	if (speedDifference != Vector2Zero())
	{
		//normalize vector
		speedDifference = Vector2Normalize(speedDifference);
		//add this with weight to the direction
		float speedWeight = 5;
		direction = Vector2Add(direction, Vector2Scale(speedDifference, speedWeight));
	}
	//find the position to cluster to
	Vector2 targetPosition = AverageNeighborPositions();
	if (targetPosition != Vector2Zero())
	{
		//normalize vector
		targetPosition = Vector2Normalize(targetPosition);
		//add this with weight to the direction
		float centerWeight = 2;
		direction = Vector2Add(direction, Vector2Scale(targetPosition, centerWeight));
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
		direction = Vector2Add(direction, Vector2Scale(nearestNeighbor, -avoidNeighborWeight));
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
		direction = Vector2Add(direction, Vector2Scale(nearestObstacle, -avoidObstacleWeight));
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
	vector<DistObj> allNearby = Game::Instance->FindNearbyObjects(getPosition(), range);
	
	//sort the objects into two types, neighbors (birds) and obstacles (player, projectiles, boundaries)
	for (int i = 0; i < allNearby.size(); i++)
	{
		GameObject* obj = allNearby[i].obj;
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
}


Vector2 Bird::AverageSpeeds()
{
	//prepare result variable
	Vector2 result = { 0,0 };
	//add up all the velocities of neighbors into that result
	for (int i = 0; i < neighbors.size(); i++)
	{
		result = result + (neighbors[i]->getVelocity() * neighbors[i]->size);
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
		return Vector2Subtract(neighbors[0]->getPosition(), getPosition());
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

	//check for being close to boundary
	Vector2 pos = getPosition();
	//calculate distances to borders
	float leftDistance = pos.x - Game::Instance->worldLeftBorder;
	float rightDistance = Game::Instance->worldRightBorder - pos.x;
	float topDistance = pos.y - Game::Instance->worldTopBorder;
	float bottomDistance = Game::Instance->worldBottomBorder - pos.y;

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
		result = Vector2Subtract(obstacles[0]->getPosition(), getPosition());
	else result = Vector2Normalize(result);

	return result;
}


Vector2 Bird::AverageNeighborPositions()
{
	//prepare result variable
	Vector2 result = { 0,0 };
	//add up all the velocities of neighbors into that result
	for (int i = 0; i < neighbors.size(); i++)
	{
		result = Vector2Add(result, Vector2Subtract(neighbors[i]->getPosition(), getPosition()));
	}
	//divide result by amount of values
	Vector2Divide(result, neighbors.size());
	return result;
}


void Bird::CheckForCollision()
{
	//since the obstacle list is sorted, and the nearest one is #0, and if any obstacle is close enough to collide #0 also is, we just check with #0
	if (obstacles.size() > 0)
	{
		float distance = Vector2Distance(obstacles[0]->getPosition(), getPosition());
		if (distance <= size + obstacles[0]->size)
		{
			deletionFlag = true;
			//inform obstacle that it hit something
			obstacles[0]->Collided();
			return;
		}
	}

	//check if bird is out of bounds
	Vector2 pos = getPosition();
	if (pos.x < Game::Instance->worldLeftBorder || pos.x > Game::Instance->worldRightBorder || pos.y < Game::Instance->worldTopBorder || pos.y > Game::Instance->worldBottomBorder)
	{
		//trigger deletion flag
		deletionFlag = true;
	}
}