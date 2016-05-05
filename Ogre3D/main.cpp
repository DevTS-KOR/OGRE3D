#define CLIENT_DESCRIPTION "Ogre3D Project"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>
#include <iostream>
using namespace std;

using namespace Ogre;

class InputController : public FrameListener,
	public OIS::KeyListener,
	public OIS::MouseListener
{

public:
	InputController(Root* root, OIS::Keyboard *keyboard, OIS::Mouse *mouse) : mRoot(root), mKeyboard(keyboard), mMouse(mouse)
	{
		mCamera = mRoot->getSceneManager("main")->getCamera("main");
		mCameraMoveVector = Ogre::Vector3::ZERO;

		mContinue = true;

		keyboard->setEventCallback(this);
		mouse->setEventCallback(this);
	}


	bool frameStarted(const FrameEvent &evt)
	{
		mKeyboard->capture();
		mMouse->capture();

		mCamera->moveRelative(mCameraMoveVector);

		return mContinue;
	}

	// Key Linstener Interface Implementation

	bool keyPressed(const OIS::KeyEvent &evt)
	{
		switch (evt.key)
		{
		case OIS::KC_W: mCameraMoveVector.y += 1; break;
		case OIS::KC_S: mCameraMoveVector.y -= 1; break;
		case OIS::KC_A: mCameraMoveVector.x -= 1; break;
		case OIS::KC_D: mCameraMoveVector.x += 1; break;
		case OIS::KC_ESCAPE: mContinue = false; break;
		}

		return true;
	}

	bool keyReleased(const OIS::KeyEvent &evt)
	{

		switch (evt.key)
		{
		case OIS::KC_W: mCameraMoveVector.y -= 1; break;
		case OIS::KC_S: mCameraMoveVector.y += 1; break;
		case OIS::KC_A: mCameraMoveVector.x += 1; break;
		case OIS::KC_D: mCameraMoveVector.x -= 1; break;
		case OIS::KC_ESCAPE: mContinue = false; break;
		}

		return true;
	}


	// Mouse Listener Interface Implementation

	bool mouseMoved(const OIS::MouseEvent &evt)
	{
		if (evt.state.buttonDown(OIS::MB_Right)) {
			mCamera->yaw(Degree(-evt.state.X.rel));
			mCamera->pitch(Degree(-evt.state.Y.rel));
		}

		mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.1f));

		return true;
	}

	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return true;
	}

	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return true;
	}


private:
	bool mContinue;
	Ogre::Root* mRoot;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	Camera* mCamera;

	Ogre::Vector3 mCameraMoveVector;
};



class OgreController : public FrameListener, public OIS::KeyListener
{

public:
	OgreController(Root* root, OIS::Keyboard *keyboard) : mKeyboard(keyboard)
	{
		mOgreNode = root->getSceneManager("main")->getSceneNode("Ogre");
		mOgreEntity = root->getSceneManager("main")->getEntity("Ogre");

		AllocConsole();
		freopen("CONOUT$", "wt", stdout);

		keyboard->setEventCallback(this);

		mWalkSpeed = 80.0f;
		mDirection = Vector3::ZERO;
		mBeforeDir = Vector3::ZERO;
		mRotating = false;
		mIdle = true;
		mKeyInput = true;
	}

	bool frameStarted(const FrameEvent &evt)
	{
		mKeyboard->capture();
		if (mDirection == Vector3::ZERO)
		{

			mAnimationState = mOgreEntity->getAnimationState("Dance");

		}


		// Fill Here ===============================================================
		else if (mRotating)
		{
			mAnimationState = mOgreEntity->getAnimationState("Dance");
			static const float ROTATION_TIME = 0.3f;
			mRotatingTime = (mRotatingTime > ROTATION_TIME) ? ROTATION_TIME : mRotatingTime;
			Quaternion delta = Quaternion::Slerp(mRotatingTime / ROTATION_TIME, mSrcQuat, mDestQuat, true);
			mOgreNode->setOrientation(delta);
			if (mRotatingTime >= ROTATION_TIME)
			{
				mRotatingTime = 0.0f;
				mRotating = false;
				mOgreNode->setOrientation(mDestQuat);
			}
			else
				mRotatingTime += evt.timeSinceLastFrame;
		}
		// ==========================================================================
		else
		{
			mAnimationState = mOgreEntity->getAnimationState("RunBase");

			Real move = mWalkSpeed * evt.timeSinceLastFrame; // 이동량 계산
			mDistance -= move; // 남은 거리 계산
			mOgreNode->translate(mDirection * move);

		}

		mAnimationState->setLoop(true);
		mAnimationState->setEnabled(true);
		mAnimationState->addTime(evt.timeSinceLastFrame);

		return mKeyInput;
	}

