#include "EnemyFly.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h" 
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "SceneIntro.h"
#include "EntityManager.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Animation.h"

EnemyFly::EnemyFly() : Entity(EntityType::ENEMY_FLY)
{
	name.Create("Enemies");

	idleAnimEnemy.PushBack({ 0,0,50,50 });
	idleAnimEnemy.PushBack({ 0,0,50,50 });
	idleAnimEnemy.PushBack({ 0,0,50,50 });
	idleAnimEnemy.PushBack({ 0,0,50,50 });
	idleAnimEnemy.PushBack({ 50,0,50,50 });
	idleAnimEnemy.PushBack({ 50,0,50,50 });
	idleAnimEnemy.PushBack({ 50,0,50,50 });
	idleAnimEnemy.PushBack({ 50,0,50,50 });

	idleAnimEnemy.speed = 0.1f;
	idleAnimEnemy.loop = true;

	movingAnimEnemy.PushBack({ 0,0,50,50 });
	movingAnimEnemy.speed = 0.2f;
	movingAnimEnemy.loop = true;

	deathAnimEnemy.PushBack({ 0,50,50,50 });
	deathAnimEnemy.PushBack({ 0,50,50,50 });
	deathAnimEnemy.PushBack({ 50,50,50,50 });
	deathAnimEnemy.PushBack({ 50,50,50,50 });
	deathAnimEnemy.PushBack({ 0,100,50,50 });
	deathAnimEnemy.PushBack({ 0,100,50,50 });
	deathAnimEnemy.PushBack({ 50,100,50,50 });
	deathAnimEnemy.PushBack({ 50,100,50,50 });
	deathAnimEnemy.PushBack({ 0,150,50,50 });
	deathAnimEnemy.PushBack({ 0,150,50,50 });
	deathAnimEnemy.PushBack({ 50,150,50,50 });
	deathAnimEnemy.PushBack({ 50,150,50,50 });
	deathAnimEnemy.PushBack({ 0,200,50,50 });
	deathAnimEnemy.PushBack({ 0,200,50,50 });
	deathAnimEnemy.PushBack({ 0,200,50,50 });
	deathAnimEnemy.PushBack({ 0,200,50,50 });
	deathAnimEnemy.PushBack({ 0,200,50,50 });
	deathAnimEnemy.speed = 0.0001f;
	deathAnimEnemy.loop = true;

}

EnemyFly::~EnemyFly() {

}

bool EnemyFly::Awake() {

	position = { 1600, 160 };
	//texturePath = parameters.child("enemy_ground").attribute("texturepath").as_string();
	texturePath = "Assets/Textures/enemyFlyAnimation.png";

	enemyFxPath = "Assets/Audio/Fx/enemy.wav";

	return true;
}

bool EnemyFly::Start()
{
	texture = app->tex->Load(texturePath);
	pathTileTex = app->tex->Load("Assets/Maps/MapMetadata.png");
	audio = app->audio->LoadFx(enemyFxPath);

	isDead = false;
	deathtimmer = 40;

	this->currentAnimation = &idleAnimEnemy;

	return true;
}

