#include "Player.h"
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

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	//Pushback animation - Idle
	baseAnimation.PushBack({ 0, 100, 50, 50 });

	baseAnimation.loop;
	baseAnimation.speed = 0.05f;


	//Pushback animation - Run
	runningAnimation.PushBack({ 0, 0, 50, 50});
	runningAnimation.PushBack({ 50, 0, 50, 50});
	runningAnimation.PushBack({ 100, 0, 50, 50});
	runningAnimation.PushBack({ 0, 50, 50, 50});
	runningAnimation.PushBack({ 50, 50, 50, 50});
	runningAnimation.PushBack({ 100, 50, 50, 50});

	runningAnimation.loop;
	runningAnimation.speed = 0.1f;

	//Pushback animation - Jumping
	jummpingAnimation.PushBack({ 0, 100, 50, 50});
	jummpingAnimation.PushBack({ 50, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});		//Repetido tantas veces por si un salto es muy alto
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});
	jummpingAnimation.PushBack({ 100, 100, 50, 50});

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

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());

	//L02: DONE 5: Get Player parameters from XML
	//position.x = parameters.attribute("x").as_int();
	//position.y = parameters.attribute("y").as_int();
	speed = parameters.attribute("speed").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	vidaPath = parameters.attribute("vidapath").as_string();
	hitFxPath = parameters.attribute("hitFxPath").as_string();
	jumpFxPath = parameters.attribute("jumpFxPath").as_string();

	return true;
}

bool Player::Start() {

	//initilize textures
	playerTexture = app->tex->Load(texturePath);
	vidaTexture = app->tex->Load(vidaPath);

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y+200, 12, bodyType::DYNAMIC);

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
	deathFxId = app->audio->LoadFx(hitFxPath);

	numJumps = 0;
	vidas = 3;
	godMode = false;

	is_dead = false;
	death_timmer = 40;

	currentAnimation = &baseAnimation;
	
	return true;
}

bool Player::Update()
{
	currentAnimation->Update();
	app->sceneIntro->playing = true;

	if (vidas == 0)
	{
		//Acabar la partida
		LOG("VIDAS = 0");
		Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
		app->sceneIntro->game_over = true;		//La camara se queda en la posición en la que estaba - falta fixear
		app->entityManager->active = false;
		app->fadetoblack->fadetoblack((Module*)app->scene, (Module*)app->sceneIntro, 60);
		
		
	}
	
	
	// L07 DONE 5: Add physics to the player - updated player position using physics
	
	b2Vec2 vel = b2Vec2(0, -GRAVITY_Y);

	//Controles de debug
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
		numJumps = 0;

	}
	
	//Provisional para bajar/subir las vidas
	if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
		is_dead = true;
		vidas--;
	}
	
	if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		vidas += 1;
	}
	 
	 
	if (godMode == false)
	{
		//L02: DONE 4: modify the position of the player using arrow keys and render the texture
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			//
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel = b2Vec2(-speed, -GRAVITY_Y);
			rotar = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			currentAnimation = &runningAnimation;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel = b2Vec2(speed, -GRAVITY_Y);
			rotar = SDL_RendererFlip::SDL_FLIP_NONE;
			currentAnimation = &runningAnimation;
		}

		if (numJumps < 2)
		{
			//Salto
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

			
				currentAnimation = &jummpingAnimation;

				//Fuerza de salto
				salto = -30.0;

				on_floor = false;
				numJumps++;
				app->audio->PlayFx(jumpFxId);
			}
		}

		if (salto < 0.0)
		{
			vel.y = salto;
			salto+=3.5;
		}

		if (on_floor)
		{
			numJumps = 0;
		}
	}
	
	if (godMode == true)
	{
		//L02: DONE 4: modify the position of the player using arrow keys and render the texture
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			//
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel = b2Vec2(-speed, -GRAVITY_Y);
			rotar = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			currentAnimation = &runningAnimation;
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel = b2Vec2(speed, -GRAVITY_Y);
			rotar = SDL_RendererFlip::SDL_FLIP_NONE;
			currentAnimation = &runningAnimation;
		}

		if (numJumps < 2)
		{
			//Salto
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {


				currentAnimation = &jummpingAnimation;

				//Fuerza de salto
				salto = -30.0;

				on_floor = false;
				numJumps++;
				app->audio->PlayFx(jumpFxId);
			}
		}

		if (salto < 0.0)
		{
			vel.y = salto;
			salto += 3.5;
		}

		numJumps = 0;
		
	}
	
	// Comprobaciones de las animaciones

	if (!on_floor)
	{
		currentAnimation = &jummpingAnimation;
	}

	if (on_floor && app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
	{
		currentAnimation = &baseAnimation;
	}


	//Comprobación de la muerte
	if (is_dead)
	{
		
		if (death_timmer >= 0)
		{
			dyingAnimation.Reset();
			//cout << "IS DEAD ";
			currentAnimation = &dyingAnimation;
			--death_timmer;
		}
		else
		{
			is_dead = false;
			death_timmer = 40;
			Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());

			if (vidas == 1)
			{
				vidas = 0;
			}
			if (vidas == 2)
			{
				vidas = 1;
			}
			if (vidas == 3)
			{
				vidas = 2;
			}
		}
	}

	//TP
	if (newPos.t == true)
	{
		b2Vec2 resetPos = b2Vec2(PIXEL_TO_METERS(newPos.posX), PIXEL_TO_METERS(newPos.posY));
		pbody->body->SetTransform(resetPos, 0);

		newPos.t = false;
	}

	//Set the velocity of the pbody of the player
	pbody->body->SetLinearVelocity(vel);

	//Update player position in pixels
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	//app->render->DrawTexture(texture, position.x, position.y);
	CameraMove();

	app->render->FitCameraInsideBounds();

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(playerTexture, -7+position.x, -20+position.y, &rect, 1.0f, NULL, NULL, NULL, rotar);

	//UI
	for (int i = 0; i < vidas; i++)
	{
		//app->render->Blit(App->UI->iconoVida, App->render->GetCameraCenterX() - 100 + (9 * i), App->render->GetCameraCenterY() + 120, NULL, 1.0, false);
		app->render->DrawTexture(vidaTexture, -app->render->camera.x + 50 + ( 35*i), -app->render->camera.y + 50);
	}

	//Debug controls
	
	//Crear a un Ground Enemy
	if (app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{

	}
	//Empezar desde el inicio del nivel 1
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
	}
	//Empezar desde el inicio del nivel 2
	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
	}
	//Empezar desde el inicio del nivel actual
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		if (!lv2)
		{
			//Tp al inicio del nivel 1
			Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
		}
		else
		{
			//Tp al inicio del nivel 2
			Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		Shoot();
	}
	if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		app->scene->enemy->follow = true;
	}

	if (bala != NULL)
	{
		//app->render->DrawTexture(playerTexture,  this->bala->body->GetPosition().x, this->bala->body->GetPosition().y*30);
	}
	

	return true;
}

