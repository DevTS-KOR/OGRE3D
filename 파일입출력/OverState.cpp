#include <Ogre.h>
#include <OIS/OIS.h>

#include "MenuState.h"
#include "PlayState.h"
#include "OverState.h"

using namespace Ogre;

OverState OverState::mOverState;

void OverState::enter(void)
{
	mContinue = true;

	mGameOverOverlay = OverlayManager::getSingleton().getByName("Overlay/GameOver");
	mGameOverOverlay->show();
}

void OverState::exit(void)
{
	mGameOverOverlay->hide();
}

void OverState::pause(void)
{
	std::cout << "OverState pause\n";
}

void OverState::resume(void)
{
	std::cout << "OverState resume\n";
}

bool OverState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	return true;
}

bool OverState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return mContinue;
}

bool OverState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
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