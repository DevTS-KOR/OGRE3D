#include <Ogre.h>
#include <OIS/OIS.h>


#include "PlayState.h"
#include "MenuState.h"
#include "OverState.h"
#include "ClearState.h"

using namespace Ogre;

PlayState PlayState::mPlayState;

void PlayState::enter(void)
{

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

	mRoot = Root::getSingletonPtr(); 
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");

	mCamera = mSceneMgr->getCamera("main");


	mProfessorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorNode");
	mProfessorYaw = mProfessorNode->createChildSceneNode("ProfessorYaw");
	mProfessorEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	mProfessorYaw->attachObject(mProfessorEntity);

	mProfessorNode->setPosition(0.0f, 0.0f, 0.0f);
	mProfessorDirection = Ogre::Vector3::ZERO;

	mAnimationState = mProfessorEntity->getAnimationState("Idle");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);
	mJump = NONE;
	mRespawn = false;

	mCameraNode = mProfessorNode->createChildSceneNode("Camera", Vector3(0.0f, 120.0f, 0.0f));
	mCameraNode->attachObject(mCamera);
	mCamera->setPosition(0.0f, 500.0f, 800.0f);
	mCamera->lookAt(0.0f, 120.0f, 0.0f);
	
	mCamera->setNearClipDistance(100.0f);
	mCamera->setFarClipDistance(300);
	
	_setLights();
	_inputFile();
	_makeTestStagePattern();
	_constructTestStageSceneNode();

	/*mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show(); */
	mCountTimeOverlay = OverlayManager::getSingleton().getByName("Overlay/CountDown");
	mCountTimeOverlay->show();
	mPlayTimeOverlay = OverlayManager::getSingleton().getByName("Overlay/GameTime");
	mPlayTimeOverlay->show();

	
	mStart = false;
	mGameTime = 95.0f;

	SoundStruct = new tagSound[2];
	FMOD::System_Create(&System); // System Object 생성
	System->getVersion(&version);
	System->init(100, FMOD_INIT_NORMAL, 0); // 초기화

	System->createStream("music.mp3", FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, NULL, &SoundStruct[0].Sound);
	System->createStream("Jump.wav", FMOD_HARDWARE | FMOD_LOOP_OFF | FMOD_2D, NULL, &SoundStruct[1].Sound);

	System->playSound(FMOD_CHANNEL_FREE, SoundStruct[0].Sound, false, &SoundStruct[0].Channel);
}

void PlayState::exit()
{
	mSceneMgr->clearScene();
	//mInformationOverlay->hide();
	mPlayTimeOverlay->hide();
	mCountTimeOverlay->hide();

	SoundStruct[0].Channel->stop();

	SoundStruct[0].Sound->release();
	SoundStruct[1].Sound->release();

	delete [] SoundStruct;
	System->close();
	System->release(); // 해제
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	System->update();
	static float countdown = 15.0f;
	if(countdown < 0.0f)
	{
		if(!mStart)
		{
			mProfessorDirection.z += -1.0f;
			mStart = true;
			mCountTimeOverlay->hide();
		}
	}
	else
	{
		countdown -= evt.timeSinceLastFrame;
	}
	OverlayElement* countTime = OverlayManager::getSingleton().getOverlayElement("CountTime");
	if(countdown < 4)
	{
		countTime->setCaption(StringConverter::toString((int)countdown));
	}
	else
		countTime->setCaption("Ready!");

#define GRAVITATIONAL_ACCELERATION 9.8
	if(mRespawn)
	{
		Vector3 temp = mProfessorNode->getPosition();
		if(mAnimationState->getAnimationName() != "Holding")
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("Holding");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
			mProfessorNode->setPosition(0.0f, 400.0f, temp.z + 1000.0f);
		}
		static float fallingSpeed = 0;
		static float accumulationTime = 0.0f;
		accumulationTime += evt.timeSinceLastFrame;
		fallingSpeed = GRAVITATIONAL_ACCELERATION * accumulationTime;
		mAnimationState->addTime(evt.timeSinceLastFrame);
		if(mProfessorNode->getPosition().y > 0)	mProfessorNode->translate(0.0f, -fallingSpeed, 0.0f);
		if(mProfessorNode->getPosition().y < 0)
		{
			mProfessorNode->setPosition(temp.x, 0.0f, temp.z);
			mRespawn = false;
			accumulationTime = 0;
			fallingSpeed = 0;
		}
	}
	else
	{
		if(mStart)
			_controlCharacter(evt);
	}

	if(mGameTime < 0)
	{
		mCountTimeOverlay->hide();
		game->changeState(OverState::getInstance());
	}

	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	if(mProfessorNode->getPosition().z < -27000.0f)
	{
		game->changeState(ClearState::getInstance());
	}
	static Ogre::DisplayString currFps = L"현재 FPS: "; 
	static Ogre::DisplayString avgFps = L"평균 FPS: ";
	static Ogre::DisplayString bestFps = L"최고 FPS: ";
	static Ogre::DisplayString worstFps = L"최저 FPS: ";
	static Ogre::DisplayString gameTime = L"Play Time: ";
