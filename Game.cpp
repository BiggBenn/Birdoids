#include "Game.hpp"
#include "Bird.hpp"
#include <time.h>
#include <algorithm>


Game* Game::getInstance()
{
    //ensure instance exists, or else initialize it
    if (Instance == nullptr)
        Instance = new Game();
    return Instance;
}

Game::Game()
{

}


Game::~Game()
{

}


//Initialization function
bool Game::InitializeWindow(int width, int height)
{
    InitWindow(width, height, "Birdoids");
    worldHeight = height;
    worldWidth = width;
    worldLeftBorder = - width / 2;
    worldRightBorder = width / 2;;
    worldTopBorder = -height / 2;;
    worldBottomBorder = height / 2;;
    SetTargetFPS(60);   //60fps should suffice for now
    return IsWindowReady();
}


// Main loop function, will repeat until the game is closed. Any game logic should be within this.
void Game::MainLoop()
{
    while (!WindowShouldClose())
    {
        //
        // Update part of the cycle
        //

        if (!victory && !paused)
        {

            //get the time that passed
            float frameTime = GetFrameTime();
            if (frameTime > 1.0f/30.0f)
            {
                frameTime = 1.0f/30.0f;
            }

            //update timer
            timer += frameTime;

            UpdateObjects(frameTime);

            HandleDeletion();

            CheckForWinCondition();
        }

        //F1 button for help screen
        if (IsKeyPressed(KEY_F1))
        {
            //switch help flag and pause flag
            helpFlag = helpFlag;
            paused = !paused;
        }


        //
        // Render section of mainloop
        //

        //signal that drawing in 2D has begun
        BeginDrawing();
        BeginMode2D(cam);

        //first clean up render frame
        ClearBackground(BLACK);

        //loop through objects
        for (GameObject* object: allObjects)
        {
            object->Render();
        }

        //draw the ui on top
        DrawUI();

        //signal that drawing has ended
        EndMode2D();
        EndDrawing();
    }
}


void Game::UpdateObjects(float frameTime)
{
    for (GameObject* obj : toUpdate)
    {
        //update object
        obj->Update(frameTime);
        //check if it has triggered a delete flag
        if (obj->deletionFlag)
        {
            //if so, add it to the toDelete vector
            toDelete.push_back(obj);
        }
    }
}


void Game::HandleDeletion()
{
    // Clean up objects marked for removal, slow but rare enough
    for (GameObject* deletee : toDelete)
    {
        //remove object from object vectors using Erase-Remove-idiom
        allObjects.erase(remove(allObjects.begin(), allObjects.end(), deletee), allObjects.end());
        toUpdate.erase(remove(toUpdate.begin(), toUpdate.end(), deletee), toUpdate.end());

        //remove object from quadrant system, *before* deletion, otherwise positional data will likely be corrupted
        RemoveFromQuadrantSystem(deletee);

        //finally delete it 
        delete deletee;
    }
    //clear deletion vector
    toDelete.clear();
}


void Game::CheckForWinCondition()
{
    //check for win condition
    if (birdCounter <= 0)
    {
        victory = true;
        score = floor(score / floor(timer));
    }
}


vector<DistObj> Game::FindNearbyObjects(Vector2 position, float range)
{
    vector<DistObj> result;

    //calculate the square of range for cheaper computation of distances to compare against
    float rangeSqrd = range * range;

    //compile vector of all quadrants that need to be searched
    vector<QuadrantKey> quadrants;

    //calculate first quadrant
    QuadrantKey baseQuadrant = CalculateQuadrant(position);

    //calculate the range of quadrants that need to be searched from range
    //If the range is less than the quadrant size, still, by being near the edges, we have to search the quadrant the object is in, and the quadrants surrounding it
    //if the range is larger than quadrant size, there could still be relevant objects two quadrants away! etc.
    int quadRange = (range / quadrantSize) + 1;

    //oscillate left to right, top to bottom around initial quadrant to add all the relevant quadrants
    for (int x = -quadRange; x <= quadRange; x++)
    {
        for (int y = -quadRange; y <= quadRange; y++)
        {
            QuadrantKey key;
            //add x and y coordinates to center quadrant to get all quadrants around center
            key.x = baseQuadrant.x + x;
            key.y = baseQuadrant.y + y;
            quadrants.push_back(key);
        }
    }


    //iterate through all quadrants
    for (QuadrantKey key : quadrants)
    {
        //first check for existence of quadrant
        if (worldGrid.count(key) != 0)
        {
            //get the relevant quadrant
            vector<GameObject*> quadrant = worldGrid.at(key);
            for (GameObject* obj : quadrant)
            {
                //calculate distance (squared for performance saving)
                float distanceSqrd = Vector2DistanceSquared(position, obj->getPosition());

                //subtract object's size from distance
                distanceSqrd -= obj->size;

                //if distance is larger than range, dont add this to the result
                if (distanceSqrd < rangeSqrd)
                {
                    result.push_back({ obj, distanceSqrd });
                }
            }
        }
    }
    
    //sort the vector before returning it
    sort(result.begin(), result.end());


    return result;
}


