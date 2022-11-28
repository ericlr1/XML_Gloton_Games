#include "FadeToBlack.h"

#include "App.h"
#include "Render.h"

#include "SDL/include/SDL_render.h"
#include "Log.h"

FadeToBlack::FadeToBlack()
{
	screenRect = { 0,0, 2000, 2000 };
}

FadeToBlack::~FadeToBlack()
{
}

bool FadeToBlack::Start()
{
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update(float dt)
{
	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			// TODO 1: Enable / disable the modules received when FadeToBlacks() gets called
			moduleToDisable->active = false;
			moduleToEnable->active = true;

			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate()
{
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	
		

	

	return true;
}

bool FadeToBlack::CleanUp()
{
	return false;
}

bool FadeToBlack::fadetoblack(Module* toDisable, Module* toEnable, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if (currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		// TODO 1: We need to keep track of the modules received in FadeToBlack(...)
		moduleToDisable = toDisable;
		moduleToEnable = toEnable;

		ret = true;
	}

	return ret;
}