	bool keyPressed(const OIS::KeyEvent &evt)
	{
		switch (evt.key)
		{
		case OIS::KC_UP:
			mBeforeDir = mDirection;
			mDirection.z -= 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_DOWN:
			mBeforeDir = mDirection;
			mDirection.z += 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_LEFT:
			mOgreNode->setPosition(Vector3(-100, 100, 600));
			/*mBeforeDir = mDirection;
			mDirection.x -= 3;
			OgreStateChange(mBeforeDir, mDirection);*/
			break;

		case OIS::KC_RIGHT:
			mOgreNode->setPosition(Vector3(100, 100, 600));
			//mBeforeDir = mDirection;
			//mDirection.x += 3;
			//OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_ESCAPE: mKeyInput = false; break;
		}
		return true;
	}

	bool keyReleased(const OIS::KeyEvent &evt)
	{
		switch (evt.key)
		{
		case OIS::KC_UP:
			mBeforeDir = mDirection;
			mDirection.z += 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_DOWN:
			mBeforeDir = mDirection;
			mDirection.z -= 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_LEFT:
			mBeforeDir = mDirection;
			mDirection.x += 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_RIGHT:
			mBeforeDir = mDirection;
			mDirection.x -= 3;
			OgreStateChange(mBeforeDir, mDirection);
			break;

		case OIS::KC_ESCAPE: mKeyInput = false; break;
		}

		return true;
	}

	bool OgreStateChange(Vector3& beforeMoveDir, Vector3& afterMoveDir)
	{
		Vector3 CurrentMoveDir = afterMoveDir;
		Vector3 BeforeMoveDir = beforeMoveDir;
		CurrentMoveDir.normalise();
		BeforeMoveDir.normalise();

		if (CurrentMoveDir == BeforeMoveDir)
			return false;

		mSrcQuat = mOgreNode->getOrientation();
		mDestQuat = Vector3(Vector3::UNIT_Z).getRotationTo(CurrentMoveDir);

		mRotating = true;
		mRotatingTime = 0.f;

		return true;
	}

	bool nextLocation(void)
	{
		//mDirection = mDestination; - mOgreNode->getPosition(); // 방향 계산
		//mDistance = mDirection.normalise(); // 거리 계산

		// Fill Here =============================================================
		mSrcQuat = mOgreNode->getOrientation();
		mDestQuat = Vector3(Vector3::UNIT_Z).getRotationTo(mDirection);
		mRotating = true;
		mRotatingTime = 0.0f;
		// =======================================================================

		return true;
	}

private:
	SceneNode *mOgreNode;
	Ogre::Entity *mOgreEntity;
	Ogre::AnimationState* mAnimationState;

	OIS::Keyboard* mKeyboard;

	Real mWalkSpeed;
	Vector3 mDirection;
	Real mDistance;
	Vector3 mDestination;

	bool mKeyInput;
	bool mRotating;
	bool mIdle;
	float mRotatingTime;
	Quaternion mSrcQuat, mDestQuat;
	Vector3 mBeforeDir;
};

class NinjaController : public FrameListener
{
public:
	NinjaController(Root* root)
	{
		mNinjaNode = root->getSceneManager("main")->getSceneNode("Ninja");
		mNinjaEntity = root->getSceneManager("main")->getEntity("Ninja");
		mOgreNode = root->getSceneManager("main")->getSceneNode("Ogre");
		mOgreEntity = root->getSceneManager("main")->getEntity("Ogre");

		mWalkSpeed = 150.0f;
		mDirection = Vector3::ZERO;
		mRotating = false;
		follow = false;

		//mNinjaWalkList.push_back(Vector3(0.0f, 0.0f, 0.0f));

		mNinjaWalkList.push_back(Vector3(400.0f, 0.0f, 400.0f));
		mNinjaWalkList.push_back(Vector3(-400.0f, 0.0f, 400.0f));
		mNinjaWalkList.push_back(Vector3(-400.0f, 0.0f, -400.0f));
		mNinjaWalkList.push_back(Vector3(400.0f, 0.0f, -400.0f));

		//mNinjaWalkList.push_back(Ogre::Vector3::ZERO);
	}

