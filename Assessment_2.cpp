// Assessment_2.cpp: A program using the TL-Engine
//Samual Hilland
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

#include <sstream> //allows the use of dynamic variables + text
using namespace std; 

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );

	/**** Set up your scene here ****/

	//car speed var
	const float playerCarSpeed = 0.02f; //sets the speed the car travels in all directions including rotation

	//dialogue backdrop box & font locations:
	const float  dialogueTextX = 600; //text loc
	const float  dialogueTextZ = 660;

	const float dialogueBoxX = 300; //box background loc
	const float  dialogueBoxZ = 650;

	const float gamePausedTextX = 0; //Paused text location
	const float gamePausedTextZ = 0;

	const float fontSize = 35; //size of text

	const float pointValue = 10; //amount of points gained when a car is hit.

	//skybox default location:
	const float skyboxX = 0.0f;
	const float skyboxY = -960.0f;
	const float skyboxZ = 0.0f;
	//Camera location & rotate vars:
	const float camera1X = 0.0f;
	const float camera1Y = 15.0f;
	const float camera1Z = -60.0f;
	const float camera1RotateX = 15;
	//enemy 1 default location:
	const float enemy1X = -20.0f;
	const float enemy1Y = 0.0f;
	const float enemy1Z = 20.0f;
	//enemy 2 default location:
	const float enemy2X = 20.0f;
	const float enemy2Y = 0.0f;
	const float enemy2Z = 20.0f;

	//crash resolution and detection:
	//enemy Bounding box of geometry = (-0.946118,0.0065695,-1.97237) to (0.946118,1.50131,1.97237).

	//enemy 1 bounding box values:
	const float enemy1BoundingMinX = -20.946118; //x coordinate (-20) - half the radius of enemy model
	const float enemy1BoundingMaxX = -19.053882; //x coordinate (-20)  + half the radius 
	const float enemy1BoundingMinY = -0.92633728;//y coordinate (0) - half the radius 
	const float enemy1BoundingMaxY = 0.926337285;//y coordinate (0) + half the radius 
	const float enemy1BoundingMinZ = 18.02763; //z coordinate (20) - half the radius 
	const float enemy1BoundingMaxZ = 21.97237; //z coordinate (20) - half the radius 

	//enemy 2 bounding box values:
	const float enemy2BoundingMinX = 19.059882;
	const float enemy2BoundingMaxX = 20.940118;
	const float enemy2BoundingMinY = -0.92633728;
	const float enemy2BoundingMaxY = 0.926337285;
	const float enemy2BoundingMinZ = 18.02763;
	const float enemy2BoundingMaxZ = 21.97237;

	float playerRadius = 0;

	//enemy 1 hit detection
	const float kCar1MinX = enemy1BoundingMinX - playerRadius;
	const float kCar1MaxX = enemy1BoundingMaxX + playerRadius;
	const float kCar1MinY = enemy1BoundingMinY - playerRadius;
	const float kCar1MaxY = enemy1BoundingMaxY + playerRadius;
	const float kCar1MinZ = enemy1BoundingMinZ - playerRadius;
	const float kCar1MaxZ = enemy1BoundingMaxZ + playerRadius;

	//enemy 2 hit detection
	const float kCar2MinX = enemy2BoundingMinX - playerRadius;
	const float kCar2MaxX = enemy2BoundingMaxX + playerRadius;
	const float kCar2MinY = enemy2BoundingMinY - playerRadius;
	const float kCar2MaxY = enemy2BoundingMaxY + playerRadius;
	const float kCar2MinZ = enemy2BoundingMinZ - playerRadius;
	const float kCar2MaxZ = enemy2BoundingMaxZ + playerRadius;

	float playerScore = 0; //stores user score, default 0

	bool enemy1Hit = false; //checks if enemy 1 has already been hit or not
	bool enemy2Hit = false; //checks if enemy 2 has already been hit or not

	enum EColourStates { Red, White}; //stores colours for indication spheres
	EColourStates sphere1Colour = White; //stores current 1st indication colour
	EColourStates sphere2Colour = White; //stores current 2nd indication colour

	enum EgameStates { Playing, Paused, GameWon }; //stores gamestates
	EgameStates gameState = Playing; //sets base game state

	bool cameraIsAttached = false; //is the camera attached to parent?

	ICamera* myCamera;//Creation of camera
	myCamera = myEngine->CreateCamera(kManual);
	myCamera->SetPosition(camera1X, camera1Y, camera1Z); //camera pos
	myCamera->RotateLocalX(camera1RotateX); //rotates camera

	//ground model:
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IModel* groundModel = groundMesh->CreateModel();
	//Skybox:
	IMesh* skyBoxMesh = myEngine->LoadMesh("skybox01.x");
	IModel* skyBoxModel = skyBoxMesh->CreateModel(skyboxX, skyboxY, skyboxZ);
	//Player car:
	IMesh* playerCarMesh = myEngine->LoadMesh("4x4jeep.x");
	IModel* playerCarModel = playerCarMesh->CreateModel(); //creates player model
	//enemy cars:
	IMesh* enemyCarMesh = myEngine->LoadMesh("audi.x");//Creates mesh for enemy cars
	IModel* enemy1 = enemyCarMesh->CreateModel(); //creates 1st enemy model
	enemy1->SetPosition(enemy1X, enemy1Y, enemy1Z); //default enemy 1 location

	IModel* enemy2 = enemyCarMesh->CreateModel(); //creates 2nd enemy model
	enemy2->SetPosition(enemy2X, enemy2Y, enemy2Z); //default enemy 2 location

	//indication balls;
	IMesh* ballMesh = myEngine->LoadMesh("ball.x"); //creates ball mesh for enemy indication
	IModel* indicationBall1 = ballMesh->CreateModel(); //enemy indication ball 1
	indicationBall1->SetPosition(-20, 3.0f, 20); //sets location above car
	//indicationBall1->AttachToParent(enemy1); //attaches to enemy car 1

	IModel* indicationBall2 = ballMesh->CreateModel(); //enemy indication ball 2
	indicationBall2->SetPosition(20, 3.0f, 20); //sets location above car
	//indicationBall2->AttachToParent(enemy2); //attaches to enemy car 2

	//dialogue backdrop box & font:
	ISprite* dialogueBackDrop = myEngine->CreateSprite("backdrop.jpg", dialogueBoxX, dialogueBoxZ); //300 , 650 was the best fitting on my home pc

	IFont* myFont = myEngine->LoadFont("Comic Sans MS", fontSize); //adds comic sans
	IFont* gameStateFont = myEngine->LoadFont("Comic Sans MS", fontSize); //adds comic sans
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		if (myEngine->KeyHit(Key_Escape)) //quits game if pressed.
		{
			myEngine->Stop();
		}

		//stores old location for crash detection:
		float prevXPos = playerCarModel->GetX();
		float prevYPos = playerCarModel->GetY();
		float prevZPos = playerCarModel->GetZ();

		//Player car movement:
		if (myEngine->KeyHeld(Key_W))
		{
			playerCarModel->MoveLocalZ(playerCarSpeed); //moves forward
		}
		if (myEngine->KeyHeld(Key_S))
		{
			playerCarModel->MoveLocalZ(-playerCarSpeed); //moves backwards
		}
		if (myEngine->KeyHeld(Key_D))
		{
			playerCarModel->RotateLocalY(playerCarSpeed); //rotates clockwise
		}
		if (myEngine->KeyHeld(Key_A))
		{
			playerCarModel->RotateLocalY(-playerCarSpeed); //rotates anti-clockwise
		}

		//Score text:
		stringstream scoreText;
		scoreText << "Score: " << playerScore;
		myFont->Draw(scoreText.str(), dialogueTextX, dialogueTextZ);

		//crash detection
		float x, y, z;
		x = playerCarModel->GetX();
		y = playerCarModel->GetY();
		z = playerCarModel->GetZ();

		if (x > kCar1MinX && x < kCar1MaxX && y > kCar1MinY && y < kCar1MaxY && z > kCar1MinZ && z < kCar1MaxZ)
		{//hit enemy1
			if (sphere1Colour == White) //if eCar1 hasnt been hit, add points & set to red
			{
				sphere1Colour = Red;//sets current colour var to red
				indicationBall1->SetSkin("red.png"); //sets texture to red

				playerScore = playerScore + pointValue; //adds points to score
				playerCarModel->SetPosition(prevXPos, prevYPos, prevZPos); //resets car pos
			}
			else if (sphere1Colour == Red)
			{
				indicationBall1->SetSkin("red.png");//failsafe, sets colour to red
				playerCarModel->SetPosition(prevXPos, prevYPos, prevZPos);//resets car pos
			}
		}

		if (x > kCar2MinX && x < kCar2MaxX && y > kCar2MinY && y < kCar2MaxY && z > kCar2MinZ && z < kCar2MaxZ)
		{//hit enemy2
			if (sphere2Colour == White) //if eCar2 hasnt been hit, add points & set to red
			{
				sphere2Colour = Red;//sets current colour var to red
				indicationBall2->SetSkin("red.png");//sets texture to red

				playerScore = playerScore + pointValue; //adds points to score
				playerCarModel->SetPosition(prevXPos, prevYPos, prevZPos); //resets car pos
			}
			else if (sphere2Colour == Red)
			{
				indicationBall2->SetSkin("red.png");//failsafe, sets colour to red
				playerCarModel->SetPosition(prevXPos, prevYPos, prevZPos);//resets car pos
			}
		}

		/*if (playerScore >= 2 && sphere2Colour == Red && sphere2Colour == Red)
		{
			myFont->Draw("Game Won", dialogueTextX, dialogueTextZ);
		}*/


	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
