#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "EnemyGround.h"
#include "EnemyFly.h"
#include "Item.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;
	EnemyGround* enemy;
	EnemyFly* enemy_fly;

private:
	SDL_Texture* img;
	SDL_Texture* fondo;
	SDL_Texture* meteorito;
	SDL_Texture* planeta_1;
	SDL_Texture* planeta_2;
	SDL_Texture* planeta_3;

	int musicId;

	SDL_Texture* mouseTileTex = nullptr;
	SDL_Texture* originTex = nullptr;

	// L12: Debug pathfing
	iPoint origin;
	bool originSelected = false;

};

#endif // __SCENE_H__