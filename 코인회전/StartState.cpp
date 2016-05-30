#include <Ogre.h>
#include <OIS/OIS.h>

#include "StartState.h"
#include "PlayState.h"
#include "GameManager.h"

using namespace Ogre;
using namespace OIS;

StartState StartState::mStartState;

void StartState::enter()
{	
	//mGUI = new BetaGUI::GUI("Font/Magic36", 50);
	//mGUI ->injectMouse(<pointer x>, <pointer y>, true); // Mouse key down.

	mUI_MainOverlay[0] = mOverlayMgr1->getSingleton().getByName("Overlay/MainTitle");
	mUI_MainOverlay[0]->show();

	mUI_MainOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/Play_UP");
	mUI_MainOverlay[1]->show();

	mUI_MainOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Option_UP");
	mUI_MainOverlay[2]->show();

	mUI_MainOverlay[3] = mOverlayMgr1->getSingleton().getByName("Overlay/Credits_UP");
	mUI_MainOverlay[3]->show();

	mUI_MainOverlay[4] = mOverlayMgr1->getSingleton().getByName("Overlay/Exit_UP");
	mUI_MainOverlay[4]->show();

	mMouseOverlay = OverlayManager::getSingleton().getByName("Overlay/Cursor");
	mMouseOverlay->show();

	mContinue = true;
	keyState = false;
}

void StartState::exit()
{
	mMouseOverlay->hide();

	mUI_MainOverlay[0]->hide();
	mUI_MainOverlay[1]->hide();
	mUI_MainOverlay[2]->hide();
	mUI_MainOverlay[3]->hide();
	mUI_MainOverlay[4]->hide();
}

void StartState::pause()
{
	std::cout<<"StartState pause\n";
}

void StartState::resume()
{
	std::cout<<"StartState resume\n";
}

bool StartState::frameStarted(GameManager *game, const Ogre::FrameEvent &evt)
{
	//mUI_MainOverlay->show();  

	return true;
}

bool StartState::frameEnded(GameManager *game, const Ogre::FrameEvent &evt)
{
	//Ogre::Real time = evt.timeSinceLastFrame;

	////int x = mMouseOverlay->getChild("UI_Mouse")->
	////int x = OIS::MouseEvent::state.  mInputDevice->getMouseRelativeX();
	//    //int y = mInputDevice->getMouseRelativeY();


	//if( mMouse->getMouseState().buttonDown )
	//	int x = mMouse->getMouseState().X
	//	int y = mMouse->getMouseState().Y



	//       mMouseX += x, mMouseY += y;
	//       int width = mCamera->getViewport()->getActualWidth();
	//       mMouseX = std::max(0, std::min((int)mWindow->getWidth(),  mMouseX));
	//       mMouseY = std::max(0, std::min((int)mWindow->getHeight(), mMouseY));
	//       x = mMouseX, y = mMouseY;
	//       bool click = mInputDevice->getMouseButton(0);

	//       if (mGUI->injectMouse(x, y, click)) { // Mouse key down.
	//           return true;


	return mContinue;
}

bool StartState::keyPressed(GameManager *game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_P:
		mUI_MainOverlay[1]->hide();
		mUI_MainOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/Play_DOWN");
		mUI_MainOverlay[1]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case OIS::KC_ESCAPE:
		mUI_MainOverlay[4]->hide();
		mUI_MainOverlay[4] = mOverlayMgr1->getSingleton().getByName("Overlay/Exit_DOWN");
		mUI_MainOverlay[4]->show();
		//mContinue = false;
		keyState = true;
		break;
	}

	return true;
}

bool StartState::keyReleased(GameManager *game, const OIS::KeyEvent &e)
{
	if( keyState )
	{
		switch(e.key)
		{
		case OIS::KC_P:
			game->changeState(PlayState::getInstance());
			break;
		case OIS::KC_ESCAPE:
			mContinue = false;
			break;
		}
	}
	return true;
}

