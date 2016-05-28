#include <Ogre.h>
#include <OIS/OIS.h>

#include "EndState.h"
#include "PlayState.h"
#include "GameManager.h"
#include "StartState.h"

using namespace Ogre;
using namespace OIS;

EndState EndState::mEndState;

void EndState::enter()
{	
	//mGUI = new BetaGUI::GUI("Font/Magic36", 50);
	//mGUI ->injectMouse(<pointer x>, <pointer y>, true); // Mouse key down.
		
	mUI_EndOverlay[0] = mOverlayMgr1->getSingleton().getByName("Overlay/Gameover");
	mUI_EndOverlay[0]->show();

	mUI_EndOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_UP");
	mUI_EndOverlay[1]->show();

	mUI_EndOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Retry_UP");
	mUI_EndOverlay[2]->show();

	mEndPointOverlay = mOverlayMgr1->getSingleton().getByName("Overlay/EndPoint");
	//mEndPointOverlay->show();

	mMouseOverlay = OverlayManager::getSingleton().getByName("Overlay/Cursor");
	mMouseOverlay->show();

	//mPointOverlay = OverlayManager::getSingleton().getByName("Overlay/Point");
	//mPointOverlay->show();


	mContinue = true;
	keyState = false;
}

void EndState::exit()
{
	mMouseOverlay->hide();
	//mPointOverlay->hide();
	mEndPointOverlay->hide();

	mUI_EndOverlay[0]->hide();
	mUI_EndOverlay[1]->hide();
	mUI_EndOverlay[2]->hide();
}

void EndState::pause()
{
	std::cout<<"EndState pause\n";
}

void EndState::resume()
{
	std::cout<<"EndState resume\n";
}

bool EndState::frameStarted(GameManager *game, const Ogre::FrameEvent &evt)
{
	//mUI_EndOverlay->show();  
	
	return true;
}

bool EndState::frameEnded(GameManager *game, const Ogre::FrameEvent &evt)
{
	
	return mContinue;
}

bool EndState::keyPressed(GameManager *game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_P:
		mUI_EndOverlay[2]->hide();
		mUI_EndOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Retry_DOWN");
		mUI_EndOverlay[2]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case OIS::KC_ESCAPE:
		mUI_EndOverlay[1]->hide();
		mUI_EndOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_DOWN");
		mUI_EndOverlay[1]->show();
		//mContinue = false;
		keyState = true;
		break;
	}

	return true;
}

bool EndState::keyReleased(GameManager *game, const OIS::KeyEvent &e)
{
	if( keyState )
	{
		switch(e.key)
		{
		case OIS::KC_P:
			keyState = false;
			game->changeState(PlayState::getInstance());
			break;
		case OIS::KC_ESCAPE:
			keyState = false;
			game->changeState(StartState::getInstance());
			break;
		}
	}
	return true;
}

bool EndState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mMouseOverlay->getChild("UI_Mouse")->setPosition(e.state.X.abs -(400-18), e.state.Y.abs -(300-18));
	return true;
}


bool EndState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	mX = e.state.X.abs -500;
	mY = e.state.Y.abs -170;
	
	mMouseState = 0;

	if( id == OIS::MB_Left )
	{	
		if( EndState::mouseCheck( mUI_EndOverlay[1]->getChild("UI_MainMenu_UP")->getLeft(), 
			mUI_EndOverlay[1]->getChild("UI_MainMenu_UP")->getTop(), 
			mUI_EndOverlay[1]->getChild("UI_MainMenu_UP")->getHeight(), 
			mUI_EndOverlay[1]->getChild("UI_MainMenu_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 1;

		if( EndState::mouseCheck( mUI_EndOverlay[2]->getChild("UI_Retry_UP")->getLeft(), 
			mUI_EndOverlay[2]->getChild("UI_Retry_UP")->getTop(), 
			mUI_EndOverlay[2]->getChild("UI_Retry_UP")->getHeight(), 
			mUI_EndOverlay[2]->getChild("UI_Retry_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 2;

		//else return true;
	}
	//else return true;

	switch( mMouseState )
	{
	case 1:
		mUI_EndOverlay[1]->hide();
		mUI_EndOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_DOWN");
		mUI_EndOverlay[1]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case 2:
		mUI_EndOverlay[2]->hide();
		mUI_EndOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Retry_DOWN");
		mUI_EndOverlay[2]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	default: break;
	}
	return true;
}
    
bool EndState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
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
			game->changeState(PlayState::getInstance());
			break;
		default: break;
		}
	}
	return true;
}

bool EndState::mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY )
{
	for( int i = btX; i < btWidth; i++ )
		for( int j = btY; j < btHeight; j++ )
			if( btX +i == mX && btY +j == mY ) return true;

	return false;
}