#include <Ogre.h>
#include <OIS/OIS.h>
#include <OgreOpcode.h>

#include "PlayState.h"
#include "StartState.h"
#include "PauseState.h"
#include "CardState.h"
#include "EndState.h"
#include "fmod.hpp"

#include <time.h>

using namespace Ogre;
using namespace OIS;
using namespace std;

PlayState PlayState::mPlayState;

void PlayState::enter()
{  

	mRoot = Root::getSingletonPtr();
	//장면 관리자를 지형 장면 관리자로 바꿈
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");

	//mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");

	//카메라 설정
	mCamera =mSceneMgr->getCamera("main");

	mCamera->setNearClipDistance(50);


	//mSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
	//mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

	// 좌표축 표시
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")-> setScale(5, 5, 5);

	mMap1Entity = mSceneMgr->createEntity("Map1","Museum..mesh");
	mMap1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Map1",Ogre::Vector3(0,0,0));
	mMap1->setScale(7,5,10);
	mMap1->yaw(Degree(180));
	mMap1->attachObject(mMap1Entity);
	mMap1Entity->setCastShadows(false);

	// 마우스 오버레이
	mMouseOverlay = OverlayManager::getSingleton().getByName("Overlay/Cursor");
	mMouseOverlay->show();

	setEnemy(0);
	setLight();
	setCoin();
	
	//drawGroundPlane();

	mMain1Entity = mSceneMgr->createEntity("Main1","Panto.mesh");
	mMain1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Main1", Ogre::Vector3(100.0f, 17.0f, 100.0f));
	mMain1->attachObject(mMain1Entity);
	mMain1->setScale(5,5,5);
	mMain1Entity->setCastShadows(true);
	mMain1->setInheritScale(false);
	//mMain1->showBoundingBox(mMain1Entity);

	mMainAnimationState = mSceneMgr->getEntity("Main1")->getAnimationState("Normal");
	mMainAnimationState->setLoop(true);
	mMainAnimationState->setEnabled(true);

	mMLightEntity = mSceneMgr->createEntity("MLight","Light..mesh");
	mMLight = mMain1->createChildSceneNode(Ogre::Vector3(-8,7,10));
	mMLight->attachObject(mMLightEntity);
	mMLight->setScale(0.2,0.2,0.2);

	//mEnemy1Entity = mSceneMgr->createEntity("mEnemy1","Goram.mesh");
	//mEnemy1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("mEnemy1", Ogre::Vector3(300.0f,0.0f,200.0f));
	//mEnemy1->attachObject(mEnemy1Entity);
	//mEnemy1Entity->setCastShadows(true);
	////mEnemy1->showBoundingBox(true);

	//mEnemy1AnimationState = mSceneMgr->getEntity("mEnemy1")->getAnimationState("Wark");
	//mEnemy1AnimationState->setLoop(true);
	//mEnemy1AnimationState->setEnabled(true);

	for(int i=0; i<EnemyNum; ++i){

		char nameEnemy[EnemyNum];
		sprintf(nameEnemy,"Enemy%d",i);

		Enemy1AnimationState[i] = mSceneMgr->getEntity(nameEnemy)->getAnimationState("Normal");
		Enemy1AnimationState[i]->setLoop(true);
		Enemy1AnimationState[i]->setEnabled(true);
	}
	 mHead1Entity = mSceneMgr->createEntity("Head1","ogrehead.mesh");
	 mHead1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Head1",Ogre::Vector3(700.0f,30.0f, 700.0f));
	 mHead1->attachObject(mHead1Entity);
	 mHead1->yaw(Degree(180));

	 mHead2Entity = mSceneMgr->createEntity("Head2","ogrehead.mesh");
	 mHead2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Head2",Ogre::Vector3(-700.0f,30.0f, 700.0f));
	 mHead2->attachObject(mHead2Entity);
	 mHead2->yaw(Degree(180));

	 mHead3Entity = mSceneMgr->createEntity("Head3","ogrehead.mesh");
	 mHead3 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Head3",Ogre::Vector3(100.0f,30.0f,100.0f));
	 mHead3->attachObject(mHead3Entity);
	 mHead3->yaw(Degree(180));


	M1Yaw = mMain1->createChildSceneNode("M1Yaw");

	cameraYaw = mMain1->createChildSceneNode("CameraYaw",Ogre::Vector3(0.0f,50.0f,0.0f));
	cameraPitch = cameraYaw->createChildSceneNode("CameraPitch");
	cameraHolder = cameraPitch->createChildSceneNode("CameraHolder",Ogre::Vector3(0.0f,0.0f,-350.0f));

	/*cameraYaw1 = mMain1->createChildSceneNode("CameraYaw1",Ogre::Vector3(0.0f,50.0f,0.0f));
	cameraPitch1 = cameraYaw1->createChildSceneNode("CameraPitch1");
	cameraHolder1 = cameraPitch1->createChildSceneNode("CameraHolder1",Ogre::Vector3(0.0f,0.0f,-350.0f));*/

	cameraYaw->setInheritOrientation(false);
	cameraYaw->setInheritScale(false);

	/*cameraYaw1->setInheritOrientation(false);
	cameraYaw1->setInheritScale(false);*/


	cameraHolder->attachObject(mCamera);
	//mCamera->lookAt(mMain1->getPosition().x,mMain1->getPosition().y+10,mMain1->getPosition().z);
	mCamera->lookAt(mMain1->getPosition());


	mCharacterDirection = Ogre::Vector3::ZERO;

	mContinue = true;
	mCount =0;

	//OgreOpcode 설정
	//new OgreOpcode::CollisionManager(mSceneMgr);



	//광원
	mLightNode = mMain1->createChildSceneNode(Ogre::Vector3(-8,10,10));
	mLightNode->attachObject(mLightD);


	// 광선 
	//mRaySceneQuery = mSceneMgr->createRayQuery(Ray());

	//지형생성
	//mSceneMgr->setWorldGeometry("terrain.cfg");
	//mSceneMgr->setSkyDome(true,"Examples/CloudySky",2,8);
	//mSceneMgr->setSkyBox(true,"Examples/SpaceSkyBox");

	mPoint=100;
	LifeState = 10;
	FlyState =0;
	mTime = 3000;
	mRightID = -1;

	// 파티클
	mFountainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PurpleFountain",Ogre::Vector3(0,0,0));
	Ogre::ParticleSystem* pSys = mSceneMgr->createParticleSystem("particle_system","Particle/PurpleFountain");
	mFountainNode->attachObject(pSys);

	//사운드
	// Fmod system 생성
	FMOD::System_Create(&m_pSystem);
	// Fmod system 초기화
	m_pSystem->init(100,FMOD_INIT_NORMAL,0);

	//사운드 생성
	m_pSystem->createSound("wkdus125.wav",FMOD_DEFAULT,0,&m_pSound);
	m_pSystem->createSound("Wheep.mp3",FMOD_DEFAULT,0,&sound1);
	//사운드 모드 설정
	m_pSound->setMode(FMOD_LOOP_NORMAL);
	sound1->setMode(FMOD_LOOP_OFF);

	m_pSystem->playSound(FMOD_CHANNEL_FREE, m_pSound, true, &m_pChannel);


	m_pChannel->setVolume(1.0f);
	//m_pChannel->setLoopCount(1);
	m_pChannel->setPaused(false);

	//gmoteManager->init();
	gmoteManager->setEventListener(this);

}

