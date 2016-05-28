#include <Ogre.h>
#include <OIS/OIS.h>

#include "MenuState.h"
#include "PlayState.h"
#include "TitleState.h"

using namespace Ogre;

MenuState MenuState::mMenuState;

void MenuState::enter(void)
{
	mInput = false;
	mMenuSelection = START_GAME;
	mContinue = true;
	mMenuOverlay1 = OverlayManager::getSingleton().getByName("Overlay/Menu1");
	mMenuOverlay1->show();
	mMenuOverlay2 = OverlayManager::getSingleton().getByName("Overlay/Menu2");
	mMenuOverlay2->hide();
	mMenuOverlay3 = OverlayManager::getSingleton().getByName("Overlay/Menu3");
	mMenuOverlay3->hide();
}

void MenuState::exit(void)
{
	mMenuOverlay1->hide();
	mMenuOverlay2->hide();
	mMenuOverlay3->hide();
}

void MenuState::pause(void)
{
	std::cout << "MenuState pause\n";
}

void MenuState::resume(void)
{
	std::cout << "MenuState resume\n";
}

bool MenuState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	if(mInput)
	{
		if(mMenuSelection == START_GAME)
		{
			game->changeState(PlayState::getInstance());
		}
		else if(mMenuSelection == OPTION)
		{
		}
		else
		{
			return false;
		}
	}
	mInput = false;

	return true;
}

bool MenuState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return mContinue;
}

bool MenuState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_SPACE:
		mInput = true;
		break;
	case OIS::KC_ESCAPE:
//		game->changeState(TitleState::getInstance());
		break;
	case OIS::KC_DOWN:
		if(mMenuSelection < EXIT_GAME)
			mMenuSelection += 1;
		break;
	case OIS::KC_UP:
		if(mMenuSelection > START_GAME)
			mMenuSelection -= 1;
		break;
	}
	if(!mInput)
	{
		if(mMenuSelection == START_GAME)
		{
			mMenuOverlay1->show();
			mMenuOverlay2->hide();
			mMenuOverlay3->hide();
		}
		else if(mMenuSelection == OPTION)
		{
			mMenuOverlay1->hide();
			mMenuOverlay2->show();
			mMenuOverlay3->hide();
		}
		else
		{
			mMenuOverlay1->hide();
			mMenuOverlay2->hide();
			mMenuOverlay3->show();
		}
	}
	return true;
}