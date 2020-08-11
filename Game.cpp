#include "Game.hpp"
#include "Bird.hpp"
#include <time.h>
#include <algorithm>


Game::Game()
{
    Instance = this;
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
    InitializeGame();

    while (!WindowShouldClose())
    {
        //
        // Update all game objects, and mark "killed" objects for deletion
        //

        if (!victory)
        {

            //get the time that passed
            float frameTime = GetFrameTime();
            if (frameTime > (float)1 / 30)
                frameTime = (float)1 / 30;

            //update timer
            timer += frameTime;

            for (int i = 0; i < toUpdate.size(); i++)
            {
                //get the object that we're looking at 
                GameObject* obj = toUpdate.at(i);

                //update it
                obj->Update(frameTime);
                //check if it has triggered a delete flag
                if (obj->deletionFlag)
                {
                    //if so, add it to the toDelete vector
                    toDelete.push_back(obj);
                }
            }

            //
            // Clean up objects marked for removal, slow but rare enough
            //
            for (int i = 0; i < toDelete.size(); i++)
            {
                //remove objects from object vectors
                _Erase_remove(allObjects, toDelete[i]);
                _Erase_remove(toUpdate, toDelete[i]);

                //finally delete it 
                delete toDelete[i];
            }
            //clear deletion vector
            toDelete.clear();

            //check for win condition
            if (birdCounter <= 0)
            {
                victory = true;
                score = floor(score / floor(timer));
            }
        }


        //
        // Render all objects
        //

        //signal that drawing in 2D has begun
        BeginDrawing();
        BeginMode2D(*cam);

        //first clean up render frame
        ClearBackground(BLACK);

        //loop through objects
        for (int i = 0; i < allObjects.size(); i++)
        {
            allObjects.at(i)->Render();
        }

        //draw the ui on top
        DrawUI();

        //signal that drawing has ended
        EndMode2D();
        EndDrawing();
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
    quadrants.push_back(CalculateQuadrant(position));

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
            key.x = quadrants[0].x + x;
            key.y = quadrants[0].y + y;
            quadrants.push_back(key);
        }
    }


    //iterate through all quadrants
    for (int i = 0; i < quadrants.size(); i++)
    {
        //first check for existence of quadrant
        if (worldGrid.count(quadrants.at(i)) != 0)
        {
            //get the relevant quadrant
            vector<GameObject*> quadrant = worldGrid.at(quadrants.at(i));
            for (int j = 0; j < quadrant.size(); j++)
            {
                GameObject* obj = quadrant.at(j);

                //calculate distance (squared for performance saving)
                float distance = Vector2DistanceSquared(position, obj->getPosition());

                //subtract object's size from distance
                distance -= obj->size;

                //if distance is larger than range, dont add this to the result
                if (distance < rangeSqrd)
                {
                    result.push_back({ obj, distance });
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


void Game::RemoveFromQuadrant(QuadrantKey quadrant, GameObject* obj)
{
    //ensure that quadrant exists
    if (worldGrid.count(quadrant) == 0)
        return;
    //get beginning and end of relevant quadrant, we can assume that it exists since the object is supposedly in it already
    vector<GameObject*>::iterator beginning = worldGrid.at(quadrant).begin();
    vector<GameObject*>::iterator end = worldGrid.at(quadrant).end();

    //remove obj
    worldGrid.at(quadrant).end() = remove(beginning, end, obj);

    //erasing quadrants is practically unnecessary, as the gameworld isn't that big. Otherwise, here empty quadrants could be deleted.
/*    //check if the quadrant is empty, if so get rid of it
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
    cam = new Camera2D();
    cam->target = { 0,0 };
    cam->offset = { (float)screenWidth / 2, (float)screenHeight / 2 };
    cam->zoom = 1;

    //add random obstacles for the swarm, just for fun, really.
    for (int j = 0; j < 0; j++)
    {
        GameObject* obstacle = new GameObject();
        obstacle->setPosition({(float) (rand() % screenWidth - screenWidth / 2) , (float)(rand() % screenHeight - screenHeight / 2) });
        allObjects.push_back(obstacle);
    }

    //add a bunch of birds
    for (int i = 0; i < 500; i++)
    {
        Bird* bird = new Bird();
        Vector2 position = { (float)(rand() % screenWidth - screenWidth / 2) , (float)(rand() % screenHeight - screenHeight / 2) };
        bird->setPosition(position * 0.9);  //multiplication with 0.9 is to ensure distance from the borders
        allObjects.push_back(bird);
        toUpdate.push_back(bird);
    }

    //spawn the player
    Player* player = new Player();
    allObjects.push_back(player);
    toUpdate.push_back(player);
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
    char* str = new char[256];
    sprintf_s(str, 256, "Score: %.4d\nTime: %.0f\nBirds left: %.3d", score, timer, birdCounter);
    //draw score in the top left corner
    DrawText(str, worldLeftBorder + 15, worldTopBorder + 15, 35, { 125,100,255,128 });

    //Draw help hint in top right
    DrawText("Press F1 to show controls", worldRightBorder - 150, worldTopBorder + 15, 10, WHITE);

    //draw help text if the player wants it
    if (helpFlag)
    {
        string text = "Controls:\n\nArrow Keys: move forward/backward, turn left/right\nSpace: fire bullet forwards, breaks after 4 hits\nAlt: turn on/off laying of \"mines\", \nwhich break after 5 hits & last 120 seconds\nYour goal is to kill all the birds that flock around.\nYour score increases with each bird killed,\nand the score is divided by the time at the end. Good luck!\n\nHint: Unlike birds, you can wrap around the screen!\n\nDouble Hint: Use your mines to corral the birds,\nthen strike with your bullet!\n\nEscape to quit";
        DrawText(text.c_str() , -350, worldTopBorder + 50, 30, WHITE);
    }

    if (victory)
    {
        string str = "Congratulations!\nYou won the game,\nand killed all those darned birds!\n\nYour final score is: %.4d\n\nThank you for playing my crappy game :)";
        char* victoryText = new char[str.size()];
        sprintf_s(victoryText, str.size() + 32, str.c_str(), score);
        DrawText(victoryText, -400, worldTopBorder + 250, 40, YELLOW);
    }
}


void Game::AddObject(GameObject* obj, bool update)
{
    allObjects.push_back(obj);
    if (update)
        toUpdate.push_back(obj);
}