void PlayState::exit()
{
	mSceneMgr->clearScene();
	//카메라 없애기
	//mSceneMgr->destroyAllCameras();
	//뷰포트 없애기
	//mRoot->getAutoCreatedWindow()->removeAllViewports();
	//오버레이 없애기
	//mOverlayMgr->destroyAll();
	mInformationOverlay->hide();
	mPointOverlay->hide();
	mLifeOverlay->hide();
	mFlyOverlay->hide();

	m_pChannel->setPaused(true);
}

void PlayState::pause()
{
	mInformationOverlay->hide();
	mPointOverlay->hide();
	mLifeOverlay->hide();
	mFlyOverlay->hide();

}

void PlayState::resume()
{
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{

	//game->captureInput();
	gmoteManager->capture();
	setOverlay();

	//////////////////////////////////////////////////////////////////////////
	//오우거 머리 이동 경로
   static float EnemyVelocity =  -50.0f;
   if(mHead1->getPosition().x < 0.f || mHead1->getPosition().x> 1000.0f)
	   EnemyVelocity *=-1;
   mHead1->translate(EnemyVelocity * evt.timeSinceLastFrame,0,EnemyVelocity*evt.timeSinceLastFrame);


   if(mHead2->getPosition().z < 0.0f || mHead1->getPosition().z>1000.0f)
	   EnemyVelocity *=-1;
   mHead2->translate(EnemyVelocity * evt.timeSinceLastFrame,0,EnemyVelocity*evt.timeSinceLastFrame);

   if(mHead3->getPosition().x < 100.0f || mHead3->getPosition().x> 1000.0f)
	   EnemyVelocity *=-1;
   mHead3->translate(EnemyVelocity * evt.timeSinceLastFrame,0,EnemyVelocity*evt.timeSinceLastFrame);

	//////////////////////////////////////////////////////////////////////////
	// 애니메이션 루프
	for(int i=0; i<EnemyNum; ++i){
		if( true == Enemy1AnimationState[i]->getLoop()){
			if( true == Enemy1AnimationState[i]->getEnabled())
				Enemy1AnimationState[i]->addTime(evt.timeSinceLastFrame/5);
		}
	}

	/*if(true == mEnemy1AnimationState->getLoop())
		mEnemy1AnimationState->addTime(evt.timeSinceLastFrame/5);*/

	mWalkSpeed2 = rand()%10;

	//////////////////////////////////////////////////////////////////////////
	//애니메이션 바꾸기
	for(int i =0 ; i<10 ; ++i)
	{
		char nameEnemy[EnemyNum];
		sprintf(nameEnemy,"Enemy%d",i);
		temp=mSceneMgr->getSceneNode(nameEnemy)->getPosition();
		EnemyState[i] = attackRange(mMain1->getPosition(),temp);
		changeattackState(EnemyState[i],mSceneMgr->getSceneNode(nameEnemy));
	}

	if(mCharacterDirection == Ogre::Vector3::ZERO){
		mMainAnimationState->setEnabled(false);
    	mMainAnimationState = mMain1Entity->getAnimationState("Normal");
    	mMainAnimationState->setLoop(true);
	    mMainAnimationState->setEnabled(true);
	}
	else if(mCharacterDirection.x>=1){
		mMainAnimationState->setEnabled(false);
		mMainAnimationState = mMain1Entity->getAnimationState("L_SideStep");
		mMainAnimationState->setLoop(true);
		mMainAnimationState->setEnabled(true);
	}
	else if(mCharacterDirection.x<=-1){
		mMainAnimationState->setEnabled(false);
		mMainAnimationState = mMain1Entity->getAnimationState("R_SideStep");
		mMainAnimationState->setLoop(true);
		mMainAnimationState->setEnabled(true);

	}
	else if(mCharacterDirection.z>=1){
		mMainAnimationState->setEnabled(false);
		mMainAnimationState = mMain1Entity->getAnimationState("Wark");
		mMainAnimationState->setLoop(true);
		mMainAnimationState->setEnabled(true);
	}
	else if(mCharacterDirection.z<=-1){
		mMainAnimationState->setEnabled(false);
		mMainAnimationState = mMain1Entity->getAnimationState("B_Wark");
		mMainAnimationState->setLoop(true);
		mMainAnimationState->setEnabled(true);
	}


	// 케릭터 사원수 회전과 애니메이션
	if(mCharacterDirection != Ogre::Vector3::ZERO)
	{	
		mMain1->setOrientation(cameraYaw->getOrientation());
		Quaternion quat = Ogre::Vector3(Ogre::Vector3::UNIT_Z).getRotationTo(mCharacterDirection);
		M1Yaw->setOrientation(quat);
		mMain1->translate(mCharacterDirection.normalisedCopy()*200*evt.timeSinceLastFrame,Node::TransformSpace::TS_LOCAL);
		//mMain1->translate(mCharacterDirection.normalisedCopy()*200*evt.timeSinceLastFrame,Node::TransformSpace::TS_PARENT);
		if(mCharacterDirection.x>1){
			mCharacterDirection.x =0;
		}
		else if(mCharacterDirection.x<-1){
			mCharacterDirection.x =0;
		}
		else if(mCharacterDirection.z>1){
			mCharacterDirection.z =0;
		}
		else if(mCharacterDirection.z<-1){
			mCharacterDirection.z =0;
		}

		//mMainAnimationState->addTime(evt.timeSinceLastFrame*2);
	}
	mMainAnimationState->addTime(evt.timeSinceLastFrame);

	if(mPoint>=0){
		DrawLine();
	}


	// Ray를 이용한 지형과 충돌 처리
	/* Ogre::Vector3 camPos = mMain1->getPosition( );
	Ray cameraRay( camPos,Ogre::Vector3::NEGATIVE_UNIT_Y );
	mRaySceneQuery->setRay( cameraRay );
	RaySceneQueryResult result = mRaySceneQuery->execute();
	RaySceneQueryResult::iterator itr = result.begin( );
	if ( itr != result.end() && itr->worldFragment ) {
	Real terrainHeight = itr->worldFragment->singleIntersection.y;
	if ((terrainHeight + 10.0f) > camPos.y)
	mMain1->setPosition(camPos.x, terrainHeight+5.0f, camPos.z);
	} */
	//return mContinue;

	// collisionTools를 이용한 지형과 충돌 
	Ogre::Vector3 mMainPos = mMain1->getPosition();
	Ogre::Entity *target = mSceneMgr->getEntity("Map1");
	Ogre::Vector3 result = Ogre::Vector3::ZERO;
	mCollisionTools = new MOC::CollisionTools(mSceneMgr);
	Ogre::Vector3 front = mMain1->getOrientation()*Ogre::Vector3::UNIT_Z;
	Ogre::Vector3 left;

	//Ray cameraRay(mMainPos,Ogre::Vector3::NEGATIVE_UNIT_Y);
	//mRaySceneQuery = mSceneMgr->createRayQuery(Ray(mMainPos,Ogre::Vector3::NEGATIVE_UNIT_Y));
	//mRaySceneQuery->setRay(cameraRay);

	float distToCollY;
	//float distToCollZ;
	//float distToColl_Z;
	//float distToCollX;
	//float distToColl_X;

	////지형과 충돌 체크 
	if(mCollisionTools->raycastFromPoint(Ogre::Vector3(mMainPos.x-10,mMainPos.y+10,mMainPos.z-10),mMainPos.UNIT_Y,result,target,distToCollY)){
			//mMain1->setPosition(mMainPos.x,mMainPos.y+(distToCollY-10), mMainPos.z);
		if(distToCollY<20)
			mMain1->setPosition(mMainPos.x, mMainPos.y +( distToCollY-5),mMainPos.z);
			std::cout<<distToCollY<<std::endl;
			std::cout<<result<<std::endl;
	}

	// 메인케릭 앞에서 레이 쏴 충돌
	//if(mCollisionTools->raycastFromPoint(Ogre::Vector3(mMainPos.x,mMainPos.y,mMainPos.z-10),front,result,target,distToCollZ)){
	//		//mMain1->setPosition(mMainPos.x,mMainPos.y+(distToCollY-10), mMainPos.z);
	//	if(distToCollZ<50)
	//		mMain1->translate(-front.x*5, 0.0f ,-front.z*5);
	//		std::cout<<distToCollZ<<std::endl;
	//		std::cout<<result<<std::endl;
	//}
	//if(mCollisionTools->raycastFromPoint(Ogre::Vector3(mMainPos.x,mMainPos.y,mMainPos.z-15),mMainPos.NEGATIVE_UNIT_Z,result,target,distToColl_Z)){
	//	if(distToColl_Z<50)
	//		mMain1->setPosition(mMainPos.x, mMainPos.y,mMainPos.z+10);
	//		std::cout<<distToCollY<<std::endl;
	//		std::cout<<result<<std::endl;
	//}
	/*if(mCollisionTools->raycastFromPoint(Ogre::Vector3(mMainPos.x+15,mMainPos.y,mMainPos.z),mMainPos.UNIT_X,result,target,distToCollX)){
		if(distToCollX<50)
			mMain1->setPosition(mMainPos.x-10, mMainPos.y,mMainPos.z);
			std::cout<<distToCollY<<std::endl;
			std::cout<<result<<std::endl;
	}*/
	/*if(mCollisionTools->raycastFromPoint(Ogre::Vector3(mMainPos.x-15,mMainPos.y,mMainPos.z),mMainPos.NEGATIVE_UNIT_X,result,target,distToColl_X)){
		if(distToColl_X<50)
			mMain1->setPosition(mMainPos.x+10, mMainPos.y,mMainPos.z);
			std::cout<<distToCollY<<std::endl;
			std::cout<<result<<std::endl;
	}*/

	//Ogre::Vector3 mEnemyPos = mEnemy1->getPosition();
	//if(!mCollisionTools->collidesWithEntity(mMainPos, mEnemyPos,2.5,0)){
	//	//mMain1->setPosition(mMainPos.x,mMainPos.y,mMainPos.z);
	//	std::cout<<"collision"<<std::endl;
	//}

    mDirection2 = Ogre::Vector3::ZERO;

	Ogre::Vector3 tem1;
	Ogre::Vector3 tem2;



	//바운드 박스를 이용한 충돌처리
	/*if(mEnemy1Entity->getWorldBoundingBox().intersects(mMain1Entity->getWorldBoundingBox())){
		mDirection10 = mEnemy1->getPosition() - mMain1->getPosition();
		mDistance10  = mDirection3.normalise();
		do{
			mMain1->translate(-mDirection10.x/10,0,-mDirection10.z/10);
		}while(mDistance10>10);
	}*/
	/*if(mSceneMgr->getEntity("Enemy1")->getWorldBoundingBox().intersects(mSceneMgr->getEntity("Enemy2")->getWorldBoundingBox())){
		mDirection10 = mSceneMgr->getSceneNode("Enemy1")->getPosition()-mSceneMgr->getSceneNode("Enemy2")->getPosition();
		mDistance10 = mDirection10.normalise();
		do{
			mSceneMgr->getSceneNode("Enemy1")->translate(-mDirection10.x/10, 0,-mDirection10.z/10);
		}while(mDistance10>10);
	}*/
	/*if(mSceneMgr->getEntity("Enemy2")->getWorldBoundingBox().intersects(mSceneMgr->getEntity("Enemy3")->getWorldBoundingBox())){
		mDirection10 = mSceneMgr->getSceneNode("Enemy2")->getPosition()-mSceneMgr->getSceneNode("Enemy3")->getPosition();
		mDistance10 = mDirection10.normalise();
		do{
			mSceneMgr->getSceneNode("Enemy2")->translate(-mDirection10.x, 0,-mDirection10.z);
		}while(mDistance10>30);
	}*/

	
	if(mMain1->getPosition().x<=-800){
		do{
		mMain1->translate(50,0,0);
		}while(mMain1->getPosition().x<-770);
	}
	if(mMain1->getPosition().x>=850){
		do{
		mMain1->translate(-50,0,0);
		}while(mMain1->getPosition().x>820);
	}
	if(mMain1->getPosition().z<=-1200){
		do{
		mMain1->translate(0,0,50);
		}while(mMain1->getPosition().z<-1170);
	}
	if(mMain1->getPosition().z>=2500){
		do{
		mMain1->translate(0,0,-50);
		}while(mMain1->getPosition().z>2470);
	}

	// 시간 
	/*time_t timer;
	time(&timer);

	tm *WindowTime;

	WindowTime = localtime(&timer);

	cout<<WindowTime<<endl;*/

	//mTime= mTime
	--mTime;
	
	//std::cout<<mMain1->getPosition()<<std::endl;

	
	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	static Ogre::DisplayString point = L"Magic ";
	static Ogre::DisplayString endpoint = L" Point ";
	static Ogre::DisplayString gametime = L"Time:";
	static Ogre::DisplayString currFps = L"현재 FPS:";
	static Ogre::DisplayString avgFps = L"평균 FPS:";
	static Ogre::DisplayString bestFps = L"최고 FPS:";
	static Ogre::DisplayString worstFps = L"최저 FPS:";
	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	OverlayElement* guiPoint = OverlayManager::getSingleton().getOverlayElement("Point");
	OverlayElement* guiEndPoint = OverlayManager::getSingleton().getOverlayElement("EndPoint");
	OverlayElement* guiGameTime  = OverlayManager::getSingleton().getOverlayElement("GameTime");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();
	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));

	guiPoint->setCaption(point +StringConverter::toString(mPoint));
	guiEndPoint->setCaption(endpoint +StringConverter::toString(mPoint));
	guiGameTime->setCaption(gametime +StringConverter::toString(mTime/10));

	//mFlyOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Fly"+ StringConverter::toString(FlyState));
	mFlyOverlay->hide();
	mLifeOverlay->hide();

	mSceneMgr->destroyAllManualObjects();
	
	if( mTime <=0 || LifeState <=0){
		mEndPointOverlay->show();
		game->changeState(EndState::getInstance()); return true;
	}

	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	//if(mMain1->getPosition().x>-840 && mMain1->getPosition().x<830 && mMain1->getPosition().z>-1950 && mMain1->getPosition().z<2550){
	switch(e.key)
	{
	case OIS::KC_W: case OIS::KC_UP: 
		//if(mCharacterDirection.z<1)
		mCharacterDirection.z += 1.0f; break;
	case OIS::KC_S: case OIS::KC_DOWN: 
		//if(mCharacterDirection.z>-1)
		mCharacterDirection.z += -1.0f; break;
	case OIS::KC_A: case OIS::KC_LEFT: 
		//if(mCharacterDirection.x<1)
		mCharacterDirection.x += 1.0f; break;
	case OIS::KC_D: case OIS::KC_RIGHT: 
		//if(mCharacterDirection.x>-1)
		mCharacterDirection.x += -1.0f; break;
	case OIS::KC_SPACE:
		//mCharacterDirection.x +=5;
		if(mPoint>=0){
			jipangCreate(); 
		}
		break;
	case OIS::KC_ESCAPE:
		game->pushState(PauseState::getInstance()); break;
	case OIS::KC_C:
		if( LifeState < 10 ) LifeState++;
		else if( LifeState >=10 ) LifeState = 10;
		if( mPoint <0)
			mPoint =50;
		game->pushState(CardState::getInstance()); break;
	case OIS::KC_E:
		game->changeState(EndState::getInstance()); break;
	}

	return true;
}

bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch(e.key)
	{
	case OIS::KC_W: case OIS::KC_UP: 
		//if(mCharacterDirection.z<1)
		mCharacterDirection.z += 1.0f; break;
	case OIS::KC_S: case OIS::KC_DOWN: 
		//if(mCharacterDirection.z>-1)
		mCharacterDirection.z += -1.0f; break;
	case OIS::KC_A: case OIS::KC_LEFT: 
		//if(mCharacterDirection.x<1)
		mCharacterDirection.x += 1.0f; break;
	case OIS::KC_D: case OIS::KC_RIGHT: 
		//if(mCharacterDirection.x>-1)
		mCharacterDirection.x += -1.0f; break;
	case OIS::KC_SPACE: 
		if(mPoint>=0){
		jipangDestroy();
		} break;
	}

	return true;
}



bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(id ==OIS::MB_Right){
		mRMouseDown = true;
	}else if(id==OIS::MB_Left){
		mLMouseDown = true;
	}
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(id ==OIS::MB_Right){
		mRMouseDown = false;
	}else if(id == OIS::MB_Left){
		mLMouseDown = false;
	}
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	mMouseOverlay->getChild("UI_Mouse")->setPosition(e.state.X.abs -400, e.state.Y.abs -300);
	cameraHolder->translate(Ogre::Vector3(0, e.state.X.rel*0.02f, -e.state.Z.rel * 0.1f));

	if(mRMouseDown){
		/*cameraHolder->detachObject(mCamera);
		cameraHolder1->attachObject(mCamera);*/
		cameraYaw->yaw(Degree(-e.state.X.rel*0.2f));

		DeMax.y+=e.state.Y.rel;

		/*if(Degree(DeMax.y)>=Degree(180)) Degree(DeMax.y) = Degree(180);
		if(Degree(DeMax.y)<=Degree(-30)) Degree(DeMax.y) = Degree(-30);*/

		//if(Degree(DeMax.y)<=Degree(180) && Degree(DeMax.y)>=Degree(-60))    //180~-90
			//cameraPitch->pitch(Degree(-e.state.Y.rel*0.12f));
			//cameraPitch->pitch(Degree(-e.state.Y.rel*0.15f));
		/*if(Degree(e.state.Y.rel)<=Degree(180) && Degree(e.state.Y.rel)>=Degree(-60))
			cameraPitch->pitch(Degree(-e.state.Y.rel*0.15f));*/

		/*else{
		cameraPitch->pitch(Degree(-e.state.Y.rel*0.1f));
		DeMax.y-=e.state.Y.rel;
		}*/
		//cameraHolder->translate(Ogre::Vector3(0, e.state.X.rel*0.02f, -e.state.Z.rel * 0.1f));
		cameraHolder->translate(Ogre::Vector3(0,-e.state.Z.rel*0.1f,-e.state.Z.rel*0.1f));
	}
	//왼쪽 마우스 누르고 움직임
	else if(mLMouseDown){
		cameraYaw->yaw(Degree(-e.state.X.rel*0.2f));
		/*mMain1->setOrientation(cameraYaw->getOrientation());
		Quaternion quat = Ogre::Vector3(Ogre::Vector3::UNIT_Z).getRotationTo(mCharacterDirection);
		M1Yaw->setOrientation(quat);*/
	}
	//왼쪽 마우스 안 누르고 움직임
	/*else{cameraYaw->yaw(Degree(-e.state.X.rel*0.2f));
	mMain1->setOrientation(cameraYaw->getOrientation());
	Quaternion quat = Ogre::Vector3(Ogre::Vector3::UNIT_Z).getRotationTo(mCharacterDirection);
	M1Yaw->setOrientation(quat);
	cameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));

	}*/

	return true;
}

