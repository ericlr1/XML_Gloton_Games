#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "Render.h"

struct SDL_Texture;

struct NewPosition {
	float posX;
	float posY;
	bool t;

};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void CameraMove();

	void Player::Teleport(int x, int y);


public:

	//Todo lo que se guarde en en save_game.xml ha de estar en publico para que se pueda acceder

	PhysBody* pbody;
	
	bool godMode;

	int vidas;

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
	SDL_Texture* playerTexture;
	const char* texturePath;

	SDL_Texture* vidaTexture;
	const char* vidaPath;

	int pickCoinFxId;
	int jumpFxId;
	int deathFxId;

	int numJumps;
	bool is_jumping;
	bool on_floor;

	float salto;

	int speed;

	NewPosition newPos;

};

#endif // __PLAYER_H__