#include "LobyState.h"
#include "PlayState.h"
#include "Play2State.h"
#include "Play3State.h"
#include "TitleState.h"
#include "OptionState.h"

using namespace Ogre;

LobyState LobyState::mLobyState;


void LobyState::enter(void)
{
	iCount1 = 0;
	iCount2 = 0;
	iCount3 = 0;


	fD1 = 1.0f, fD2 = 0.5f, fD3 = 0.0f;
	fP1 = 0.0f, fP2 = 1.0f, fP3 = 0.0f;

	ffD1 = 0.1f, ffD2 = 0.2f, ffD3 = 0.5f;
	ffP1 = 0.25f, ffP2 = 0.1f, ffP3 = 0.2f;

	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");
	mCamera->setPosition(Ogre::Vector3::ZERO);

	_drawGridPlane();
	_setLights();
	_drawScene();
	_drawGroundPlane();

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
	mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");

	mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 120.0f, 0.0f));
	mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
	mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 80.0f, 500.0f));
	mCameraYaw->yaw(Degree(180));
	mCameraYaw->setInheritOrientation(false);

	mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mCharacterYaw->attachObject(mCharacterEntity);
	mCharacterEntity->setCastShadows(true);

	
	
	m1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("LightD");
	m2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("LightP");
	m3 = mSceneMgr->getRootSceneNode()->createChildSceneNode("LightS");

	mCameraHolder->attachObject(mCamera);
	mCamera->lookAt(mCameraYaw->getPosition());

	mCharacterDirection = Ogre::Vector3::ZERO;

	mIdleState = mCharacterEntity->getAnimationState("Idle");
	mRunState = mCharacterEntity->getAnimationState("Run");

	mIdleState->setLoop(true);
	mRunState->setLoop(true);

	mIdleState->setEnabled(true);
	mRunState->setEnabled(false);

	
	// Sound
	//soundInit();
	soundInit();
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[LOBY], 0, &g_Channel[LOBY]);
	//mAnimationState = mCharacterEntity->getAnimationState("Run");
	//mAnimationState->setLoop(true);
	//mAnimationState->setEnabled(true);
}

void LobyState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();
	// ---------------------------------------
}

void LobyState::pause(void)
{
}

void LobyState::resume(void)
{
}

bool LobyState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	//mAnimationState->addTime(evt.timeSinceLastFrame);

	if (fD1 == 1.0f || fD1 == 0.0f)
	{
		ffD1 *= -1;
	}

	if (fD2 == 1.0f || fD2 == 0.0f)
	{
		ffD2 *= -1;
	}


	if (fD3 == 1.0f || fD3 == 0.0f)
	{
		ffD3 *= -1;
	}

	if (fP1 == 1.0f || fP1 == 0.0f)
	{
		ffP1 *= -1;
	}


	if (fP2 == 1.0f || fP2 == 0.0f)
	{
		ffP2 *= -1;
	}


	if (fP3 == 1.0f || fP3 == 0.0f)
	{
		ffP3 *= -1;
	}



	_modifyLight();

	if (mCharacterDirection != Vector3::ZERO)
	{
		mCharacterRoot->setOrientation(mCameraYaw->getOrientation());
		Quaternion quat = Vector3(Vector3::UNIT_Z).getRotationTo(mCharacterDirection);
		mCharacterYaw->setOrientation(quat);
		mCharacterRoot->translate(mCharacterDirection.normalisedCopy() * 600 * evt.timeSinceLastFrame
			, Node::TransformSpace::TS_LOCAL);
		if (!mRunState->getEnabled())
		{
			mRunState->setEnabled(true);
			mIdleState->setEnabled(false);
		}
		mRunState->addTime(evt.timeSinceLastFrame);
	}
	else
	{
		if (!mIdleState->getEnabled())
		{
			mIdleState->setEnabled(true);
			mRunState->setEnabled(false);
		}
		mIdleState->addTime(evt.timeSinceLastFrame);
	}


	if (mCharacterRoot->getPosition().x >= 450.0f && mCharacterRoot->getPosition().x <= 750.0f
		&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
	{
		if (iCount1 < 1)
			iCount1 = 0;
		iCount1 += 1;
		if (iCount1 == 1)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_1], 0, &g_Channel[SD_1]);
			//FMOD_Sound_Release(g_Sound[SD_2]);
			//FMOD_Sound_Release(g_Sound[SD_3]);
		}
	}
	else
	{
		if (iCount1 > 1)
			iCount1 = 1;
		iCount1 -= 1;
		if (iCount1 == 0)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[LOBY], 0, &g_Channel[LOBY]);
		}
		/*FMOD_Sound_Release(g_Sound[SD_1]);
		FMOD_Sound_Release(g_Sound[SD_2]);
		FMOD_Sound_Release(g_Sound[SD_3]);*/
	}
	

	if (mCharacterRoot->getPosition().x >= -200.0f && mCharacterRoot->getPosition().x <= 200.0f
		&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
	{
		if (iCount2 < 1)
			iCount2 = 0;
		iCount2 += 1;
		if (iCount2 == 1)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_2], 0, &g_Channel[SD_2]);
			//FMOD_Sound_Release(g_Sound[SD_2]);
			//FMOD_Sound_Release(g_Sound[SD_3]);
		}
	}
	else
	{
		if (iCount2 > 1)
			iCount2 = 1;
		iCount2 -= 1;
		if (iCount2 == 0)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[LOBY], 0, &g_Channel[LOBY]);
		}
		/*FMOD_Sound_Release(g_Sound[SD_1]);
		FMOD_Sound_Release(g_Sound[SD_2]);
		FMOD_Sound_Release(g_Sound[SD_3]);*/
	}

	if (mCharacterRoot->getPosition().x >= -750.0f && mCharacterRoot->getPosition().x <= -450.0f
		&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
	{
		if (iCount3 < 1)
			iCount3 = 0;
		iCount3 += 1;
		if (iCount3 == 1)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_3], 0, &g_Channel[SD_3]);
			//FMOD_Sound_Release(g_Sound[SD_2]);
			//FMOD_Sound_Release(g_Sound[SD_3]);
		}
	}
	else
	{
		if (iCount3 > 1)
			iCount3 = 1;
		iCount3 -= 1;
		if (iCount3 == 0)
		{
			Release();
			soundInit();
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[LOBY], 0, &g_Channel[LOBY]);
		}
		/*FMOD_Sound_Release(g_Sound[SD_1]);
		FMOD_Sound_Release(g_Sound[SD_2]);
		FMOD_Sound_Release(g_Sound[SD_3]);*/
	}

	return true;
}