void PlayState::jipangCreate()
{
	Ogre::Vector3 objectposition;

	char name[16];
	sprintf( name, "jipang%d", mCount++ );
	ent = mSceneMgr->createEntity( name, "jipang.mesh" );
	mCurrentObject = mSceneMgr->getRootSceneNode( )->createChildSceneNode(
		name, mMain1->getPosition()
		);

	// 지팡이 위치 잡기
	mCurrentObject->attachObject( ent );
	mCurrentObject->setScale(50,50,50);

	objectposition = mCurrentObject->getPosition();
	mObjectList.push_back(objectposition);

}
void PlayState::setEnemy(int temp)
{
	char nameEnemy[EnemyNum];
	for(int i=0 ; i<EnemyNum ; ++i){
		if(i<3){
		pos.x = rand()%500-300;
		pos.y = -20.0f;
		pos.z = rand()%500-300;
		}else if(3<=i && i<7)
		{
		pos.x = -400+rand()%500-300;
		pos.y = -20.0f;
		pos.z = 900+rand()%500-300;
		}else
		{
		pos.x = 400+rand()%500-300;
		pos.y = -20.0f;
		pos.z = 900+rand()%500-300;
		}

		sprintf(nameEnemy,"Enemy%d",i);
		enemy = mSceneMgr->createEntity( nameEnemy, "Goram.mesh" );
		enemyObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			String(nameEnemy), pos
			);
		enemyObject->attachObject(enemy);
		//enemyObject->setScale(10,10,10);
		enemyObject->translate(0,0,-20,Node::TS_LOCAL);
		enemyObject->yaw(Degree(rand()%360));
		//enemyObject->yaw(Degree(30));
		//enemyObject->showBoundingBox(enemy);
		enemy->setCastShadows(true);
	}


}

