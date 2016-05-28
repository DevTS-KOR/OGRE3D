#pragma once

#include "inc/fmod.hpp"
#pragma comment (lib,"fmodex_vc.lib")
using namespace FMOD;

#include "GameState.h"
#include <fstream>
#include <iostream>

using namespace Ogre;

enum SOUONDKIND {
	SD_Stage1,
	SD_Effect
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
  void soundInit();
  void Release();

private:
  void _setLights(void);
  void _drawGroundPlane(void);
  void _drawGridPlane(void);
  void _drawScene(void);
  void _inputFile(void);
  void _makeTestStagePattern(void);
  void _constructTestStageSceneNode(void);
  void _controlCharacter(const Ogre::FrameEvent& evt);


  static PlayState mPlayState;
  std::vector<std::string> mInputFile;
  std::vector<Ogre::Vector3> mPattern;
  bool mCollisionCheck[90][7];

  Ogre::Root *mRoot;
  Ogre::RenderWindow* mWindow;
  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;

  Ogre::Light *mLightP, *mLightD, *mLightS;

  Ogre::SceneNode* mCharacterRoot;
  Ogre::SceneNode* mCharacterYaw;
  //Ogre::SceneNode* mCamera;
  Ogre::Vector3 mCharacterDirection;
  Ogre::SceneNode** mGroundNode;
  Ogre::SceneNode* mCameraRoot;
  Ogre::SceneNode* mCameraHolder;
  Ogre::SceneNode* mCameraYaw;
  Ogre::SceneNode* mCameraPitch;

  Ogre::Entity* mCharacterEntity;
  Ogre::Entity*	mGround;
  Ogre::Entity** mGroundEntity;

  Ogre::AnimationState* mAnimationState;

  Ogre::Overlay*           mInformationOverlay;

  // ����  �������� 3���� ������
  FMOD_SYSTEM *g_System; //���� �ý��� ���� �ϴ� �κ��̰�
  FMOD_SOUND *g_Sound[SD_Effect]; //���� �����ϴºκ�
  FMOD_CHANNEL *g_Channel[SD_Effect]; //ä�� �����ϴºκ��̰�
  FMOD_BOOL mIsPlaying;
};