void Game::OnObjectMoved(GameObject* obj, Vector2 previousPosition)
{
    //get the different quadrants for current and previous position
    QuadrantKey prevQuadrant = CalculateQuadrant(previousPosition);
    QuadrantKey currentQuadrant = CalculateQuadrant(obj->getPosition());

    //compare to see if they are different
    if (prevQuadrant.x != currentQuadrant.x || prevQuadrant.y != currentQuadrant.y)
    {
        //remove object from previous quadrant
        RemoveFromQuadrant(prevQuadrant, obj);

        //add to new quadrant
        AddToQuadrant(currentQuadrant, obj);
    }
}


void Game::AddToQuadrantSystem(GameObject* obj)
{
    //calculate the initial quadrant this object resides in
    QuadrantKey quadrant = CalculateQuadrant(obj->getPosition());

    //add object to that quadrant
    AddToQuadrant(quadrant, obj);
}


void Game::RemoveFromQuadrantSystem(GameObject* obj)
{
    //calculate current quadrant
    QuadrantKey quadrant = CalculateQuadrant(obj->getPosition());

    //remove from that quadrant
    RemoveFromQuadrant(quadrant, obj);
}


void Game::RemoveFromQuadrant(QuadrantKey quadrantKey, GameObject* obj)
{
    //ensure that quadrant exists
    if (worldGrid.count(quadrantKey) == 0)
        return;
    
    //get relevant quadrant
    vector<GameObject*>* quadrant = &worldGrid[quadrantKey];

    //use erase remove idiom to remove obj from quadrant 
    quadrant->erase(remove(quadrant->begin(), quadrant->end(), obj), quadrant->end());

    //erasing quadrants is practically unnecessary, as the gameworld isn't that big. Otherwise, here empty quadrants could be deleted.
    /*//check if the quadrant is empty, if so get rid of it
    if (worldGrid.at(quadrant).size() == 0)
        worldGrid.erase(quadrant);*/
}


void Game::AddToQuadrant(QuadrantKey quadrant, GameObject* obj)
{
    worldGrid[quadrant].push_back(obj);
}

//initialize the game world, camera, etc
void Game::InitializeGame()
{
    //set up RNG
    srand(time(NULL));

    //variables
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    //set up the camera
    cam.target = { 0,0 };
    cam.offset = { (float)screenWidth / 2, (float)screenHeight / 2 };
    cam.zoom = 1;

    //add a bunch of birds
    for (int i = 0; i < 500; i++)
    {
        Bird* bird = new Bird();
        Vector2 position = { (float)(rand() % screenWidth - screenWidth / 2) , (float)(rand() % screenHeight - screenHeight / 2) };
        bird->setPosition(position * 0.9);  //multiplication with 0.9 is to ensure distance from the borders
        AddObject(bird, true);
    }

    //spawn the player
    Player* player = new Player();
    AddObject(player, true);
}


QuadrantKey Game::CalculateQuadrant(Vector2 position)
{
    QuadrantKey result;
    //calculate quadrant values
    result.x = floorf(position.x / quadrantSize);
    result.y = floorf(position.y / quadrantSize);
    return result;
}


void Game::DrawUI()
{
    //prep score string
    const char* str = TextFormat("Score: %.4d\nTime: %.0f\nBirds left: %.3d", score, timer, birdCounter);
    //draw score in the top left corner
    DrawText(str, worldLeftBorder + 15, worldTopBorder + 15, 35, { 125,100,255,128 });

    //Draw help hint in top right
    DrawText("Press F1 to show controls", worldRightBorder - 150, worldTopBorder + 15, 10, WHITE);

    //draw help text if the player wants it
    if (helpFlag)
    {
        string text = "Controls:\n\nArrow Keys: move forward/backward, turn left/right\nRamming birds kills them!\nSpace: fire bullet forwards, big bullet breaks after 25 hits,\nwhile the fast bullet breaks after one\nCtrl: Switch between rapidfire and slowfire bullets\nAlt: turn on/off laying of \"mines\", \nwhich break after 5 hits & last 120 seconds\nYour goal is to kill all the birds that flock around.\nYour score increases with each bird killed,\nand the score is divided by the time at the end. Good luck!\n\nHint: Unlike birds, you can wrap around the screen!\n\nDouble Hint: Use your mines to corral the birds,\nthen strike with your bullet!\n\nEscape to quit";
        DrawText(text.c_str() , -350, worldTopBorder + 50, 30, WHITE);
    }

    if (victory)
    {
        const char* victoryText = TextFormat("Congratulations!\nYou won the game,\nand killed all those darned birds!\n\nYour final score is: %.4d\n\nThank you for playing my crappy game :)", score);
        DrawText(victoryText, -400, worldTopBorder + 250, 40, YELLOW);
    }
}


void Game::AddObject(GameObject* obj, bool update)
{
    allObjects.push_back(obj);
    if (update)
        toUpdate.push_back(obj);
}
