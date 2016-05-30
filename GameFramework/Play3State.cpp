#include "Play3State.h"
#include "TitleState.h"
#include "OptionState.h"
#include "LobyState.h"

using namespace std;
using namespace Ogre;


static const float professorSpeed = 100.0f;
Play3State Play3State::mPlay3State;
void Play3State::enter(void)
{
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");

	_drawGridPlane();
	_setLights();
	_drawGroundPlane();
	_drawScene();
	_inputFile();
	_makeTestStagePattern();
	_constructTestStageSceneNode();

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

	// Sound
	soundInit();
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Stage3], 0, &g_Channel[SD_Stage3]);
}

void Play3State::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();
	Release();
	// ---------------------------------------
}

void Play3State::pause(void)
{
}

void Play3State::resume(void)
{
}

bool Play3State::frameStarted(GameManager* game, const FrameEvent& evt)
{
	mAnimationState->addTime(evt.timeSinceLastFrame);

	mSceneMgr->getSceneNode("ProfessorRoot")->translate(0.0f, 0.0, 800.0f * evt.timeSinceLastFrame);
	//mCameraHolder->translate(0.0f, 0.0f, 10.0f * evt.timeSinceLastFrame);
	_controlCharacter(evt);
	return true;
}

bool Play3State::frameEnded(GameManager* game, const FrameEvent& evt)
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


bool Play3State::keyReleased(GameManager* game, const OIS::KeyEvent &e)
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

bool Play3State::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_ESCAPE:
		Release();
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

bool Play3State::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool Play3State::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool Play3State::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	/*mCameraYaw->yaw(Degree(-e.state.X.rel));
	mCameraPitch->pitch(Degree(-e.state.Y.rel));

	mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));*/
	return true;
}



void Play3State::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -2.0f, -1));
	mLightD->setVisible(true);
}

void Play3State::_drawGroundPlane(void)
{
	//할당할 바닥 갯수
	int iGroundCount = 100;
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

void Play3State::_drawScene(void)
{
	mSceneMgr->setSkyBox(true, "Sky/PlaySkyBox", 5000);
}


void Play3State::_drawGridPlane(void)
{
	// 좌표축 표시
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);
}

void Play3State::_controlCharacter(const Ogre::FrameEvent & evt)
{

}

void Play3State::_inputFile(void)
{
	std::ifstream inFile;
	inFile.open("stage3.txt");
	std::string buf;

	while (!inFile.eof())
	{
		std::getline(inFile, buf, '\n');
		mInputFile.push_back(buf);
	}
	inFile.close();
}

void Play3State::_makeTestStagePattern(void)
{
	for (int i = 0; i<mInputFile.size(); ++i)
	{
		int temp = atoi(mInputFile[i].c_str());
		int cnt = 0;
		while (cnt<3)
		{
			int temp2 = temp % 10;
			switch (temp2)
			{
			case 0:
				break;
				mCollisionCheck[i][cnt] = false;
			case 1:
				//0.25초당 한번씩(300은 1초당, 150은 0.5초당)
				mPattern.push_back(Vector3(cnt * 250.0f - 250.0f, 100.0f, (i * 150.0f)));
				mCollisionCheck[i][cnt] = true;
				break;
			}
			//if(mCollisionCheck[i][cnt])
			//	std::cout<<"true  ";
			//else
			//	std::cout<<"false ";
			temp = temp / 10;
			cnt++;
		}
		//std::cout<<i<<std::endl;
	}
}

void Play3State::_constructTestStageSceneNode(void)
{

#define COLLISION_MAX 200
	SceneNode* mCollision = mSceneMgr->getRootSceneNode()->createChildSceneNode("CollisionNode");
	SceneNode* collisionNode[COLLISION_MAX];
	Entity* collisionEntity[COLLISION_MAX];
	for (int i = 0; i<mPattern.size(); ++i)
	{
		char collisionName[20];
		sprintf(collisionName, "CollisionEntity%d", i + 1);

		collisionEntity[i] = mSceneMgr->createEntity(collisionName, "coin.mesh");


		sprintf(collisionName, "CollisionNode%d", i + 1);
		collisionNode[i] = mCollision->createChildSceneNode(collisionName, mPattern[i]);
		//mCollision->setScale(3, 3, 1);
		collisionNode[i]->attachObject(collisionEntity[i]);
		//collisionNode[i]->scale(2.0f, 2.0f, 2.0f);
	}
}


void Play3State::soundInit()/*
						   그냥 내가 만든 함수이니까 너네 마음대로 바꿔서 쓰면됨
						   배경음은 CreateStream 으로 하는게 메모리를 덜 잡아먹는데 참고하시길.
						   우리는 Sound파일을 다 SD_Opening.mp3 이런식으로 이름 다바꿔서 Sound폴더에서 관리해. 절대경로로. 그래서 저렇게 불러오면됨
						   */
{

	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 10, FMOD_INIT_NORMAL, NULL);

	//배경음
	FMOD_System_CreateStream(g_System, "Sound/Sound3.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_Stage3]);

	// 형식은 :                g_system에, 경로, 재생 방식(계속 음악끝나면 반복), 사운드 설정 해주고


}


void Play3State::Release() //마지막으로 음악 다 재생 되면 이런식으로 릴리즈해줘야 해.
{
	for (int i = 0; i < SD_Stage3; ++i)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	/*FMOD_Sound_Release(g_Sound[0]);
	FMOD_Sound_Release(g_Sound[1]);*/
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}