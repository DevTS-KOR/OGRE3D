#include "PlayState.h"
#include "TitleState.h"
#include "OptionState.h"
#include "LobyState.h"

using namespace std;
using namespace Ogre;


static const float professorSpeed = 100.0f;
PlayState PlayState::mPlayState;

void PlayState::enter(void)
{

	fSpeed = 1.f;
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

	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	//mInformationOverlay2 = OverlayManager::getSingleton().getByName("Overlay/Information2");
	//mInformationOverlay2->show();

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

	//mOverlaySystem = new Ogre::OverlaySystem();
	//mSceneMgr->addRenderQueueListener(mOverlaySystem);


	mAnimationState = mCharacterEntity->getAnimationState("Run");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	_constructTestStageSceneNode();
	_setOverlay();
	CoinCount = 0;
	// Sound
	soundInit();
	FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[SD_Stage1], 0, &g_Channel[SD_Stage1]);
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	mInformationOverlay->hide();
	mTextOverlay->hide();
	//mInformationOverlay2->hide();
	//Release();


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
	_crush();
	_UpdateOverlay();
	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	static Ogre::DisplayString currFps = L"���� FPS: ";
	static Ogre::DisplayString avgFps = L"��� FPS: ";
	static Ogre::DisplayString bestFps = L"�ְ� FPS: ";
	static Ogre::DisplayString worstFps = L"���� FPS: ";
	static Ogre::DisplayString Coin = L"���� ���� : ";

	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");
	OverlayElement* guiCoint = OverlayManager::getSingleton().getOverlayElement("CoinCount");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));
	guiCoint->setCaption(Coin + StringConverter::toString(stats.coinFPS));

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
		Release();
		exit();
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
		fSpeed += 1.0f;
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
	//�Ҵ��� �ٴ� ����
	int iGroundCount = 100;
	//�ٴ��� ���� ��ƼƼ & ����� �Ҵ�
	mGroundEntity = (Entity**)malloc(sizeof(Entity*)* iGroundCount);
	mGroundNode = (SceneNode**)malloc(sizeof(SceneNode*)* iGroundCount);

	//�ٴڻ���
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
	mSceneMgr->setSkyBox(true, "Sky/PlaySkyBox", 5000);
}


void PlayState::_drawGridPlane(void)
{
	// ��ǥ�� ǥ��
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);
}

void PlayState::_controlCharacter(const Ogre::FrameEvent & evt)
{

}

void PlayState::_inputFile(void)
{
	std::ifstream inFile;
	inFile.open("stage1.txt");
	std::string buf;

	while (!inFile.eof())
	{
		std::getline(inFile, buf, '\n');
		mInputFile.push_back(buf);
	}
	inFile.close();
}

void PlayState::_makeTestStagePattern(void)
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
				//0.25�ʴ� �ѹ���(300�� 1�ʴ�, 150�� 0.5�ʴ�)
				if(cnt == 1)
					mPattern.push_back(Vector3(cnt * 250.0f - 250.0f, 200.0f, (i * 150.0f)));
				else
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

void PlayState::_constructTestStageSceneNode(void)
{

	SceneNode* mCollision = mSceneMgr->getRootSceneNode()->createChildSceneNode("CollisionNode");
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

void PlayState::_crush()
{
	for (int i = 0; i < mPattern.size(); ++i)
	{
		if (mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().z > collisionNode[i]->getPosition().z - 2.5f &&
			mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().z < collisionNode[i]->getPosition().z + 2.5f &&
			mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().x > collisionNode[i]->getPosition().x - 100.0f &&
			mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().x < collisionNode[i]->getPosition().x + 100.0f &&
			mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().y > collisionNode[i]->getPosition().y - 200.0f &&
			mSceneMgr->getSceneNode("ProfessorRoot")->getPosition().y < collisionNode[i]->getPosition().y + 200.0f)
		{
			//cout << 1 << endl;
			collisionNode[i]->setPosition(Vector3(0.0f, 5000.0f, 0.0f));
			FMOD_System_PlaySound(g_System, FMOD_CHANNEL_FREE, g_Sound[COL], 0, &g_Channel[COL]);
			FMOD_System_Update(g_System);
			CoinCount++;
		}
	}
}

void PlayState::_setOverlay()
{

	mOverlayMgr = OverlayManager::getSingletonPtr();
	mTextOverlay = mOverlayMgr->create("TextOverlay");
	mPanel = static_cast<Ogre::OverlayContainer*> (mOverlayMgr->createOverlayElement("Panel", "container1"));

	mPanel->setDimensions(1, 1);
	mPanel->setPosition(0.5f, 0.0f);
	textBox = mOverlayMgr->createOverlayElement("TextArea", "TextID");

	textBox->setMetricsMode(Ogre::GMM_PIXELS); textBox->setPosition(10, 10);
	textBox->setWidth(100); textBox->setHeight(20);
	textBox->setParameter("font_name", "Font/NanumBold18");
	textBox->setParameter("char_height", "40");
	textBox->setColour(Ogre::ColourValue::White);

	mPanel->addChild(textBox);
	mTextOverlay->add2D(mPanel);
	mTextOverlay->show();

	

}

void PlayState::_UpdateOverlay()
{
	textBox->setCaption(StringConverter::toString(CoinCount));
}



void PlayState::soundInit()/*
						   �׳� ���� ���� �Լ��̴ϱ� �ʳ� ������� �ٲ㼭 �����
						   ������� CreateStream ���� �ϴ°� �޸𸮸� �� ��ƸԴµ� �����Ͻñ�.
						   �츮�� Sound������ �� SD_Opening.mp3 �̷������� �̸� �ٹٲ㼭 Sound�������� ������. �����η�. �׷��� ������ �ҷ������
						   */
{
	
	FMOD_System_Create(&g_System);
	FMOD_System_Init(g_System, 10, FMOD_INIT_NORMAL, NULL);

	//�����
	FMOD_System_CreateStream(g_System, "Sound/Sound1.mp3", FMOD_LOOP_NORMAL, 0, &g_Sound[SD_Stage1]);
	FMOD_System_CreateSound(g_System, "Sound/col.wav", FMOD_DEFAULT, 0, &g_Sound[COL]);

	// ������ :                g_system��, ���, ��� ���(��� ���ǳ����� �ݺ�), ���� ���� ���ְ�


}


void PlayState::Release() //���������� ���� �� ��� �Ǹ� �̷������� ����������� ��.
{
	for (int i = 0; i < COL; ++i)
	{
		FMOD_Sound_Release(g_Sound[i]);
	}
	/*FMOD_Sound_Release(g_Sound[0]);
	FMOD_Sound_Release(g_Sound[1]);*/
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}