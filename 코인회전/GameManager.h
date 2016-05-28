#ifndef GameManager_H
#define GameManager_H

#include <vector>
#include <Ogre.h>
#include <OIS/OIS.h>

#include "Gmote/GmoteManager.h"

class GameState;

class GameManager : public Ogre::FrameListener,
	public OIS::KeyListener, public OIS::MouseListener, public GmoteListener
{
public:
	GameManager();
	~GameManager();

	void init(void);
	void changeState(GameState* state);
	void pushState(GameState* state);
	void popState();

	void go(void);

	void captureInput(void) { mKeyboard->capture(); mMouse->capture();gmoteManager->capture(); }
	bool isKeyDown(OIS::KeyCode kc) { return mKeyboard->isKeyDown(kc); }
	int getMouseRelativeX(void) { return mMouse->getMouseState().X.rel; }
	int getMouseRelativeY(void) { return mMouse->getMouseState().Y.rel; }
	bool getMouseButton(OIS::MouseButtonID mbid) { return mMouse->getMouseState().buttonDown(mbid); }

	bool mouseMoved( const OIS::MouseEvent &e );
    bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

    bool keyPressed( const OIS::KeyEvent &e );
    bool keyReleased( const OIS::KeyEvent &e );		

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
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;
    OIS::InputManager *mInputManager;

	void setupResources(void);
	//bool configure(void);
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);

private:
	std::vector<GameState*> states;
	Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
    Ogre::Viewport* mViewport;
};

#endif