void PlayState::setCoin()
{
	char nameCoin[30];
	for(int i=0; i<30; ++i){
		if(i<10){
			coinPos.x = rand()%600-300;
			coinPos.y = 0.0f;
			coinPos.z = rand()%600-300;
		}else if(10<=i && i<20)
		{
			coinPos.x = -400+rand()%600-300;
			coinPos.y = 0.0f;
			coinPos.z = 900+rand()%600-300;
		}else
		{
			coinPos.x = 400+rand()%600-300;
			coinPos.y = 0.0f;
			coinPos.z = 900+rand()%600-300;
		}
		sprintf(nameCoin,"Coin%d",i);
		Coin = mSceneMgr->createEntity(nameCoin, "coin.mesh");
		coinObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			String(nameCoin), coinPos
			);
		coinObject->attachObject(Coin);
		coinObject->translate(0,20,0);

	}
}

void PlayState::jipangDestroy()
{

	char name2[16];
	sprintf(name2, "jipang%d", mCount-4);
	mSceneMgr->destroyEntity(name2);

}

void PlayState::DrawLine()
{
	// 위치 가져오기 및 초기화
	Ogre::Vector3 position1 = mMain1->getPosition();
	Ogre::Vector3 position2 = mMain1->getPosition();
	Ogre::Vector3 position3 = mMain1->getPosition();
	Ogre::Vector3 position4 = mMain1->getPosition();



	//라인 로직
	char name1[16];
	sprintf( name1, "manual%d", mCount1++ );
	Ogre::ManualObject* manual = mSceneMgr->createManualObject(name1);
	manual->begin("BaseWhiteNoLighting", RenderOperation::OT_LINE_STRIP);

	if(mObjectList.size()>=3)
	{
		position4 = mObjectList.front();
		mObjectList.pop_front();
	}

	std::deque<Ogre::Vector3>::const_iterator pos;
	for(pos=mObjectList.begin(); pos != mObjectList.end(); ++pos){
		position2 = mObjectList.front();
	}

	for(pos=mObjectList.begin(); pos != mObjectList.end(); ++pos){
		position3 = mObjectList.back();
	}
	manual->position(position1);
	manual->position(position2);
	manual->position(position3);

	PositionCenter = Ogre::Vector3((position4.x+position2.x+position3.x)/3,(position4.y+position2.y+position2.y)/3
		,(position4.z+position2.z+position3.z)/3);
	distance = sqrt( (position2.x-PositionCenter.x)*(position2.x-PositionCenter.x)+(position2.z-PositionCenter.z)*(position2.z-PositionCenter.z));

	manual->index(0);
	manual->index(1);
	manual->index(2);
	manual->index(0); 

	manual->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

	if(mCount != 0 && mCount%3==0)
	{		
		length1 = length(position2,position3)+length(position3,position4)+length(position4,position2);

		for(int i =0; i<30; ++i)
		{
			char nameCoin[30];
			sprintf(nameCoin,"Coin%d",i);
			range(mSceneMgr->getSceneNode(nameCoin)->getPosition());
		}

		char name2[16];
		for(int i=1; i<4; i++){
			sprintf(name2, "jipang%d", mCount-i);
			mSceneMgr->destroyEntity(name2);
			//mSceneMgr->destroySceneNode(name2);
		}
		mSceneMgr->destroyManualObject(manual);
		mObjectList.clear();
		if(length1>0)
			std::cout<<length1/10<<std::endl;
			length2 = length1/10;
	}
	if(0!= length1)
		length1=0;
	mPoint -= length2;
	//std::cout<<mPoint<<std::endl;
}

