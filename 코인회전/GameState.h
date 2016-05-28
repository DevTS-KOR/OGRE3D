#ifndef GameState_H
#define GameState_H

#include <Ogre.h>
#include "GameManager.h"

class GameState
{
public:
	virtual void enter() = 0;
	virtual void exit() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt) = 0;
	virtual bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt) = 0;
	
	
    virtual bool mouseMoved(GameManager* game, const OIS::MouseEvent &e) = 0;
    virtual bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id ) = 0;
    virtual bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id ) = 0;

    virtual bool keyPressed(GameManager* game, const OIS::KeyEvent &e) = 0;
    virtual bool keyReleased(GameManager* game, const OIS::KeyEvent &e) = 0;


	void changeState(GameManager* game, GameState* state)
	{
		game->changeState(state);
	}
protected:
	GameState() { }
};

#endif
