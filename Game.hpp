#pragma once
#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <math.h>
#include <cstdio>
#include <unordered_map>
#include "raylib.h"
#include "GameObject.hpp"
#include "Player.hpp"
#include "Structs.hpp"

using namespace std;



class Game
{
	//
	// Public Variables
	//
public: 
    
    //Singleton-type static reference
    inline static Game* Instance = nullptr;

    //world size variables
    int worldWidth;
    int worldHeight;

    //world coordinates
    float worldLeftBorder;
    float worldRightBorder;
    float worldTopBorder;
    float worldBottomBorder;

    //Score counter
    int score = 0;

    //flag that controls whether controls are displayed or not
    bool helpFlag = false;

    //flag that controls whether the game is paused or not
    bool paused = false;

    //time that the game is running, for score purposes
    float timer = 0;

    //counts how many birds are left
    int birdCounter = 0;

    //
    // Private Variables
    //
private:

    //vectors holding all the game objects, and one to hold game object pointers that should be deleted
    vector<GameObject*> allObjects;
    vector<GameObject*> toDelete;

    //vector to hold all the gameobjects that *need* updating, so not including walls or passive obstacles
    vector<GameObject*> toUpdate;

    //std::unordered_map<std::string, std::vector<std::string*>> worldGrid;

    //this map represents a grid in the world, size determined by next variable. Used to reduce compute time required to find nearby objects, by only searching nearby quadrants
    unordered_map<QuadrantKey, vector<GameObject*>, QuadrantKeyHash> worldGrid;

    //this represents the size in pixels/units of a single quadrant in the worldGrid
    int quadrantSize = 128;

    //camera to center on the playing field
    Camera2D* cam;

    //win variable, if this is true the player has won
    bool victory = false;

    //
    // Public Functions
    //
public: 

    //constructor
    Game();

    //destructor
    ~Game();

    //initializes raylib window
    bool InitializeWindow(int width, int height);

    // Main loop function, will repeat until the game is closed. Any game logic should be within this.
    void MainLoop();

    //Returns all objects within range of the position, sorted by distance
    vector<DistObj> FindNearbyObjects(Vector2 position, float range);

    //Objects call this function to notify the game that they moved
    void OnObjectMoved(GameObject* obj, Vector2 previousPosition);

    //Adds an object to the quadrant system in the first place, ideally in constructor of object
    void AddToQuadrantSystem(GameObject* obj);

    //removes object from quadrant system entirely, on destruction for example
    void RemoveFromQuadrantSystem(GameObject* obj);

    //Add a game object to the world
    void AddObject(GameObject* obj, bool update);

    //
    // Private Functions
    //
private:

    //Removes an object from a quadrant, if it is in there
    void RemoveFromQuadrant(QuadrantKey quadrant, GameObject* obj);

    //Adds an object to a quadrant, even if it is already in there.
    void AddToQuadrant(QuadrantKey quadrant, GameObject* obj);

    //initializes the game world
    void InitializeGame();

    //calculates the quadrant vector from a position vector
    QuadrantKey CalculateQuadrant(Vector2 position);

    //Draw the ui stuff, score, weapon choice etc
    void DrawUI();

    //Iterates through all objects that need update and updates them
    void UpdateObjects(float frameTime);

    //Deletes all the objects that are queued up for deletion
    void HandleDeletion();

    //Checks whether or not the win conditions are met, and if so, triggers the victory flag
    void CheckForWinCondition();
};

#endif