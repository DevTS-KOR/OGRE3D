#pragma once

#include "GameState.h"
#include <iostream>
#include <fstream>
#include <fmod.hpp>
#pragma comment (lib,"fmodex_vc.lib")

struct tagSound
{
	FMOD::Sound      *Sound;
	FMOD::Channel    *Channel;
};

class PlayState : public GameState
{
public:
	void enter(void);
	void exit(void);

	void pause(void);
	void resume(void);

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent &e) ;
	bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

	static PlayState* getInstance() { return &mPlayState; }

private:

	void _setLights(void);
	void _drawGridPlane(void);
	void _inputFile(void);
	void _makeTestStagePattern(void);
	void _constructTestStageSceneNode(void);
	bool _checkCollision(void);

	void _controlCharacter(const Ogre::FrameEvent& evt);

	Ogre::SceneNode* mCollision;
	std::vector<std::string> mInputFile;
	std::vector<Ogre::Vector3> mPattern;
	bool mCollisionCheck[90][7];

	static PlayState mPlayState;

	Ogre::Root *mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	Ogre::SceneNode* mCameraNode;

	Ogre::Light *mLightP, *mLightD, *mLightS;

	Ogre::SceneNode* mProfessorNode;
	Ogre::SceneNode* mProfessorYaw;
	Ogre::Entity* mProfessorEntity;
	Ogre::Vector3 mProfessorDirection;
	
	Ogre::AnimationState* mAnimationState;
	enum{JUMP_UP, JUMPPING, JUMP_END, NONE};
	int mJump;
	bool mRespawn;

	Ogre::Overlay* mInformationOverlay;
	Ogre::Overlay* mPlayTimeOverlay;
	Ogre::Overlay* mCountTimeOverlay;
	Ogre::Overlay* mGameOverOverlay;
	Ogre::Overlay* mGameClearOverlay;

	float mGameTime;
	bool mStart;

	FMOD::System     *System; 
	unsigned int version;
	tagSound* SoundStruct;
};