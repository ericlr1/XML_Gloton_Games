#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "Render.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}*/

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");
	enemy_1 = (EnemyGround*)app->entityManager->CreateEntity(EntityType::ENEMY_GROUND);
	enemy_1->parameters = config.child("enemy_ground");

	

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	

	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	fondo = app->tex->Load("Assets/Maps/Tiles/Assets/Background_3.png");
	planeta_1 = app->tex->Load("Assets/Maps/Tiles/Assets/planeta_1.png");
	planeta_2 = app->tex->Load("Assets/Maps/Tiles/Assets/planeta_2.png");
	planeta_3 = app->tex->Load("Assets/Maps/Tiles/Assets/planeta_3.png");
	meteorito = app->tex->Load("Assets/Maps/Tiles/Assets/meteo.png");
	musicId = app->audio->LoadFx("Assets/Audio/Space Music Pack/Space Music Pack/slow-travel.wav");

	// L03: DONE: Load map
	app->map->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	app->audio->PlayMusic("Assets/Audio/Space Music Pack/Space Music Pack/loading.wav", 1.0f);
	
	app->render->SetCameraBounds({ 0, 0, 2800, 750 });

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (app->entityManager->active == false)
	{
		app->entityManager->active = true;
	}

	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 3;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 3;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 3;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 3;


	

	//Parallax
	app->render->DrawTexture(fondo, app->render->camera.x*0.2, 0);

	app->render->DrawTexture(meteorito, app->render->camera.x*0.2 +600, 250);
	app->render->DrawTexture(planeta_2, app->render->camera.x * 0.5 + 1000 , 200);
	app->render->DrawTexture(planeta_1, app->render->camera.x * 0.2 + 950, 250);
	app->render->DrawTexture(planeta_3, app->render->camera.x * 0.1 + 1500 , 250);
	app->render->DrawTexture(planeta_1, app->render->camera.x * 0.2 + 1900, 421);
	app->render->DrawTexture(meteorito, app->render->camera.x * 0.7 + 2312, 120);
	app->render->DrawTexture(planeta_1, app->render->camera.x * 0.3 + 2600, 100);
	app->render->DrawTexture(planeta_2, app->render->camera.x * 0.1 + 2300, 321);
	// Draw map
	
	app->map->Draw();
	

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	

	return true;
}