bool StartState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mMouseOverlay->getChild("UI_Mouse")->setPosition(e.state.X.abs -(400-18), e.state.Y.abs -(300-18));

	return true;
}


bool StartState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	////마우스의 좌표를 저장할 변수들
	/*int mX = e.state.X.abs -430; 
	int mY = e.state.Y.abs -150;*/

	mX = e.state.X.abs -500;
	mY = e.state.Y.abs -120;
	
	mMouseState = 0;

	if( id == OIS::MB_Left )
	{	
		if( StartState::mouseCheck( mUI_MainOverlay[1]->getChild("UI_Play_UP")->getLeft(), 
			mUI_MainOverlay[1]->getChild("UI_Play_UP")->getTop(), 
			mUI_MainOverlay[1]->getChild("UI_Play_UP")->getHeight(), 
			mUI_MainOverlay[1]->getChild("UI_Play_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 1;

		if( StartState::mouseCheck( mUI_MainOverlay[2]->getChild("UI_Option_UP")->getLeft(), 
			mUI_MainOverlay[2]->getChild("UI_Option_UP")->getTop(), 
			mUI_MainOverlay[2]->getChild("UI_Option_UP")->getHeight(), 
			mUI_MainOverlay[2]->getChild("UI_Option_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 2;

		if( StartState::mouseCheck( mUI_MainOverlay[3]->getChild("UI_Credits_UP")->getLeft(), 
			mUI_MainOverlay[3]->getChild("UI_Credits_UP")->getTop(), 
			mUI_MainOverlay[3]->getChild("UI_Credits_UP")->getHeight(), 
			mUI_MainOverlay[3]->getChild("UI_Credits_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 3;

		if( StartState::mouseCheck( mUI_MainOverlay[4]->getChild("UI_Exit_UP")->getLeft(), 
			mUI_MainOverlay[4]->getChild("UI_Exit_UP")->getTop(), 
			mUI_MainOverlay[4]->getChild("UI_Exit_UP")->getHeight(), 
			mUI_MainOverlay[4]->getChild("UI_Exit_UP")->getWidth(), mX, mY ) )
			
			mMouseState = 4;

		//else return true;
	}
	//else return true;

	switch( mMouseState )
	{
	case 1:
		mUI_MainOverlay[1]->hide();
		mUI_MainOverlay[1] = mOverlayMgr1->getSingleton().getByName("Overlay/Play_DOWN");
		mUI_MainOverlay[1]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case 2:
		mUI_MainOverlay[2]->hide();
		mUI_MainOverlay[2] = mOverlayMgr1->getSingleton().getByName("Overlay/Option_DOWN");
		mUI_MainOverlay[2]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case 3:
		mUI_MainOverlay[3]->hide();
		mUI_MainOverlay[3] = mOverlayMgr1->getSingleton().getByName("Overlay/Credits_DOWN");
		mUI_MainOverlay[3]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	case 4:
		mUI_MainOverlay[4]->hide();
		mUI_MainOverlay[4] = mOverlayMgr1->getSingleton().getByName("Overlay/Exit_DOWN");
		mUI_MainOverlay[4]->show();
		//game->changeState(PlayState::getInstance());
		keyState = true;
		break;
	default: break;
	}
	return true;
}

bool StartState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if( keyState )
	{
		switch( mMouseState )
		{
		case 1:
			keyState = false;
			game->changeState(PlayState::getInstance());
			break;
		case 2:
			keyState = false;
			break;
		case 3:
			keyState = false;
			break;
		case 4:
			mContinue = false;
			break;
		default: break;
		}
	}
	return true;
}

bool StartState::mouseCheck( int btX, int btY, int btWidth, int btHeight, int mX, int mY )
//bool StartState::mouseCheck( float btX, float btY, float btWidth, float btHeight, Ogre::Real mX, Ogre::Real mY )
{
	for( int i = btX; i < btWidth; i++ )
		for( int j = btY; j < btHeight; j++ )
			if( btX +i == mX && btY +j == mY ) return true;

	return false;
}