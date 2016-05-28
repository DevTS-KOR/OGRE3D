#include <Ogre.h>
#include <OIS/OIS.h>

#include "CardState.h"
#include "StartState.h"
#include "PlayState.h"
#include "GameManager.h"

using namespace Ogre;
using namespace OIS;

CardState CardState::mCardState;

void CardState::enter()
{	
	mWindow = Root::getSingleton().getAutoCreatedWindow();
	mSceneMgr= Root::getSingleton().getSceneManager("main");

	mUI_Base = mOverlayMgr1->getSingleton().getByName("Overlay/Base");
	mUI_Base->show();

	mUI_CardBoxOverlay[0] = mOverlayMgr1->getSingleton().getByName("Overlay/CardBox");
	mUI_CardBoxOverlay[0]->show();

	mUI_CardBoxOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/ItemBox");
	mUI_CardBoxOverlay[1]->show();

	mMouseOverlay = OverlayManager::getSingleton().getByName("Overlay/Cursor");
	mMouseOverlay->show();

	mUI_Card = mOverlayMgr1->getSingleton().getByName("Overlay/Card_R1");
	mUI_Card->show();

	mContinue = true;
	bool keyState = false;
}

void CardState::exit()
{
	mSceneMgr->clearScene();
	mMouseOverlay->hide();

	mUI_Base->hide();
	
	mUI_CardBoxOverlay[0]->hide();
	mUI_CardBoxOverlay[1]->hide();
}

void CardState::pause()
{
	mMouseOverlay->hide();

	mUI_Base->hide();
	
	mUI_CardBoxOverlay[0]->hide();
	mUI_CardBoxOverlay[1]->hide();
	mUI_Card->hide();
	std::cout<<"CardState pause\n";
}

void CardState::resume()
{
	std::cout<<"CardState resume\n";
}

bool CardState::frameStarted(GameManager *game, const Ogre::FrameEvent &evt)
{
	return true;
}

bool CardState::frameEnded(GameManager *game, const Ogre::FrameEvent &evt)
{
	
	return mContinue;
}

bool CardState::keyPressed(GameManager *game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_ESCAPE:
		keyState = true;
		//game->changeState(PlayState::getInstance());
		break;
	}

	return true;
}

bool CardState::keyReleased(GameManager *game, const OIS::KeyEvent &e)
{
	if( keyState )
	{
		switch(e.key)
		{
		case OIS::KC_ESCAPE:
			keyState = false;
			game->popState();
			break;
		}
	}
	return true;
}

bool CardState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mUI_Card->getChild("UI_CardR1")->setPosition(e.state.X.abs -400, e.state.Y.abs -300);
	mMouseOverlay->getChild("UI_Mouse")->setPosition(e.state.X.abs -(400-18), e.state.Y.abs -(300-18));
	return true;
}


bool CardState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	//mX = e.state.X.abs -550;
	//mY = e.state.Y.abs -170;
	//
	//mMouseState = 0;

	//if( id == OIS::MB_Left )
	//{	
	//	if( PauseState::mouseCheck( mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getLeft(), 
	//		mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getTop(), 
	//		mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getHeight(), 
	//		mUI_PauseOverlay[1]->getChild("UI_MainMenu_UP")->getWidth(), mX, mY ) )
	//		
	//		mMouseState = 1;

	//	if( PauseState::mouseCheck( mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getLeft(), 
	//		mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getTop(), 
	//		mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getHeight(), 
	//		mUI_PauseOverlay[2]->getChild("UI_Resume_UP")->getWidth(), mX, mY ) )
	//		
	//		mMouseState = 2;

	//	//else return true;
	//}
	////else return true;

	//switch( mMouseState )
	//{
	//case 1:
	//	mUI_PauseOverlay[1]->hide();
	//	mUI_PauseOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/MainMenu_DOWN");
	//	mUI_PauseOverlay[1]->show();
	//	//game->changeState(PlayState::getInstance());
	//	keyState = true;
	//	break;
	//case 2:
	//	mUI_PauseOverlay[2]->hide();
	//	mUI_PauseOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Resume_DOWN");
	//	mUI_PauseOverlay[2]->show();
	//	//game->changeState(PlayState::getInstance());
	//	keyState = true;
	//	break;
	//default: break;
	//}
	return true;
}
    
bool CardState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool CardState::mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY )
{
	for( int i = btX; i < btWidth; i++ )
		for( int j = btY; j < btHeight; j++ )
			if( btX +i == mX && btY +j == mY ) return true;

	return false;
}