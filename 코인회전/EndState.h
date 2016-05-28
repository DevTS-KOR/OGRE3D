#ifndef EndState_H
#define EndState_H

#include<Ogre.h>
#include "GameState.h"

class EndState: public GameState
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

	bool mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY );

	static EndState* getInstance() {return &mEndState;}

	Ogre::OverlayManager* mOverlayMgr1;

private:
	EndState(){}	

	static EndState mEndState;
	
	bool mContinue;
	bool keyState;

	//¸¶¿ì½º
	int mX, mY, mMouseState;

	OIS::Mouse *mMouse;

	Ogre::RenderWindow* mWindow;

	Ogre::Overlay* mUI_EndOverlay[3];
	Ogre::Overlay* mMouseOverlay;
	Ogre::Overlay* mPointOverlay;
	Ogre::Overlay* mEndPointOverlay;
    Ogre::OverlayElement* mStartMsg;
};

#endif