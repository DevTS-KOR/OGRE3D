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
  
  // ��ϵ� force ������ ū force�� �����Ǹ� ȣ���.
    bool forceDetected(int id, float x, float y, float z);
  
  // ��ϵ� peak force ���� ū peak force�� �����Ǹ� ȣ���.
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

	// ���� �ɸ���
	Entity *mMain1Entity;
	SceneNode *mMain1;

	//�� 
	Entity *mMLightEntity;
	SceneNode *mMLight;

	// ���� �ɸ��� �ִϸ��̼�
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
	//����
	Ogre::RaySceneQuery* mRaySceneQuery;

	//������ ��ġ�� �ֱ� ���� deque
	std::deque<Ogre::Vector3>mObjectList;
	Ogre::Vector3 mNowPosition;
	Ogre::Vector3 mNextPosition;

	// ���� ������ ��ǥ ����
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

	//�������� �Ŵ���
	Ogre::OverlayManager* mOverlayMgr;

	//���콺 ��������
	Ogre::Overlay* mMouseOverlay;

	// UI ����
	Ogre::Overlay* mPointOverlay;
	// UI �����
	Ogre::Overlay* mLifeOverlay;
	//int LifeState;
	// UI �ö��װ�����
	Ogre::Overlay* mFlyOverlay;
	int FlyState;

	//���� ����
	Ogre::Overlay* mEndPointOverlay;

	Ogre::Overlay* mInformationOverlay;
	Ogre::OverlayContainer* mPanel;

	//Ogre::OverlayContainer* Panel2;

	MOC::CollisionTools *mCollisionTools;

	// ����� �����ڵ�
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

	//��ƼŬ ���� ����
	Ogre::SceneNode* mFountainNode;


	// FMOD ����� ���õ� ����
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