#pragma once

#include "GameState.h"

class MenuState : public GameState
{
public:
	void enter();
	void exit();
	void pause();
	void resume();
	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);
	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e)
	{ return true; }
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id )
	{ return true; }
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
	{ return true; }
	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e) { return true; }
	static MenuState* getInstance() { return &mMenuState; }

private:
	static MenuState mMenuState;

	bool mContinue;
	enum{START_GAME=0, OPTION=1, EXIT_GAME=2};
	int mMenuSelection;
	bool mInput;

	Ogre::Overlay* mMenuOverlay1;
	Ogre::Overlay* mMenuOverlay2;
	Ogre::Overlay* mMenuOverlay3;
};
