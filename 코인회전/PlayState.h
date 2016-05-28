#ifndef PlayState_H
#define PlayState_H

#include "Ogre.h"
#include "ExampleLoadingBar.h"
#include "GameState.h"
#include "CollisionTools.h"
#include <OgreOpcode.h>
#include "fmod.hpp"

#define EnemyNum 10

class PlayState : public GameState , public GmoteListener
{
public:
	void enter();
	void exit();

	void pause();
	void resume();

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool nextLocation(void);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e) ;
    bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id );
    bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

    bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
    bool keyReleased(GameManager* game, const OIS::KeyEvent &e);
	
	static PlayState* getInstance() { return &mPlayState; }
	
	int attackRange(Ogre::Vector3 temp2, Ogre::Vector3 temp3);
	void jipangCreate();
	void jipangDestroy();
	void setEnemy(int i);
	void setCoin();
	void DrawLine();
	void CharacDestroy(Ogre::Vector3 temp);
	void range(Ogre::Vector3 temp);
	void chase(Ogre::Vector3 temp);
	void changeattackState(int i,SceneNode* temp);

	void setLight();
	void drawGroundPlane(void);
	void setOverlay(void);
	int length(Ogre::Vector3 temp2, Ogre::Vector3 temp3);

	bool disconnected(int id);

    bool connected(int id);
    bool accelerationCaptured(int id, float x, float y, float z);

    bool buttonPressed(int id, int button, int buttons) ;


    bool buttonReleased(int id, int button, int buttons);

    bool rightFlagCaptured(int id, char rightFlag);
  
  // 등록된 force 값보다 큰 force가 감지되면 호출됨.
    bool forceDetected(int id, float x, float y, float z);
  
  // 등록된 peak force 보다 큰 peak force가 감지되면 호출됨.
    bool peakForceDetected(int id, float x, float y, float z, unsigned long period);
  
    bool gestureDectected(int id, std::string& gestureName);
  

    bool batteryChanged(int id, float battery);



