#ifndef __ENEMY_GROUND_H__
#define __ENEMY_GROUND_H__

#include "Entity.h"		
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "Render.h"
#include "PugiXml/src/pugixml.hpp"
#include "List.h"
#include "DynArray.h"

struct SDL_Texture;

struct NewPositionEnemy {
	float posX;
	float posY;
	bool t;

};

class EnemyGround : public Entity
{
public:

	EnemyGround();

	virtual ~EnemyGround();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void EnemyGround::Teleport(int x, int y);

	void Move(DynArray<iPoint>& path, float dt);

public:

	//Todo lo que se guarde en en save_game.xml ha de estar en publico para que se pueda acceder

	PhysBody* pbody;

private:

	SDL_RendererFlip rotar;

	//Animaciones
	Animation* currentAnimation;

	Animation baseAnimation;
	Animation runningAnimation;
	Animation jummpingAnimation;
	Animation on_airAnimation;
	Animation dyingAnimation;


	//L02: DONE 1: Declare player parameters
	SDL_Texture* enemyGroundTexture;
	const char* texturePath;

	//Audio paths
	const char* jumpFxPath;

	bool pathfinding;

	int jumpFxId;

	NewPositionEnemy newPosEnemy;

};

#endif // __ENEMY_GROUND_H__