#include <Ogre.h>
#include <OIS/OIS.h>

#include "PauseState.h"
#include "StartState.h"
#include "PlayState.h"
#include "GameManager.h"

using namespace Ogre;
using namespace OIS;

PauseState PauseState::mPauseState;

void PauseState::enter()
{	
	mWindow = Root::getSingleton().getAutoCreatedWindow();
	mSceneMgr= Root::getSingleton().getSceneManager("main");

	mUI_Base = mOverlayMgr1->getSingleton().getByName("Overlay/Base");
	mUI_Base->show();

	mUI_PauseOverlay[0] = mOverlayMgr1->getSingleton().getByName("Overlay/Paused");
	mUI_PauseOverlay[0]->show();

	mUI_PauseOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_UP");
	mUI_PauseOverlay[1]->show();

	mUI_PauseOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Resume_UP");
	mUI_PauseOverlay[2]->show();

	mMouseOverlay = OverlayManager::getSingleton().getByName("Overlay/Cursor");
	mMouseOverlay->show();

	mContinue = true;
	keyState = false;
}

void PauseState::exit()
{
	mSceneMgr->clearScene();
	mMouseOverlay->hide();

	mUI_Base->hide();

	mUI_PauseOverlay[0]->hide();
	mUI_PauseOverlay[1]->hide();
	mUI_PauseOverlay[2]->hide();
	
}

void PauseState::pause()
{
	mMouseOverlay->hide();

	mUI_Base->hide();

	mUI_PauseOverlay[0]->hide();
	mUI_PauseOverlay[1]->hide();
	mUI_PauseOverlay[2]->hide();
	std::cout<<"PauseState pause\n";
}

void PauseState::resume()
{
	std::cout<<"PauseState resume\n";
}

bool PauseState::frameStarted(GameManager *game, const Ogre::FrameEvent &evt)
{
	return true;
}

bool PauseState::frameEnded(GameManager *game, const Ogre::FrameEvent &evt)
{
	
	return mContinue;
}

bool PauseState::keyPressed(GameManager *game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_P:
		mUI_PauseOverlay[2]->hide();
		mUI_PauseOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Resume_DOWN");
		mUI_PauseOverlay[2]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case OIS::KC_ESCAPE:
		mUI_PauseOverlay[1]->hide();
		mUI_PauseOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_DOWN");
		mUI_PauseOverlay[1]->show();
		keyState = true;
		//game->changeState(StartState::getInstance());
		break;
	}

	return true;
}

bool PauseState::keyReleased(GameManager *game, const OIS::KeyEvent &e)
{
	if( keyState )
	{
		switch(e.key)
		{
		case OIS::KC_P:
			keyState = false;
			game->popState();
			//game->changeState(PlayState::getInstance());
			break;
		case OIS::KC_ESCAPE:
			keyState = false;
			game->changeState(StartState::getInstance());
			break;
		}
	}
	return true;
}

bool PauseState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mMouseOverlay->getChild("UI_Mouse")->setPosition(e.state.X.abs -(400-18), e.state.Y.abs -(300-18));
	return true;
}


bool PauseState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	mX = e.state.X.abs -550;
	mY = e.state.Y.abs -170;
	
	mMouseState = 0;

	if( id == OIS::MB_Left )
	{	
		if( PauseState::mouseCheck( mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getLeft(), 
			mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getTop(), 
			mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getHeight(), 
			mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 1;

		if( PauseState::mouseCheck( mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getLeft(), 
			mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getTop(), 
			mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getHeight(), 
			mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 2;

		//else return true;
	}
	//else return true;

	switch( mMouseState )
	{
	case 1:
		mUI_PauseOverlay[1]->hide();
		mUI_PauseOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_DOWN");
		mUI_PauseOverlay[1]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case 2:
		mUI_PauseOverlay[2]->hide();
		mUI_PauseOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Resume_DOWN");
		mUI_PauseOverlay[2]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	default: break;
	}
	return true;
}
    
bool PauseState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if( keyState )
	{
		switch( mMouseState )
		{
		case 1:
			keyState = false;
			game->changeState(StartState::getInstance());
			break;
		case 2:
			keyState = false;
			game->popState();
			break;
		default: break;
		}
	}
	return true;
}

//bool StartState::mouseCheck( float btX, float btY, float btWidth, float btHeight, int mX, int mY )
bool PauseState::mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY )
{
	for( int i = btX; i < btWidth; i++ )
		for( int j = btY; j < btHeight; j++ )
			if( btX +i == mX && btY +j == mY ) return true;

	return false;
}