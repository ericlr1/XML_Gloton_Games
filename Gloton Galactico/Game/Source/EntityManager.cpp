#include "EntityManager.h"
#include "Player.h"
#include "EnemyGround.h"
#include "EnemyFly.h"

#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L02: DONE 2: Instantiate entity according to the type and add the new entoty it to the list of Entities

	switch (type)
	{

	case EntityType::PLAYER:
		entity = new Player();
		break;

	case EntityType::ENEMY_FLY:
		entity = new EnemyFly();		//CAMBIAR
		break;

	case EntityType::ENEMY_GROUND:
		entity = new EnemyGround();		//CAMBIAR
		break;

	case EntityType::ITEM:		
		entity = new Item();
		break;

	default: break;
	}

	// Created entities are added to the list
	AddEntity(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update();
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	float x = data.child("player").attribute("x").as_int();
	float y = data.child("player").attribute("y").as_int();
	int vidas = data.child("player").attribute("vidas").as_int();
	bool godMode = data.child("player").attribute("godMode").as_bool();

	
	app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(x) + 0.34f, PIXEL_TO_METERS(y) + 0.34f }, 0);			//Mirar de sumar/restar el offset al cargar
	app->scene->player->vidas = vidas;
	app->scene->player->godMode = godMode;

	if (!app->scene->enemy->isDead) {


		PosX = data.child("ground_enemy").attribute("x").as_int();
		PosY = data.child("ground_enemy").attribute("y").as_int();

		/*app->scene->fly->position.x = PosX;
		app->scene->fly->position.y = PosY;*/

		app->scene->enemy->pbody->body->SetTransform({ PIXEL_TO_METERS(PosX) + 0.34f, PIXEL_TO_METERS(PosY) + 0.32f }, 0);
	}

	if (enemy_groundLive == 0)
	{
		if (app->scene->enemy->isDead)
		{
			app->scene->enemy->col = true;
		}
	}

	if (!app->scene->enemy_fly->isDead) {


		PosX = data.child("fly_enemy").attribute("x").as_int();
		PosY = data.child("fly_enemy").attribute("y").as_int();

		/*app->scene->fly->position.x = PosX;
		app->scene->fly->position.y = PosY;*/

		app->scene->enemy_fly->pbody->body->SetTransform({ PIXEL_TO_METERS(PosX) + 0.34f, PIXEL_TO_METERS(PosY) + 0.34f }, 0);
	}

	if (flyenemyLive == 0)
	{
		if (app->scene->enemy_fly->isDead)
		{
			app->scene->enemy_fly->col = true;
		}
	}

	return true;
}

// L03: DONE 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool EntityManager::SaveState(pugi::xml_node& data)
{
	pugi::xml_node player = data.append_child("player");

	player.append_attribute("x") = app->scene->player->position.x;
	player.append_attribute("y") = app->scene->player->position.y;
	player.append_attribute("vidas") = app->scene->player->vidas;
	player.append_attribute("godMode") = app->scene->player->godMode;

	//Incluir aqui los saveState de los enemigos igual que con el player

	pugi::xml_node ground_enemy = data.append_child("ground_enemy");
	ground_enemy.append_attribute("x") = app->scene->enemy->position.x;
	ground_enemy.append_attribute("y") = app->scene->enemy->position.y;

	if (!app->scene->enemy->isDead) {
		enemy_groundLive = 0;
	}

	if (app->scene->enemy->isDead) {
		enemy_groundLive = 1;
	}

	pugi::xml_node fly_enemy = data.append_child("fly_enemy");
	fly_enemy.append_attribute("x") = app->scene->enemy_fly->position.x;
	fly_enemy.append_attribute("y") = app->scene->enemy_fly->position.y;

	if (!app->scene->enemy_fly->isDead) {
		flyenemyLive = 0;
	}

	if (app->scene->enemy_fly->isDead) {
		flyenemyLive = 1;
	}

	


	return true;
}