bool EnemyFly::Update()
{
	if (col)
	{
		pbody = app->physics->CreateCircle(position.x, position.y, 7.5, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;
		pbody->body->SetGravityScale(0);

		LOG("PosX: %d", position.x + (8 * 2));
		LOG("PosY: %d", position.y + (8 * 2));

		//Sensor de si el player está cerca
		sensor = app->physics->CreateRectangleSensor(position.x + (8 * 2), position.y + (8 * 2), 250, 250, bodyType::KINEMATIC);
		sensor->listener = this;
		sensor->ctype = ColliderType::SENSOR;

		//Collider para matar al enemigo saltando sobre él
		Kill = app->physics->CreateRectangleSensor(position.x, position.y + (20), 30, 10, bodyType::KINEMATIC);
		Kill->ctype = ColliderType::KILLFLY;

		pbody->GetPosition(WalkPosX, WalkPosY);

		alive = true;
		isDead = false;
		kill = false;

		col = false;
		deadanim = false;

	}

	if (deadanim == true) {

		if (this->currentAnimation->currentFrame < 2) {
			this->currentAnimation = &deathAnimEnemy;
			SDL_Rect die = this->currentAnimation->GetCurrentFrame();
			this->currentAnimation->Update();
			app->render->DrawTexture(texture, this->position.x-7, this->position.y-20, &die, 1.0f, NULL, NULL, NULL, flip);
		}

	}




	if (!isDead)
	{
		app->scene->player->pbody->GetPosition(p.x, p.y);
		this->pbody->GetPosition(e.x, e.y);

		this->enemy = app->map->WorldToMap(e.x, e.y);

		this->player = app->map->WorldToMap(p.x, p.y);

		SDL_Rect r = this->currentAnimation->GetCurrentFrame();

		this->currentAnimation = &idleAnimEnemy;

		if (pbody->body->GetLinearVelocity().x <= -0.1f) {

			flip = SDL_FLIP_HORIZONTAL;
		}

		if (pbody->body->GetLinearVelocity().x >= 0.1f) {

			flip = SDL_FLIP_NONE;
		}

		this->currentAnimation->Update();

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

		if (alive == true) {
			app->render->DrawTexture(texture, this->position.x-7, this->position.y-20, &r, 1.0f, NULL, NULL, NULL, flip);
		}



		if (app->physics->debug)
		{
			const DynArray<iPoint>* path = app->path->GetLastPath();
			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				SDL_Rect rect = { 0, 0, 16, 16 };

				app->render->DrawTexture(pathTileTex, pos.x, pos.y, &rect);

			}
		}

		if (kill)
		{
			app->audio->PlayFx(audio);
			alive = false;
			pbody->body->GetWorld()->DestroyBody(pbody->body);
			sensor->body->GetWorld()->DestroyBody(sensor->body);
			Kill->body->GetWorld()->DestroyBody(Kill->body);
			//app->audio->PlayFx(audio);
			isDead = true;
			deadanim = true;


		}

		if (follow) {

			LOG("FOLLOWING------------------------------------------------------------------");

			app->path->CreatePath(enemy, player);

			const DynArray<iPoint>* path = app->path->GetLastPath();

			if (path->At(1) != nullptr)
			{
				iPoint pos = app->map->MapToWorld(path->At(1)->x, path->At(1)->y);

				float32 speed = 0.05;

				if (e.y < pos.y) {
					pbody->body->SetLinearVelocity({ 0, speed * 20 });
					LOG("POS Y -");
					this->currentAnimation = &idleAnimEnemy;
				}

				if (e.x < pos.x) {
					pbody->body->ApplyForceToCenter({ speed, 0 }, true);
					LOG("POS X +");
					this->currentAnimation = &idleAnimEnemy;
				}

				if (e.y > pos.y) {
					pbody->body->ApplyForceToCenter({ 0, -speed }, true);
					LOG("POS Y +");
					this->currentAnimation = &idleAnimEnemy;
				}

				if (e.x > pos.x) {
					pbody->body->ApplyForceToCenter({ -speed, 0 }, true);
					LOG("POS X -");
					this->currentAnimation = &idleAnimEnemy;
				}
			}
		}

		b2Vec2 vec = { (float32)PIXEL_TO_METERS(position.x), (float32)PIXEL_TO_METERS(position.y) };
		b2Vec2 vec2 = { (float32)PIXEL_TO_METERS(position.x) + 0.36f, (float32)PIXEL_TO_METERS(position.y) };

		sensor->body->SetTransform(vec, 0);
		Kill->body->SetTransform(vec2, 0);
	}



	return true;
}

bool EnemyFly::CleanUp()
{
	if (!isDead) {
		pbody->body->GetWorld()->DestroyBody(pbody->body);
		sensor->body->GetWorld()->DestroyBody(sensor->body);
		Kill->body->GetWorld()->DestroyBody(Kill->body);
	}
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void EnemyFly::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		//app->path->CreatePath(enemy, player);
		if (app->path->IsWalkable(player) && app->path->IsWalkable(enemy))
		{
			follow = true;
		}
		break;
	}
}

void EnemyFly::EndContact(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		follow = false;
		pbody->body->SetLinearVelocity({ 0, 0 });
		sensor->body->SetLinearVelocity({ 0, 0 });
		Kill->body->SetLinearVelocity({ 0, 0 });
		break;
	}
}