protected:
	PlayState() { }

	Root *mRoot;
	SceneManager* mSceneMgr;
	Viewport* mViewport;
	Camera* mCamera;
    ExampleLoadingBar mLoadingBar;

	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;

	RenderWindow* mWindow;

	OIS::InputManager *mInputManager;

	// 메인 케릭터
	Entity *mMain1Entity;
	SceneNode *mMain1;

	//구 
	Entity *mMLightEntity;
	SceneNode *mMLight;

	// 메인 케릭터 애니메이션
	Ogre::AnimationState* mMainAnimationState;
	
	Entity *mEnemy1Entity;
	SceneNode* mEnemy1;

	Ogre::AnimationState* mEnemy1AnimationState;
	Ogre::AnimationState* tempAnimationState;

	Entity *mMain2Entity;
	SceneNode *mMain2;
	Ogre::AnimationState* mDanceState;

	Entity *mMain3Entity;
	SceneNode *mMain3;

	Entity *mMain4Entity;
	SceneNode *mMain4;

	Entity *mHead1Entity;
	SceneNode *mHead1;

	Entity *mHead2Entity;
	SceneNode *mHead2;

	Entity *mHead3Entity;
	SceneNode *mHead3;


	Entity *mMap1Entity;
	SceneNode *mMap1;

	Entity *ent;
	Ogre::SceneNode* mCurrentObject;
	Ogre::AnimationState* Enemy1AnimationState[EnemyNum];


	Entity *enemy;
	Ogre::SceneNode *enemyObject;

	Entity *Coin;
	Ogre::SceneNode* coinObject;
	
	SceneNode* M1Yaw ;
	SceneNode* cameraYaw;
	SceneNode* cameraPitch;
	SceneNode* cameraHolder;

	SceneNode* cameraYaw1;
	SceneNode* cameraPitch1;
	SceneNode* cameraHolder1;

	SceneNode* mCharacterRoot;
    SceneNode* mCharacterYaw;

 
	Ogre::Vector3 DeMax;
	Ogre::Vector3 pos;

	Ogre::Vector3 coinPos;
	//광선
	Ogre::RaySceneQuery* mRaySceneQuery;

	//지팡이 위치를 넣기 위한 deque
	std::deque<Ogre::Vector3>mObjectList;
	Ogre::Vector3 mNowPosition;
	Ogre::Vector3 mNextPosition;

	// 몹의 움직임 좌표 설정
	std::vector<Ogre::Vector3>mWalkList;


	Ogre::Vector3 mCharacterDirection;

	 bool mRotating;  
     Quaternion mSrcQuat, mDestQuat;  
     float mRotatingTime;

	 Ogre::Vector3 PositionCenter;
	 double distance;

	 Ogre::Vector3 temp;
	 Ogre::Vector3 temp2;
	 Ogre::Vector3 temp3;

	 double distance1;
	 double attackDistance;
	 double checkDistance;

	 std::deque<Vector3> mWalkList2;
	 Real mWalkSpeed2;
	 Real Move;
	 Vector3 mDirection2;
	 Real mDistance2;
	 Vector3 mDestination2;

	 Vector3 mDirection3;
	 Real mDistance3;
	 Vector3 mDestination3;

	 Vector3 mDirection4;
	 Real mDistance4;
	 Vector3 mDestination4;
	 
	 //Real Move[10];
	 Vector3 mDirection[10];
	 Real mDistance[10];
	 Vector3 mDestination[10];

	//오버레이 매니저
	Ogre::OverlayManager* mOverlayMgr;

	//마우스 오버레이
	Ogre::Overlay* mMouseOverlay;

	// UI 점수
	Ogre::Overlay* mPointOverlay;
	// UI 생명력
	Ogre::Overlay* mLifeOverlay;
	//int LifeState;
	// UI 플라잉게이지
	Ogre::Overlay* mFlyOverlay;
	int FlyState;

	//종료 점수
	Ogre::Overlay* mEndPointOverlay;

	Ogre::Overlay* mInformationOverlay;
	Ogre::OverlayContainer* mPanel;

	//Ogre::OverlayContainer* Panel2;

	MOC::CollisionTools *mCollisionTools;

	// 오우거 오피코드
	OgreOpcode::CollisionContext* mCollideContext;

	Ogre::Bone* mSwordTipBone;
    OgreOpcode::CollisionObject* mTipCollObj;
    OgreOpcode::BoxCollisionShape* mTipCollShape;
    Ogre::SceneNode* mTipNode;

    Ogre::Bone* mHeadBone;
    OgreOpcode::CollisionObject* mHeadCollObj;
    OgreOpcode::BoxCollisionShape* mHeadCollShape;
    Ogre::SceneNode* mHeadNode;

	Ogre::AxisAlignedBox* mAAB;

	Ogre::Vector3 mDirection10;
	Real mDistance10;
	Ogre::Vector3 mDirection1[20];
	Real mDistance1[20];
	
	int AtAnimation;

	//파티클 관련 변수
	Ogre::SceneNode* mFountainNode;


	// FMOD 사운드와 관련된 변수
	FMOD::System* m_pSystem;
	FMOD::Sound* m_pSound, *sound1, *sound2;
	FMOD::Channel* m_pChannel, *channel1, * channel2;

	
private:
	static PlayState mPlayState;
	int mCount;
	int mCount1;
	bool mContinue;
	bool mLMouseDown, mRMouseDown;
	int EnemyState[10];
	SceneNode* mLightNode;
	SceneNode* mLightNode1;
	SceneNode* mLightNode2;
	SceneNode* mLightNode3;

	int mPoint;
	int LifeState;

	int mTime;

	int length1;
	int length2;

	int mRightID;
	
//	Ogre::OverlayContainer* mPanel;
//	Ogre::OverlayContainer* mPanel2;
//	Ogre::Overlay* mLogoOverlay;

  Ogre::Light *mLightD, *mLightD2, *mLightP, *mLightS;
};

#endif