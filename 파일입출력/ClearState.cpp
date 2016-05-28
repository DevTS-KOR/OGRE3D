#include <Ogre.h>
#include <OIS/OIS.h>

#include "MenuState.h"
#include "PlayState.h"
#include "ClearState.h"

using namespace Ogre;

ClearState ClearState::mClearState;

void ClearState::enter(void)
{
	mContinue = true;

	mGameOverClearlay = OverlayManager::getSingleton().getByName("Overlay/GameClear");
	mGameOverClearlay->show();
}

void ClearState::exit(void)
{
	mGameOverClearlay->hide();
}

void ClearState::pause(void)
{
	std::cout << "ClearState pause\n";
}

void ClearState::resume(void)
{
	std::cout << "ClearState resume\n";
}

bool ClearState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	return true;
}

bool ClearState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return mContinue;
}

bool ClearState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_SPACE:
		game->changeState(MenuState::getInstance());
		break;
	case OIS::KC_ESCAPE:
		game->changeState(MenuState::getInstance());
		break;
	case OIS::KC_DOWN:
		break;
	case OIS::KC_UP:
		break;
	}
	return true;
}