void PlayState::CharacDestroy(Ogre::Vector3 temp)
{
	for(int i=0; i<30; ++i)
	{
		char nameCoin[30];
		sprintf(nameCoin,"Coin%d",i);
		if(temp==mSceneMgr->getSceneNode(nameCoin)->getPosition()){
			mSceneMgr->destroyEntity(nameCoin);
			mSceneMgr->getSceneNode(nameCoin)->setPosition(5000,0,50000);
		}
	}

	// Fmod 효과음
	m_pSystem->playSound(FMOD_CHANNEL_FREE, sound1, true, &channel1);

	channel1->setVolume(2.0f);
	channel1->setLoopCount(1);
	channel1->setPaused(false);

	mPoint += 50;
}

void PlayState::range(Ogre::Vector3 temp)
{
	distance1 = sqrt((temp.x-PositionCenter.x)*(temp.x-PositionCenter.x)+(temp.z-PositionCenter.z)*(temp.z-PositionCenter.z));

	if(distance > distance1){
		CharacDestroy(temp);
	}

}
// 공격 범위 체크
int PlayState::attackRange(Ogre::Vector3 temp2, Ogre::Vector3 temp3)
{
	attackDistance = sqrt((temp2.x-temp3.x)*(temp2.x-temp3.x)
		+(temp2.z-temp3.z)*(temp2.z-temp3.z));
	if(attackDistance >350.0f && attackDistance <600.0f){
		return 2;  //시선 이동
	}
	else if(attackDistance <=350.0f){
		return 3;  //따라감
	}
	else
		return 1;  //배회

}