	bool frameStarted(const FrameEvent &evt)
	{
		const Vector3 OgrePos = mOgreNode->getPosition();
		const Vector3 ninPos = mNinjaNode->getPosition();

		if (!follow)
		{

			if (ninPos.distance(OgrePos) < 200.f)
			{
				follow = true;
				nextLocation();
			}
		}

		if (ninPos.distance(OgrePos) > 200.f)
			follow = false;

		if (mDirection == Vector3::ZERO)
		{
			if (nextLocation())
			{
				mAnimationState = mNinjaEntity->getAnimationState("Walk");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
			}
		}
		// Fill Here ===============================================================
		else if (mRotating)
		{
			static const float ROTATION_TIME = 0.3f;
			mRotatingTime = (mRotatingTime > ROTATION_TIME) ? ROTATION_TIME : mRotatingTime;
			Quaternion delta = Quaternion::Slerp(mRotatingTime / ROTATION_TIME, mSrcQuat, mDestQuat, true);
			mNinjaNode->setOrientation(delta);
			if (mRotatingTime >= ROTATION_TIME)
			{
				mRotatingTime = 0.0f;
				mRotating = false;
				mNinjaNode->setOrientation(mDestQuat);
			}
			else
				mRotatingTime += evt.timeSinceLastFrame;
		}
		// ==========================================================================
		else
		{

			Real move = mWalkSpeed * evt.timeSinceLastFrame; // 이동량 계산
			mDistance -= move; // 남은 거리 계산

			if (mDistance <= 0.0f)
			{ // 목표 지점에 다 왔으면…

				mNinjaNode->setPosition(mDestination); // 목표 지점에 캐릭터를 위치
				mDirection = Vector3::ZERO; // 정지 상태로 들어간다.
				nextLocation();
			}
			else
			{
				mNinjaNode->translate(mDirection * move);
			}
		}

		mAnimationState->addTime(evt.timeSinceLastFrame);

		return true;
	}

	bool nextLocation(void)
	{
		if (mNinjaWalkList.empty() && follow == false)  // 더 이상 목표 지점이 없으면 false 리턴
		{
			mNinjaWalkList.push_back(Vector3(400.0f, 0.0f, 400.0f));
			mNinjaWalkList.push_back(Vector3(-400.0f, 0.0f, 400.0f));
			mNinjaWalkList.push_back(Vector3(-400.0f, 0.0f, -400.0f));
			mNinjaWalkList.push_back(Vector3(400.0f, 0.0f, -400.0f));
		}


		if (!follow)
		{
			mDestination = mNinjaWalkList.front(); // 큐의 가장 앞에서 꺼내기
			mNinjaWalkList.pop_front(); // 가장 앞 포인트를 제거
			mNinjaWalkList.push_back(mDestination);
			mDirection = mDestination - mNinjaNode->getPosition(); // 방향 계산
			mDistance = mDirection.normalise(); // 거리 계산
		}
		else
		{
			mNinjaWalkList.clear();
			Vector3 targetPos = mOgreNode->getPosition();
			mDestination = targetPos;
			//mNinjaWalkFollowList.push_back(mDestination);
			mDirection = targetPos - mNinjaNode->getPosition(); // 방향 계산
			mDistance = mDirection.normalise(); // 거리 계산
		}
		// Fill Here =============================================================
		mSrcQuat = mNinjaNode->getOrientation();
		mDestQuat = Vector3(Vector3::UNIT_Z).getRotationTo(-mDirection);
		mRotating = true;
		mRotatingTime = 0.0f;
		// =======================================================================

		return true;
	}

private:
	SceneNode *mNinjaNode;
	Ogre::Entity *mNinjaEntity;
	Ogre::AnimationState* mAnimationState;

