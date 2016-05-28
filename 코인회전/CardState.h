#ifndef CardState_H
#define CardState_H

#include <Ogre.h>
#include "GameState.h"

class CardState: public GameState
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

	static CardState* getInstance() {return &mCardState;}

	Ogre::OverlayManager* mOverlayMgr1;

	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;

private:
	CardState(){}	

	static CardState mCardState;
	
	bool mContinue;
	bool keyState;
	
	//¸¶¿ì½º
	int mX, mY, mMouseState;
	
	Ogre::Overlay* mUI_CardBoxOverlay[3];
	Ogre::Overlay* mMouseOverlay;
	Ogre::Overlay* mUI_Card;
	Ogre::Overlay* mUI_Base;

};

#endif