/*
	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");
	*/OverlayElement* guiTime = OverlayManager::getSingleton().getOverlayElement("PlayTime");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();
	if(mStart)
		mGameTime -= evt.timeSinceLastFrame;
/*
	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));*/
	guiTime->setCaption(gameTime + StringConverter::toString(mGameTime));

	return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_W:
	case OIS::KC_UP:
		mProfessorDirection.z += 1.0f;
		break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		mProfessorDirection.z += -1.0f;
		break;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		mProfessorDirection.x += 1.0f;
		break;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		mProfessorDirection.x += -1.0f;
		break;
	}
	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_ESCAPE:
		game->changeState(MenuState::getInstance());
		break;
	case OIS::KC_W:
	case OIS::KC_UP:
		mProfessorDirection.z += -1.0f;
		break;
	case OIS::KC_S:
	case OIS::KC_DOWN:
		mProfessorDirection.z += 1.0f;
		break;
	case OIS::KC_A:
	case OIS::KC_LEFT:
		mProfessorDirection.x += -1.0f;
		break;
	case OIS::KC_D:
	case OIS::KC_RIGHT:
		mProfessorDirection.x += 1.0f;
		break;
	case OIS::KC_RETURN:
		mStart = false;
		break;
	case OIS::KC_SPACE:
		mJump = JUMP_UP;
		break;
	}
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
	return true;
}



void PlayState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection( Vector3( 1, -2.0f, -1 ) );
	mLightD->setVisible(true);
}

void PlayState::_drawGridPlane(void)
{
	// 좌표축 표시
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

	Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
	Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

	Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
	gridPlaneMaterial->setReceiveShadows(false); 
	gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

	gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
	for(int i=0; i<21; i++)
	{
		gridPlane->position(-500.0f, 0.0f, 500.0f-i*50);
		gridPlane->position(500.0f, 0.0f, 500.0f-i*50);

		gridPlane->position(-500.f+i*50, 0.f, 500.0f);
		gridPlane->position(-500.f+i*50, 0.f, -500.f);
	}

	gridPlane->end(); 

	gridPlaneNode->attachObject(gridPlane);
}

void PlayState::_inputFile(void)
{
	std::ifstream inFile;
	inFile.open("stage1.txt");
	std::string buf;

	while(!inFile.eof())
	{
		std::getline(inFile, buf, '\n');
		mInputFile.push_back(buf);
	}
	inFile.close();
}

void PlayState::_makeTestStagePattern(void)
{
	for(int i=0; i<mInputFile.size(); ++i)
	{
		int temp = atoi(mInputFile[i].c_str());
		int cnt = 0;
		while(cnt<7)
		{
			int temp2 = temp%10;
			switch(temp2)
			{
			case 0:
				break;
				mCollisionCheck[i][cnt] = false;
			case 1:
				mPattern.push_back(Vector3(cnt * 200.0f - 600.0f, 0.0f, - (i*300.0f)));
				mCollisionCheck[i][cnt] = true;
				break;
			}
			//if(mCollisionCheck[i][cnt])
			//	std::cout<<"true  ";
			//else
			//	std::cout<<"false ";
			temp = temp/10;
			cnt++;
		}
		//std::cout<<i<<std::endl;
	}
}

