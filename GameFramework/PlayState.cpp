#include "PlayState.h"
#include "TitleState.h"
#include "OptionState.h"

using namespace Ogre;
static const float professorSpeed = 100.0f;
PlayState PlayState::mPlayState;
void PlayState::enter(void)
{
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");

	_drawGridPlane();
	_setLights();
	_drawGroundPlane();
	_drawScene();

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mSceneMgr->getSceneNode("ProfessorRoot")->setPosition(0, 0, -300);
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");

	mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw");
	mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
	mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder");
	//mCameraHolder->setPosition(0.0f, 80.f, -550.0f);

	mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mCharacterYaw->attachObject(mCharacterEntity);
	mCharacterEntity->setCastShadows(true);
	mCharacterDirection = Ogre::Vector3::ZERO;

	//mCameraHolder->attachObject(mCamera);
	mCamera->setPosition(0.0f, 350.f, -700.0f);
	mCamera->lookAt(0.0f, 200.0f, 0.0f);
	mCamera->setNearClipDistance(100);
	mCamera->setFarClipDistance(500);
	mCameraHolder->attachObject(mCamera);

	mAnimationState = mCharacterEntity->getAnimationState("Run");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();
	// ---------------------------------------
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	mAnimationState->addTime(evt.timeSinceLastFrame);

	mSceneMgr->getSceneNode("ProfessorRoot")->translate(0.0f, 0.0, 800.0f * evt.timeSinceLastFrame);
	//mCameraHolder->translate(0.0f, 0.0f, 10.0f * evt.timeSinceLastFrame);
	_controlCharacter(evt);
	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	static Ogre::DisplayString currFps = L"현재 FPS: ";
	static Ogre::DisplayString avgFps = L"평균 FPS: ";
	static Ogre::DisplayString bestFps = L"최고 FPS: ";
	static Ogre::DisplayString worstFps = L"최저 FPS: ";

	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));

	return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{

	switch (e.key)
	{
		case OIS::KC_UP:
			mSceneMgr->getSceneNode("ProfessorRoot")->translate(0.0f, -100.0f, 0.0f);
			mAnimationState->setEnabled(false);
			mAnimationState = mCharacterEntity->getAnimationState("Run");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
			mCameraHolder->translate(0.0f, 100.0f, 0.0f);
			break;
		case OIS::KC_DOWN:
			
			break;
		case OIS::KC_LEFT:
			mSceneMgr->getSceneNode("ProfessorRoot")->translate(-300.0f, 0.0f, 0.0f);
			mCameraHolder->translate(300.0f, 0.0f, 0.0f);
			break;
		case OIS::KC_RIGHT:
			mSceneMgr->getSceneNode("ProfessorRoot")->translate(300.0f, 0.0f, 0.0f);
			mCameraHolder->translate(-300.0f, 0.0f, 0.0f);
			break;
	}
	// -----------------------------------------------------

	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		game->changeState(TitleState::getInstance());
		break;
	case OIS::KC_UP:
		mSceneMgr->getSceneNode("ProfessorRoot")->translate(0.0f, 100.0f, 0.0f);
		mAnimationState->setEnabled(false);
		mAnimationState = mCharacterEntity->getAnimationState("Jumping");
		mAnimationState->setLoop(true);
		mAnimationState->setEnabled(true);
		mCameraHolder->translate(0.0f, -100.0f, 0.0f);
		break;
	case OIS::KC_DOWN:
		
		break;
	case OIS::KC_LEFT:
		mSceneMgr->getSceneNode("ProfessorRoot")->translate(300.0f, 0.0f, 0.0f);
		mCameraHolder->translate(-300.0f, 0.0f, 0.0f);
		break;
	case OIS::KC_RIGHT:
		mSceneMgr->getSceneNode("ProfessorRoot")->translate(-300.0f, 0.0f, 0.0f);
		mCameraHolder->translate(300.0f, 0.0f, 0.0f);
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	/*mCameraYaw->yaw(Degree(-e.state.X.rel));
	mCameraPitch->pitch(Degree(-e.state.Y.rel));

	mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));*/
	return true;
}



void PlayState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -2.0f, -1));
	mLightD->setVisible(true);
}

void PlayState::_drawGroundPlane(void)
{
	//할당할 바닥 갯수
	int iGroundCount = 50;
	//바닥을 위한 엔티티 & 씬노드 할당
	mGroundEntity = (Entity**)malloc(sizeof(Entity*)* iGroundCount);
	mGroundNode = (SceneNode**)malloc(sizeof(SceneNode*)* iGroundCount);

	//바닥생성
	Plane plane(Vector3::UNIT_Y, 0);
	
	MeshManager::getSingleton().createPlane(
		"Ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		500, 1000,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	for (int i = 0; i < iGroundCount; ++i)
	{
		char buf[255];
		sprintf(buf, "GroundPlane%d", i + 1);
		mGroundEntity[i] = mSceneMgr->createEntity(buf, "Ground");
		
		if (i == 0)
		{
			mGroundNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(buf);
			mGroundNode[i]->setScale(5, 1, 1);
			mGroundNode[i]->setPosition(0.0f, 0.0f, 0.0f);
		}
		else
		{
			mGroundNode[i] = mGroundNode[i - 1]->createChildSceneNode(buf);
			mGroundNode[i]->translate(Vector3(0.0f, 0.0f, 1000.0f));
		}

		mGroundNode[i]->attachObject(mGroundEntity[i]);
		
		mGroundEntity[i]->setMaterialName("Map");
		mGroundEntity[i]->setCastShadows(false);
	}

	/*Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode", Ogre::Vector3(0, 0, 0))->attachObject(groundEntity);
	mSceneMgr->getSceneNode("GroundNode")->setScale(3, 1, 3);
	mSceneMgr->getSceneNode("GroundNode")->setPosition(0, 0, 0);
	groundEntity->setMaterialName("Map");
	groundEntity->setCastShadows(false);*/
}

void PlayState::_drawScene(void)
{
	mSceneMgr->setSkyBox(true, "Sky/SpaceSkyBox", 5000);
}


void PlayState::_drawGridPlane(void)
{
	// 좌표축 표시
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);
}

void PlayState::_controlCharacter(const Ogre::FrameEvent & evt)
{

}