bool Player::CleanUp()
{
	return true;
}



// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision
	
	switch (physB->ctype)
	{
		
		case ColliderType::PLATFORM:
			LOG("Collision PLATFORM");
			on_floor = true;
			break;
		case ColliderType::DEATH:
			LOG("Collision DEATH");
			app->audio->PlayFx(deathFxId);
			if (physB->body->GetWorldCenter().y + 32 < position.y) //Comprobación de que el collider está por debajo, es decir es el suelo y no el techo
			{
				on_floor = true;
			}
			if (godMode == false)
			{
				is_dead = true;
			}
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		case ColliderType::WIN:
			LOG("Collision WIN");
			app->fadetoblack->fadetoblack((Module*)app->scene, (Module*)app->scene, 60);
			Teleport(parameters.attribute("x").as_int(), parameters.attribute("y").as_int());
			lv2 = true;
			break;
		case ColliderType::KILLWALK:		//Collider para matar al enemigo
			app->scene->enemy->kill = true;
			app->scene->enemy->deadanim = true;
			break;
			
	}
	
}

void Player::Teleport(int x, int y)
{
	newPos.posX = x;
	newPos.posY = y;
	newPos.t = true;
}

void Player::CameraMove()
{
	iPoint camera_center = app->render->GetCameraCenter();

	int mov = 100;


	int despla_d = camera_center.x - mov;
	int despla_e = camera_center.x + mov;

	if (-position.x < despla_d)
	{	
		int total_despla = despla_d + position.x;
		app->render->camera.x -= total_despla;
	}

	if (-position.x > despla_e)
	{
		int total_despla = position.x + despla_e;
		app->render->camera.x -= total_despla;
	}

}

void Player::Shoot()
{
	bool r;
	if (rotar)
	{
		r = false;
	}
	else
	{
		r = true;
	}

	if (r == true)
	{
		//Disparar a la derecha
		LOG("DISPARO DERECHA-----------------");
		
		bala = app->physics->CreateCircle(position.x + 16, position.y + 10, 3, DYNAMIC);
		bala->ctype = ColliderType::BULLET;
		
		bala->body->SetLinearVelocity({ 10, 0 });
	}
	else
	{
		//Disparar a la izquierda
		LOG("DISPARO IZQUIERDA-----------------");

		bala = app->physics->CreateCircle(position.x + 16, position.y + 10, 3, DYNAMIC);
		bala->ctype = ColliderType::BULLET;
		app->render->DrawTexture(playerTexture, bala->body->GetPosition().x, bala->body->GetPosition().y);
		bala->body->SetLinearVelocity({ -10, 0 });

	}
}