bool LobyState::frameEnded(GameManager* game, const FrameEvent& evt)
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


bool LobyState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch (e.key)
	{
	case OIS::KC_W: case OIS::KC_UP: mCharacterDirection.z -= -600.0f; break;
	case OIS::KC_S: case OIS::KC_DOWN: mCharacterDirection.z -= 600.0f; break;
	case OIS::KC_A: case OIS::KC_LEFT: mCharacterDirection.x -= -600.0f; break;
	case OIS::KC_D: case OIS::KC_RIGHT: mCharacterDirection.x -= 600.0f; break;
	}
	return true;
}

bool LobyState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_SPACE:
		Release();
		if (mCharacterRoot->getPosition().x >= 450.0f && mCharacterRoot->getPosition().x <= 750.0f
			&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
			game->changeState(PlayState::getInstance());

		if (mCharacterRoot->getPosition().x >= -200.0f && mCharacterRoot->getPosition().x <= 200.0f
			&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
			game->changeState(Play2State::getInstance());

		if (mCharacterRoot->getPosition().x >= -750.0f && mCharacterRoot->getPosition().x <= -450.0f
			&& mCharacterRoot->getPosition().z >= 450.0f && mCharacterRoot->getPosition().z <= 750.0f)
			game->changeState(Play3State::getInstance());
		break;
	case OIS::KC_O:
		game->pushState(OptionState::getInstance());
	case OIS::KC_W: case OIS::KC_UP: mCharacterDirection.z += -600.0f; break;
	case OIS::KC_S: case OIS::KC_DOWN: mCharacterDirection.z += 600.0f; break;
	case OIS::KC_A: case OIS::KC_LEFT: mCharacterDirection.x += -600.0f; break;
	case OIS::KC_D: case OIS::KC_RIGHT: mCharacterDirection.x += 600.0f; break;
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool LobyState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool LobyState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool LobyState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	return true;
}

void LobyState::_drawScene(void)
{
	//mSceneMgr->setSkyBox(true, "3D-Diggers/SkyBox", 5000);
}


void LobyState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -1.0f, -1));
	mLightD->setVisible(true);

	mLightP = mSceneMgr->createLight("LightP");
	mLightP->setType(Light::LT_POINT);
	mLightP->setPosition(Vector3(0, 150, -500)); 
	mLightP->setVisible(true);

	mLightS = mSceneMgr->createLight("LightS");
	mLightS->setType(Light::LT_SPOTLIGHT); 
	mLightS->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
	mLightS->setPosition(Vector3(0, 100, -500)); 
	mLightS->setSpotlightRange(Degree(10), Degree(80)); 
	mLightS->setVisible(true);

}

void LobyState::_modifyLight()
{
	mLightD->setDiffuseColour(fD1 + ffD1, fD2 + ffD2, fD3 + ffD3);
	mLightP->setDiffuseColour(fP1 + ffP1, fP2 + ffP1, fP3 + ffP1);
	mLightS->setDiffuseColour(0.0f, 0.0f, 1.0f);
}

