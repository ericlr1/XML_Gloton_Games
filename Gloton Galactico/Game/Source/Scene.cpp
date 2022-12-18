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
#include "Pathfinding.h"

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

	//Generar todos los enemigos
	/*for (pugi::xml_node itemNode = config.child("enemy_ground"); itemNode; itemNode = itemNode.next_sibling("enemy_ground"))
	{
		EnemyGround* enemy = (EnemyGround*)app->entityManager->CreateEntity(EntityType::ENEMY_GROUND);
		enemy->parameters = itemNode;
	}*/

	enemy = (EnemyGround*)app->entityManager->CreateEntity(EntityType::ENEMY_GROUND);
	enemy->parameters = config.child("enemy_ground");

	enemy_fly = (EnemyFly*)app->entityManager->CreateEntity(EntityType::ENEMY_FLY);
	enemy_fly->parameters = config.child("enemy_fly");

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");	

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
	bool retLoad = app->map->Load();

	// L12 Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->path->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

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


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (app->physics->debug)
	{


		// Texture to highligh mouse position 
		mouseTileTex = app->tex->Load("Assets/Maps/path_square.png");

		// Texture to show path origin 
		originTex = app->tex->Load("Assets/Maps/x_square.png");

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
	
	
	//Debug draws - IA
	if (app->physics->debug)
	{
		app->render->DrawRectangle({player->position.x - 94, player->position.y - 94, 200, 200}, 188, 0, 0, 100, false, true);
	}


	//Updates de los enemigos

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (app->physics->debug)
	{
		//Pathfinding with the mouse
		// L08: DONE 3: Test World to map method
		int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);

		iPoint mouseTile = iPoint(0, 0);

		mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y);


		//Convert again the tile coordinates to world coordinates to render the texture of the tile
		iPoint highlightedTileWorld = app->map->MapToWorld(mouseTile.x, mouseTile.y);
		app->render->DrawTexture(mouseTileTex, highlightedTileWorld.x, highlightedTileWorld.y);

		//Test compute path function
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			if (originSelected == true)
			{
				app->path->CreatePath(origin, mouseTile);
				originSelected = false;
			}
			else
			{
				origin = mouseTile;
				originSelected = true;
				app->path->ClearLastPath();
			}
		}

		// L12: Get the latest calculated path and draw
		const DynArray<iPoint>* path = app->path->GetLastPath();
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
		}

		// L12: Debug pathfinding
		iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
		app->render->DrawTexture(originTex, originScreen.x, originScreen.y);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