// 공격 상태 변경
void PlayState::changeattackState(int i,SceneNode* temp)
{
	switch(i)
	{
	case 1:
		{
			/*for(int i=0; i<10; ++i){
				Enemy1AnimationState[i]->setEnabled(false);
				String stemp = temp->getName();
				Enemy1AnimationState[i]= mSceneMgr->getEntity(stemp)->getAnimationState("Normal");
				Enemy1AnimationState[i]->setLoop(true);
				Enemy1AnimationState[i]->setEnabled(true);
			}*/
		}
		break;
	case 2:
		{	
			
			temp2= temp->getPosition();
			mDestination[i]= mMain1->getPosition();
			mDirection[i] = mDestination[i] - temp2;
			mDirection[i] = Ogre::Vector3(mDirection[i].x,0,mDirection[i].z);
			Quaternion quat = Ogre::Vector3(Ogre::Vector3::UNIT_Z).getRotationTo(mDirection[i]);
			temp->setOrientation(quat);


		}
		break;
	case 3:
		{			
			
			temp3= temp->getPosition();
			mDestination[i]= mMain1->getPosition();
			mDirection[i] = mDestination[i] - temp3;
			mDistance[i] = mDirection[i].normalise();
			mDirection[i] = Ogre::Vector3(mDirection[i].x,0,mDirection[i].z);
			Quaternion quat = Ogre::Vector3(Ogre::Vector3::UNIT_Z).getRotationTo(mDirection[i]);
			temp->setOrientation(quat);
			Move = mWalkSpeed2 - (rand()%10)*0.3;
			mDistance[i] -=Move;
			if(mDistance[i]>35){
				
				temp->translate(Ogre::Vector3((mDirection[i].x)*Move/3, 0.0f, (mDirection[i].z)*Move/3));
			}
			else if(mDistance[i]<=35){
				--LifeState;
				std::cout<<LifeState<<std::endl;
				do{
					temp->translate(-(mDirection[i].x)*Move,0.0f,-(mDirection[i].z)*Move);
				}while(mDistance[i]>45);
			
			}
			
		}
		break;
	}

}
// 라이트 셋팅
void PlayState::setLight(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.0f,0.0f,0.0f));
	//mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_SPOTLIGHT);
	mLightD->setDirection(0.0f, -3.0f, 0.0f);
	mLightD->setPosition(0.0f, 0.0f, 0.0f);
	mLightD->setSpotlightRange( Degree(60), Degree(165) );
	mLightD->setDiffuseColour(1.0f, 1.0f, 1.0f);
	mLightD->setVisible(true);

	/*mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_POINT);
	mLightD->setPosition(0.0f, 0.0f, 0.0f);
	mLightD->setVisible(true);*/

}
// 오버레이 설정
void PlayState::setOverlay(void)
{	 

	// 생명력 표시
	switch( LifeState )
	{
	case 10:	mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life10"); break;
	case 9:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life9"); break;
	case 8:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life8"); break;
	case 7:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life7"); break;
	case 6:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life6"); break;
	case 5:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life5"); break;
	case 4:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life4"); break;
	case 3:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life3"); break;
	case 2:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life2"); break;
	case 1:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life1"); break;
	case 0:		mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life0"); break;
	default:	mLifeOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Life0"); break;
	}
	mLifeOverlay->getChild("UILifePanel0");	
	mLifeOverlay->show();

	// 플라잉게이지 표시

	mFlyOverlay= OverlayManager::getSingleton().getByName("Overlay/UI_Fly"+ StringConverter::toString(FlyState));
	//mFlyOverlay->show();

	mEndPointOverlay = mOverlayMgr->getSingleton().getByName("Overlay/EndPoint");
	//mEndPointOverlay->show();




	// Fill Here ---------------------------------
	mInformationOverlay= mOverlayMgr->getSingleton().getByName("Overlay/Information");
	mInformationOverlay->show();

	//-------------------------------------------
	mPointOverlay = mOverlayMgr->getSingleton().getByName("Overlay/Point");
	mPointOverlay->show();

}