	std::deque<Vector3> mNinjaWalkList;
	Real mWalkSpeed;
	Vector3 mDirection;
	Real mDistance;
	Vector3 mDestination;
	Vector3 mLastPosition;


	bool mRotating;
	bool follow;
	float mRotatingTime;
	Quaternion mSrcQuat, mDestQuat;
	SceneNode* mOgreNode;
	Ogre::Entity *mOgreEntity;
};

class LectureApp {

	Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	Camera* mCamera;
	Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;

	OIS::InputManager *mInputManager;



public:

	LectureApp() {}

	~LectureApp() {}

	void go(void)
	{
		// OGRE의 메인 루트 오브젝트 생성
#if !defined(_DEBUG)
		mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
		mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


		// 초기 시작의 컨피규레이션 설정 - ogre.cfg 이용
		if (!mRoot->restoreConfig()) {
			if (!mRoot->showConfigDialog()) return;
		}

		mWindow = mRoot->initialise(true, CLIENT_DESCRIPTION " : Music Run!");

		mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
		mCamera = mSceneMgr->createCamera("main");


		mCamera->setPosition(0.0f, 200.0f, 900.0f);
		mCamera->lookAt(0.0f, 100.0f, 0.0f);

		mViewport = mWindow->addViewport(mCamera);
		mViewport->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.5f));
		mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));


		ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

		// 좌표축 표시
		Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "wooden.mesh");
		mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
		mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

		_drawGridPlane();
		_drawGroundPlane();

		Entity* entity1 = mSceneMgr->createEntity("Ogre", "Sinbad.mesh");
		SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ogre", Vector3(0.0f, 100.0f, 600.0f));
		node1->yaw(Degree(180));
		node1->setScale(Vector3(10, 10, 10));
		node1->attachObject(entity1);

		Entity* entity2 = mSceneMgr->createEntity("Ninja", "ninja.mesh");
		SceneNode* node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Ninja", Vector3(400.0f, 0.0f, -400.0f));
		node2->attachObject(entity2);
		node2->setOrientation(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y));

		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		OIS::ParamList pl;
		mWindow->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
		mInputManager = OIS::InputManager::createInputSystem(pl);


		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

		InputController* inputController = new InputController(mRoot, mKeyboard, mMouse);
		mRoot->addFrameListener(inputController);

		OgreController* ogreController = new OgreController(mRoot, mKeyboard);
		mRoot->addFrameListener(ogreController);

		NinjaController* ninjaController = new NinjaController(mRoot);
		mRoot->addFrameListener(ninjaController);

		mRoot->startRendering();

		mInputManager->destroyInputObject(mKeyboard);
		mInputManager->destroyInputObject(mMouse);
		OIS::InputManager::destroyInputSystem(mInputManager);

		delete ogreController;
		delete ninjaController;
		delete inputController;

		delete mRoot;
	}

private:
	void _drawGridPlane(void)
	{
		Ogre::ManualObject* gridPlane = mSceneMgr->createManualObject("GridPlane");
		Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode");

		Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial", "General");
		gridPlaneMaterial->setReceiveShadows(false);
		gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 0);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1, 1, 1);
		gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1, 1, 1);

		gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST);
		for (int i = 0; i < 21; i++)
		{
			gridPlane->position(-500.0f, 0.0f, 500.0f - i * 50);
			gridPlane->position(500.0f, 0.0f, 500.0f - i * 50);

			gridPlane->position(-500.f + i * 50, 0.f, 500.0f);
			gridPlane->position(-500.f + i * 50, 0.f, -500.f);
		}

		gridPlane->end();

		gridPlaneNode->attachObject(gridPlane);
	}

	void _drawGroundPlane(void)
	{
		Plane plane(Vector3::UNIT_Y, 0);
		MeshManager::getSingleton().createPlane(
			"Ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane,
			1000, 1000,
			1, 1,
			false, 1, 1, 1,
			Vector3::NEGATIVE_UNIT_Z
			);

		Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
		groundEntity->setMaterialName("Map");
		groundEntity->setCastShadows(false);
	}
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
		LectureApp app;

		try {

			app.go();

		}
		catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif

