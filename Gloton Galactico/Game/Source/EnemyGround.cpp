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
#include "SceneIntro.h"
#include "EntityManager.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Player.h"

EnemyGround::EnemyGround() : Entity(EntityType::ENEMY_GROUND)
{
	name.Create("Enemies");

	idleAnimEnemy.PushBack({ 0,0,16,16 });
	idleAnimEnemy.speed = 0.1f;
	idleAnimEnemy.loop = true;

	movingAnimEnemy.PushBack({ 0,0,16,16 });
	movingAnimEnemy.speed = 0.2f;
	movingAnimEnemy.loop = true;

	deathAnimEnemy.PushBack({ 0,0,16,16 });
	deathAnimEnemy.speed = 0.1f;
	deathAnimEnemy.loop = true;

}

EnemyGround::~EnemyGround() {

}

bool EnemyGround::Awake() {

	position = { 160, 560 };
	texturePath = parameters.child("enemy_ground").attribute("texturepath").as_string();

	return true;
}

bool EnemyGround::Start()
{
	texture = app->tex->Load(texturePath);
	pathTileTex = app->tex->Load("Assets/Maps/MapMetadata.png");

	currentAnimation = &idleAnimEnemy;

	return true;
}

bool EnemyGround::Update()
{
	if (col)
	{
		pbody = app->physics->CreateCircle(position.x, position.y, 7.5, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

		LOG("PosX: %d", position.x + (8 * 2));
		LOG("PosY: %d", position.y + (8 * 2));

		//Sensor de si el player está cerca
		sensor = app->physics->CreateRectangleSensor(position.x + (8 * 2), position.y + (8 * 2), 250, 100, bodyType::KINEMATIC);
		sensor->listener = this;
		sensor->ctype = ColliderType::SENSOR;

		//Collider para matar al enemigo saltando sobre él
		Kill = app->physics->CreateRectangleSensor(position.x, position.y + (20), 10, 20, bodyType::KINEMATIC);
		Kill->ctype = ColliderType::KILLWALK;

		pbody->GetPosition(WalkPosX, WalkPosY);

		alive = true;
		isDead = false;
		kill = false;

		col = false;
		deadanim = false;
		currentAnimation = &deathAnimEnemy;
		currentAnimation->Reset();
	}

	if (deadanim == true) {

		
	}

	if (!isDead)
	{
		app->scene->player->pbody->GetPosition(p.x, p.y);
		this->pbody->GetPosition(e.x, e.y);

		this->enemy = app->map->WorldToMap(e.x, e.y);

		this->player = app->map->WorldToMap(p.x, p.y);

		SDL_Rect walk = currentAnimation->GetCurrentFrame();

		currentAnimation = &idleAnimEnemy;

		if (pbody->body->GetLinearVelocity().x <= -0.1f) {

			flip = SDL_FLIP_HORIZONTAL;
			currentAnimation = &movingAnimEnemy;
		}

		if (pbody->body->GetLinearVelocity().x >= 0.1f) {

			flip = SDL_FLIP_NONE;
			currentAnimation = &movingAnimEnemy;
		}

		currentAnimation->Update();

		position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
		position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

		if (alive == true) {
			app->render->DrawTexture(texture, position.x - 12, position.y - 11, &walk, flip);
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


				float32 speed = 0.6f;

				/*if (e.y < pos.y) {
					pbody->body->SetLinearVelocity({ 0, speed });
				}*/

				if (e.x < pos.x) {
					LOG("-");
					pbody->body->SetLinearVelocity({ speed, 5.0f });
				}

				//if (e.y > pos.y) {
				//	//pbody->body->ApplyForce({0, -4}, {(float32)position.x, (float32)position.y}, true);
				//	pbody->body->SetLinearVelocity({ 0, -speed });
				//}

				if (e.x > pos.x) {
					LOG("+");
					pbody->body->SetLinearVelocity({ -speed, 5.0f });
				}
			}
		}

		b2Vec2 vec = { (float32)PIXEL_TO_METERS(position.x), (float32)PIXEL_TO_METERS(position.y) };
		b2Vec2 vec2 = { (float32)PIXEL_TO_METERS(position.x), (float32)PIXEL_TO_METERS(position.y - 0.25) };

		sensor->body->SetTransform(vec, 0);
		Kill->body->SetTransform(vec2, 0);
	}

	return true;
}

bool EnemyGround::CleanUp()
{
	if (!isDead) {
		pbody->body->GetWorld()->DestroyBody(pbody->body);
		sensor->body->GetWorld()->DestroyBody(sensor->body);
		Kill->body->GetWorld()->DestroyBody(Kill->body);
	}
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void EnemyGround::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		//app->path->CreatePath(enemy, player);
		if (app->path->IsWalkable(player) && app->path->IsWalkable(enemy))
		{
			follow = true;
		}
		//Follow();
		break;
	}
}

void EnemyGround::EndContact(PhysBody* physA, PhysBody* physB) {
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

void EnemyGround::Follow()
{
	const DynArray<iPoint>* path = app->path->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		iPoint epos = app->map->MapToWorld(e.x, e.y + 4);


		LOG("e.y: %d", epos.y);
		LOG("pos.y: %d", pos.y);

		float32 speed = 1.5f;

		if (epos.y < pos.y) {
			pbody->body->SetLinearVelocity({ 0, speed });
		}

		if (epos.x < pos.x) {
			pbody->body->SetLinearVelocity({ speed, 0 });

		}

		if (epos.y > pos.y) {
			//pbody->body->ApplyForce({0, -4}, {(float32)position.x, (float32)position.y}, true);
			pbody->body->SetLinearVelocity({ 0, -speed });
		}

		if (epos.x > pos.x) {
			pbody->body->SetLinearVelocity({ -speed, 0 });

		}
	}
}