void PlayState::_constructTestStageSceneNode(void)
{
	Ogre::Entity* backgroundEntity = mSceneMgr->createEntity("Background", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode", Ogre::Vector3(0, 0, 0))->attachObject(backgroundEntity);
	mSceneMgr->getSceneNode("BackgroundNode")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity2 = mSceneMgr->createEntity("Background2", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode2", Ogre::Vector3(0, 0, -7998))->attachObject(backgroundEntity2);
	mSceneMgr->getSceneNode("BackgroundNode2")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity3 = mSceneMgr->createEntity("Background3", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode3", Ogre::Vector3(0, 0, -15996))->attachObject(backgroundEntity3);
	mSceneMgr->getSceneNode("BackgroundNode3")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity4 = mSceneMgr->createEntity("Background4", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode4", Ogre::Vector3(0, 0, -23994))->attachObject(backgroundEntity4);
	mSceneMgr->getSceneNode("BackgroundNode4")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity5 = mSceneMgr->createEntity("Background5", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode5", Ogre::Vector3(0, 0, -31992))->attachObject(backgroundEntity5);
	mSceneMgr->getSceneNode("BackgroundNode5")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity6 = mSceneMgr->createEntity("Background6", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode6", Ogre::Vector3(0, 0, -39990))->attachObject(backgroundEntity6);
	mSceneMgr->getSceneNode("BackgroundNode6")->setScale(0.1f, 0.1f, 0.1f);

	Ogre::Entity* backgroundEntity7 = mSceneMgr->createEntity("Background7", "Background.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("BackgroundNode7", Ogre::Vector3(0, 0, -47988))->attachObject(backgroundEntity7);
	mSceneMgr->getSceneNode("BackgroundNode7")->setScale(0.1f, 0.1f, 0.1f);

	#define COLLISION_MAX 200
	SceneNode* mCollision = mSceneMgr->getRootSceneNode()->createChildSceneNode("CollisionNode");
	SceneNode* collisionNode[COLLISION_MAX];
	Entity* collisionEntity[COLLISION_MAX];
	for(int i=0; i<mPattern.size(); ++i)
	{
		char collisionName[20];
		sprintf(collisionName, "CollisionEntity%d", i+1);
		if(i %5 == 0)
			collisionEntity[i] = mSceneMgr->createEntity(collisionName, "box2.mesh");
		else if(i %5 == 1)
			collisionEntity[i] = mSceneMgr->createEntity(collisionName, "box3.mesh");
		else if(i %5 == 2)
			collisionEntity[i] = mSceneMgr->createEntity(collisionName, "box4.mesh");
		else if(i %5 == 3)
			collisionEntity[i] = mSceneMgr->createEntity(collisionName, "box5.mesh");
		else
			collisionEntity[i] = mSceneMgr->createEntity(collisionName, "box6.mesh");

		sprintf(collisionName, "CollisionNode%d", i+1);
		collisionNode[i] = mCollision->createChildSceneNode(collisionName, mPattern[i]);
		collisionNode[i]->attachObject(collisionEntity[i]);
		//collisionNode[i]->scale(2.0f, 2.0f, 2.0f);
	}
}

bool PlayState::_checkCollision()
{
#define PROFESSOR_BONDINGBOX_WIDTH_HALF 5

	Vector3 temp1 = mProfessorNode->getPosition();//+ Vector3(0.0f, 0.0f, 10.0f);

	if(((int)temp1.z % 300) / 50 == 0)
	{
		if(temp1.y < 100)
		{
			int temp3[2][2] = {
				{abs((int)temp1.z) / 300, (int)(temp1.x+PROFESSOR_BONDINGBOX_WIDTH_HALF + 700) / 200},
				{abs((int)temp1.z) / 300, (int)(temp1.x-PROFESSOR_BONDINGBOX_WIDTH_HALF + 700) / 200}
			};

			//std::cout<<temp3[0][0]<<" "<<temp3[0][1]<<std::endl;

			if ((mCollisionCheck[temp3[0][0]][temp3[0][1]]||mCollisionCheck[temp3[1][0]][temp3[1][1]]))
			{
				return true;
			}
			return (mCollisionCheck[temp3[0][0]][temp3[0][1]]||mCollisionCheck[temp3[1][0]][temp3[1][1]]);
		}
	}
	return false;
}

void PlayState::_controlCharacter(const FrameEvent& evt)
{
	static const float professorSpeed = 300.0f;
	if(mProfessorDirection != Vector3::ZERO)
	{
		mRespawn = _checkCollision();
		//if(mRespawn)
		//	std::cout<<"true"<<std::endl;
		//else
		//	std::cout<<"false"<<std::endl;

		if(mJump == NONE && mAnimationState->getAnimationName() != "Run")
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("Run");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
		}

		Quaternion quat = Vector3(Vector3::UNIT_Z).getRotationTo(mProfessorDirection);
		mProfessorYaw->setOrientation(quat);
		if(mProfessorNode->getPosition().x >= -700 && mProfessorNode->getPosition().x <= 700)
		{
			mProfessorNode->translate(mProfessorDirection.normalisedCopy() * professorSpeed * evt.timeSinceLastFrame, Ogre::Node::TransformSpace::TS_LOCAL);
		}
		else
		{
			Vector3 temp = mProfessorNode->getPosition();
			if(temp.x < -700) mProfessorNode->setPosition(-700, temp.y, temp.z);
			if(temp.x > 700) mProfessorNode->setPosition(700, temp.y, temp.z);
		}
	}
	else
	{
		if(mJump == NONE && mAnimationState->getAnimationName() != "Idle")
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("Idle");
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(true);
		}
	}

	static float professorJumpVelocity = 500.0f;
	if(mJump != NONE)
	{
		professorJumpVelocity -= GRAVITATIONAL_ACCELERATION * evt.timeSinceLastFrame * 70;
		mProfessorNode->translate(0.0f, professorJumpVelocity*evt.timeSinceLastFrame, 0.0f);

		if(mProfessorNode->getPosition().y < 0)
		{
			professorJumpVelocity = 500.0f;
			Vector3 temp = mProfessorNode->getPosition();
			mProfessorNode->setPosition(temp.x, 0.0f, temp.z);
			mJump = NONE;
		}
	}
	if(mJump == JUMP_UP)
	{
		if(mAnimationState->getAnimationName() == "Run" || mAnimationState->getAnimationName() == "Idle")
		{
			System->playSound(FMOD_CHANNEL_FREE, SoundStruct[1].Sound, false, &SoundStruct[1].Channel);
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("JumpUp");
			mAnimationState->setTimePosition(0.0f);
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(false);
		}
		else if(mAnimationState->hasEnded())
		{
			mJump = JUMPPING;
		}
	}
	else if(mJump == JUMPPING)
	{
		if(mAnimationState->getAnimationName() == "JumpUp")
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("Jumping");
			mAnimationState->setTimePosition(0.0f);
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(false);
		}
		else if(mProfessorNode->getPosition().y<80)
		{
			mJump = JUMP_END;
		}
	}
	else if(mJump == JUMP_END)
	{
		if(mAnimationState->getAnimationName() == "Jumping")
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
			mAnimationState = mProfessorEntity->getAnimationState("JumpEnd");
			mAnimationState->setTimePosition(0.0f);
			mAnimationState->setEnabled(true);
			mAnimationState->setLoop(false);
		}
	}
	
	mAnimationState->addTime(evt.timeSinceLastFrame);
}