int PlayState::length(Ogre::Vector3 temp2, Ogre::Vector3 temp3)
{
	return (int)sqrt((temp3.x-temp2.x)*(temp3.x-temp2.x)+(temp3.z-temp2.z)*(temp3.z-temp2.z));
}

// 바닥 그리는 함수
void PlayState::drawGroundPlane()
{
	// Fill Here -----------------------------------------------------------------------------
	Plane plane(Ogre::Vector3::UNIT_Y,0);
	MeshManager::getSingleton().createPlane(
		"Ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		1500,1500,
		10,10,
		true,1,5,5,
		Ogre::Vector3::NEGATIVE_UNIT_Z
		//Ogre::Vector3::UNIT_Z
		);
	Entity* groundEntity = mSceneMgr->createEntity("GroundPlane","Ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setMaterialName("KPU_LOGO");
	groundEntity->setCastShadows(false);
	// ---------------------------------------------------------------------------------------
}
bool PlayState::disconnected(int id)
{
	gmoteManager->disableButtonInterface(id);
	gmoteManager->disablePeakForceInterface(id);
	
	return true;
}

bool PlayState::connected(int id)
{
	gmoteManager->enableButtonInterface(id);
	gmoteManager->enablePeakForceInterface(id);
	gmoteManager->registerPeakForce(id, 10.0f);
	
	return true;
}

bool PlayState::accelerationCaptured(int id, float x, float y, float z)
{
	return true;
}

bool PlayState::buttonPressed(int id, int button, int buttons)
{
	switch(button)
	{
	case B_UP: mCharacterDirection.z += 1.0f; break;
	case B_DOWN: mCharacterDirection.z += -1.0f; break;
	case B_LEFT: mCharacterDirection.x += 1.0f; break;
	case B_RIGHT: mCharacterDirection.x += -1.0f; break;
	case B_EXTRA: mContinue = false; break;
	}
	return true;
}

bool PlayState::buttonReleased(int id, int button, int buttons)
{
	switch(button)
	{
	case B_UP: mCharacterDirection.z += 1.0f; break;
	case B_DOWN: mCharacterDirection.z += -1.0f; break;
	case B_LEFT: mCharacterDirection.x += 1.0f; break;
	case B_RIGHT: mCharacterDirection.x += -1.0f; break;
	}
	return true;
}
bool PlayState::rightFlagCaptured(int id, char rightFlag)
{
	 if(rightFlag)
	  {
		  mRightID = id;
	  }
	return true;
}

bool PlayState::forceDetected(int id, float x, float y, float z)
{
	return true;
}

bool PlayState::peakForceDetected(int id, float x, float y, float z, unsigned long period)
{
	if(id == mRightID)
	  {
		jipangCreate();
		
	  }
	else{

		//jipangDestroy();
	}
	
	return true;
}

bool PlayState::gestureDectected(int id, std::string &gestureName)
{
	return true;
}

bool PlayState::batteryChanged(int id, float battery)
{
	return true;
}