void LobyState::_drawGroundPlane(void)
{

	mTitaniumNode = (SceneNode*)malloc(sizeof(SceneNode*));

	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"LobyGround",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		1500, 1500,
		1, 1,
		true, 1, 5, 5,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity = mSceneMgr->createEntity("LobyGroundPlane", "LobyGround");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setMaterialName("KPU_LOGO");
	groundEntity->setCastShadows(false);


	Plane plane11(Vector3::UNIT_Y, -1000);
	MeshManager::getSingleton().createPlane(
		"ClubGround",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane11,
		4000, 2000,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity11 = mSceneMgr->createEntity("ClubGroundPlane", "ClubGround");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity11);
	mTitaniumNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ClubGroundPlane");
	mTitaniumNode->setPosition(0.0f, 0.0f, 300.0f);
	mTitaniumNode->pitch(Degree(90), Ogre::Node::TS_WORLD);
	mTitaniumNode->yaw(Degree(-180), Ogre::Node::TS_WORLD);
	mTitaniumNode->attachObject(groundEntity11);
	groundEntity11->setMaterialName("CLUB");
	groundEntity11->setCastShadows(false);



	Plane plane1(Vector3::UNIT_Y, -1000);
	MeshManager::getSingleton().createPlane(
		"TitanuimImage",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane1,
		500, 500,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity1 = mSceneMgr->createEntity("TitanuimPlane", "TitanuimImage");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	mTitaniumNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("TitanuimPlane");
	mTitaniumNode->setPosition(750.0f, 100.0f, 0.0f);
	mTitaniumNode->pitch(Degree(-90), Ogre::Node::TS_WORLD);
	mTitaniumNode->attachObject(groundEntity1);
	groundEntity1->setMaterialName("KPU_TITANIUM");
	groundEntity1->setCastShadows(false);

	Plane plane2(Vector3::UNIT_Y, -1000);
	MeshManager::getSingleton().createPlane(
		"WaitImage",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane1,
		500, 500,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity2 = mSceneMgr->createEntity("WaitPlane", "WaitImage");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	mTitaniumNode2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("WaitPlane");
	mTitaniumNode2->setPosition(-750.0f, 100.0f, 0.0f);
	mTitaniumNode2->pitch(Degree(-90), Ogre::Node::TS_WORLD);
	mTitaniumNode2->attachObject(groundEntity2);
	groundEntity2->setMaterialName("KPU_WAIT");
	groundEntity2->setCastShadows(false);

	Plane plane3(Vector3::UNIT_Y, -1000);
	MeshManager::getSingleton().createPlane(
		"TiestoImage",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane1,
		500, 500,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity3 = mSceneMgr->createEntity("TiestoPlane", "TiestoImage");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	mTitaniumNode3 = mSceneMgr->getRootSceneNode()->createChildSceneNode("TiestoPlane");
	mTitaniumNode3->setPosition(0.0f, 100.0f, 0.0f);
	mTitaniumNode3->pitch(Degree(-90), Ogre::Node::TS_WORLD);
	mTitaniumNode3->attachObject(groundEntity3);
	groundEntity3->setMaterialName("KPU_TIESTO");
	groundEntity3->setCastShadows(false);

}

void LobyState::_drawGridPlane(void)
{
	
}


void LobyState::soundInit()/*
						   그냥 내가 만든 함수이니까 너네 마음대로 바꿔서 쓰면됨
						   배경음은 CreateStream 으로 하는게 메모리를 덜 잡아먹는데 참고하시길.
						   우리는 Sound파일을 다 SD_Opening.mp3 이런식으로 이름 다바꿔서 Sound폴더에서 관리해. 절대경로로. 그래서 저렇게 불러오면됨
						   */
{

	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 10, FMOD_INIT_NORMAL, NULL);

	//배경음
	FMOD_System_CreateStream(g_System, "Sound/Sound1.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_1]);
	FMOD_System_CreateStream(g_System, "Sound/Sound2.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_2]);
	FMOD_System_CreateStream(g_System, "Sound/Sound3.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_3]);
	FMOD_System_CreateStream(g_System, "Sound/loby.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[LOBY]);
	// 형식은 :                g_system에, 경로, 재생 방식(계속 음악끝나면 반복), 사운드 설정 해주고

	//효과음 

	//FMOD_System_CreateSound(g_System, "Sound/효과음.wav", FMOD_DEFAULT, 0, &g_Sound[SD_Effect]);

}


void LobyState::Release() //마지막으로 음악 다 재생 되면 이런식으로 릴리즈해줘야 해.
{
	for (int i = 0; i < LOBY; ++i)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	/*FMOD_Sound_Release(g_Sound[0]);
	FMOD_Sound_Release(g_Sound[1]);*/
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}