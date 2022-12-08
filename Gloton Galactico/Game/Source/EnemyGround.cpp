#include "EnemyGround.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include "Map.h"
#include "SceneIntro.h"
#include "FadeToBlack.h"
#include "EntityManager.h"
#include "SDL/include/SDL_render.h"
#include "Animation.h"

EnemyGround::EnemyGround() : Entity(EntityType::ENEMY_GROUND)
{
	name.Create("Enemy Ground");

	//Pushback animation - Idle
	baseAnimation.PushBack({ 0, 0, 50, 50 });

	baseAnimation.loop;
	baseAnimation.speed = 0.05f;


	//Pushback animation - Run
	runningAnimation.PushBack({ 0, 0, 50, 50 });
	runningAnimation.PushBack({ 50, 0, 50, 50 });
	runningAnimation.PushBack({ 100, 0, 50, 50 });
	runningAnimation.PushBack({ 0, 50, 50, 50 });
	runningAnimation.PushBack({ 50, 50, 50, 50 });
	runningAnimation.PushBack({ 100, 50, 50, 50 });

	runningAnimation.loop;
	runningAnimation.speed = 0.1f;

	//Pushback animation - Jumping
	jummpingAnimation.PushBack({ 0, 100, 50, 50 });
	jummpingAnimation.PushBack({ 50, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });		//Repetido tantas veces por si un salto es muy alto
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });
	jummpingAnimation.PushBack({ 100, 100, 50, 50 });

	jummpingAnimation.loop = false;
	jummpingAnimation.speed = 0.1f;

	//Pushback animation - death

	dyingAnimation.PushBack({ 0, 152, 50, 50 });
	dyingAnimation.PushBack({ 100, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });
	dyingAnimation.PushBack({ 150, 152, 50, 50 });

	dyingAnimation.loop;
	dyingAnimation.speed = 0.05f;


}

EnemyGround::~EnemyGround() {

}

bool EnemyGround::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());

	//L02: DONE 5: Get Player parameters from XML
	//position.x = parameters.attribute("x").as_int();
	//position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	jumpFxPath = parameters.attribute("jumpFxPath").as_string();

	return true;
}

bool EnemyGround::Start() {

	//initilize textures
	enemyGroundTexture = app->tex->Load(texturePath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y + 200, 12, bodyType::DYNAMIC);

	//b2MassData mass;
	//mass.mass = 10;
	//pbody->body->SetMassData(b2MassData())

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetLinearVelocity(b2Vec2(0, -GRAVITY_Y));

	//initialize audio effect - !! Path is hardcoded, should be loaded from config.xml
	jumpFxId = app->audio->LoadFx(jumpFxPath);

	currentAnimation = &baseAnimation;

	return true;
}

bool EnemyGround::Update()
{
	currentAnimation->Update();
	app->sceneIntro->playing = true;

	// L07 DONE 5: Add physics to the player - updated player position using physics

	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);

	if (app->scene->player->position.x + 100 > this->position.x && app->scene->player->position.x - 100 < this->position.x &&
		app->scene->player->position.y + 100 > this->position.y && app->scene->player->position.y - 100 < this->position.y)
	{
		this->pathfinding = true;
	}
	else
	{
		this->pathfinding = false;
	}
	
	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	
	if (this->pathfinding == true)
	{
		if (app->scene->player->position.x <= this->position.x) {
			vel = b2Vec2(-4, -GRAVITY_Y);
		}

		if (app->scene->player->position.x - 10 > this->position.x) {
			vel = b2Vec2(4, -GRAVITY_Y);
		}

	}

	//TP
	if (newPosEnemy.t == true)
	{
		b2Vec2 resetPos = b2Vec2(PIXEL_TO_METERS(newPosEnemy.posX), PIXEL_TO_METERS(newPosEnemy.posY));
		this->pbody->body->SetTransform(resetPos, 0);

		newPosEnemy.t = false;
	}

	//Set the velocity of the pbody of the player
	this->pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//app->render->DrawTexture(texture, position.x, position.y);

	app->render->FitCameraInsideBounds();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(enemyGroundTexture, -7 + position.x, -20 + position.y, &rect, 1.0f, NULL, NULL, NULL, rotar);

	return true;
}

bool EnemyGround::CleanUp()
{
	return true;
}



// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void EnemyGround::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	}

}

void EnemyGround::Teleport(int x, int y)
{
	newPosEnemy.posX = x;
	newPosEnemy.posY = y;
	newPosEnemy.t = true;
}
