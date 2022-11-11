#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneIntro.h"
#include "EntityManager.h"
#include "Map.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

SceneIntro::SceneIntro() : Module()
{
	name.Create("scene");
}

// Destructor
SceneIntro::~SceneIntro()
{}

// Called before render is available
bool SceneIntro::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	

	return ret;
}

// Called before the first frame
bool SceneIntro::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	// L03: DONE: Load map
	app->map->Load();
	fons = app->tex->Load("Assets/Maps/Tiles/Assets/Background_3.png");
	menu = app->tex->Load("Assets/Textures/menu.png");
	Game_Over = app->tex->Load("Assets/Textures/game_over.png");
	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	playing = false;
	game_over = false;


	app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool SceneIntro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneIntro::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		
		app->fadetoblack->fadetoblack((Module*)app->sceneIntro, (Module*)app->scene, 60);
		app->entityManager->active = true;
	}

	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{

		game_over = true;
	}

		//App.fadetoblack(scene)
	app->render->DrawTexture(fons, 0, 0);

	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	if (game_over == false && playing == false)
	{
		app->render->DrawTexture(menu, 0, 0);
	}

	if (game_over == true)
	{
		app->render->DrawTexture(Game_Over, 0, 0);
	}


	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool SceneIntro::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneIntro::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
