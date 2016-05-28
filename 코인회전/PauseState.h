#ifndef PauseState_H
#define PauseState_H

#include <Ogre.h>
#include "GameState.h"
#include "PlayState.h"

class PauseState: public GameState
{
public:
	void enter();
	void exit();

	void pause();
	void resume();

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool nextLocation(void);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e) ;
    bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id );
    bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

    bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
    bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

	//bool mouseCheck( float btX, float btY, float btWidth, float btHeight, Ogre::Real mX, Ogre::Real mY );
	bool mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY );

	static PauseState* getInstance() {return &mPauseState;}

	Ogre::OverlayManager* mOverlayMgr1;

	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;

private:
	PauseState(){}	

	static PauseState mPauseState;
	
	bool mContinue;
	bool keyState;

	//¸¶¿ì½º
	int mX, mY, mMouseState;

	Ogre::Overlay* mUI_PauseOverlay[3];
	Ogre::Overlay* mUI_Base;
	Ogre::Overlay* mMouseOverlay;

};

#endif