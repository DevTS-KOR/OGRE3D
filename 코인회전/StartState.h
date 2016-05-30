#ifndef StartState_H
#define StartState_H

#include<Ogre.h>
#include "GameState.h"

class StartState: public GameState
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

	static StartState* getInstance() {return &mStartState;}

	bool mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY );
	//bool mouseCheck( float btX, float btY, float btWidth, float btHeight, Ogre::Real mX, Ogre::Real mY );

	Ogre::OverlayManager* mOverlayMgr1;

private:
	StartState(){}	

	static StartState mStartState;
	
	bool mContinue;
	bool keyState;

	//¸¶¿ì½º
	int mX, mY, mMouseState;

	OIS::Mouse *mMouse;

	Ogre::RenderWindow* mWindow;

	Ogre::Overlay* mUI_MainOverlay[5];
	Ogre::Overlay* mMouseOverlay;
    Ogre::OverlayElement* mStartMsg;
};

#endif