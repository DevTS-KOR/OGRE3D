
#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameManager.h"
#include "GameState.h"

using namespace Ogre;

GameManager::GameManager()
{
  mRoot = 0;
}

GameManager::~GameManager()
{
  while (!states.empty()) {
    states.back()->exit();
    states.pop_back();
  }

  if (mRoot)
    delete mRoot;
}

void GameManager::init(void)
{
#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
	mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif

    if (!mRoot->restoreConfig()) {
			if (!mRoot->showConfigDialog()) return;
		}
	
	mWindow = mRoot->initialise(true,"2010 SILVERTOWN PROJECT ");
	setupResources();

	mSceneMgr = mRoot->createSceneManager(ST_GENERIC,"main");
	//mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE,"main");

	mCamera = mSceneMgr->createCamera("main");

	mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.0f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));



	mRoot->addFrameListener(this);

	//mWindow = mRoot->getAutoCreatedWindow();


    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
	OIS::ParamList pl;
	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem(pl); 
	
	
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
	mMouse = static_cast<OIS::Mouse*>( mInputManager->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);

	mMouse->getMouseState().width = mWindow->getWidth();
	mMouse->getMouseState().height = mWindow->getHeight();

	gmoteManager->init();
	gmoteManager->setEventListener(this);

}

void GameManager::go(void)
{
  if (mRoot)
    mRoot->startRendering();
}

void GameManager::changeState(GameState* state)
{
  if ( !states.empty() ) {
    states.back()->exit();
    states.pop_back();
  }
  states.push_back(state);
  states.back()->enter();
}

void GameManager::pushState(GameState* state)
{
  // pause current state
  if ( !states.empty() ) {
    states.back()->pause();
  }
  // store and init the new state
  states.push_back(state);
  states.back()->enter();
}

void GameManager::popState()
{
  // cleanup the current state
  if ( !states.empty() ) {
    states.back()->pause();
    states.pop_back();
  }
  // resume previous state
  if ( !states.empty() ) {
    states.back()->resume();
  }
}

void GameManager::setupResources(void)
{
	ResourceGroupManager::getSingleton().addResourceLocation("resource.zip","Zip");
	ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem");
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
 

}

//bool GameManager::configure(void)
//{
//  // load config settings from ogre.cfg
//  if (!mRoot->restoreConfig()) {
//    // if there is no config file, show the configuration dialog
//    if (mRoot->showConfigDialog()) {
//      // if returned true, user clicked OK so initialise
//      // and create a default rendering window
//      mWindow = mRoot->initialise(true);
//      return true;
//    }
//  }
//
//  // initialise and create a default rendering window
//  mWindow = mRoot->initialise(true);
//
//  return true;
//}

bool GameManager::frameStarted(const FrameEvent& evt)
{
  //////////////////////////////////////////////////
	gmoteManager->capture();
	if(mKeyboard) 
        mKeyboard->capture();
	if(mMouse)
		mMouse->capture();
  // call frameStarted of current state
  return states.back()->frameStarted(this, evt);


}

bool GameManager::frameEnded(const FrameEvent& evt)
{
  // call frameEnded of current state
  return states.back()->frameEnded(this, evt);

}

bool GameManager::mouseMoved(const OIS::MouseEvent &e) { return states.back()->mouseMoved(this, e); }
bool GameManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id ) { return states.back()->mousePressed(this, e, id); }
bool GameManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id ) { return states.back()->mouseReleased(this, e, id); }

bool GameManager::keyPressed(const OIS::KeyEvent &e) { return states.back()->keyPressed(this, e); }
bool GameManager::keyReleased(const OIS::KeyEvent &e) { return states.back()->keyReleased(this, e); }	


bool GameManager::disconnected(int id){

	gmoteManager->disableButtonInterface(id);
    gmoteManager->disablePeakForceInterface(id);


	return true;
}
bool GameManager::connected(int id){
	gmoteManager->enableButtonInterface(id);

    // Fill Here -----------------------------------------------
	gmoteManager->enablePeakForceInterface(id);
	gmoteManager->registerPeakForce(id, 10.0f);
   // ---------------------------------------------------------

	return true;
}

bool GameManager::accelerationCaptured(int id, float x, float y, float z) { return true; }

bool GameManager::buttonPressed(int id, int button, int buttons){
	return true;
}

bool GameManager::buttonReleased(int id, int button, int buttons){
	return true;
}

bool GameManager::rightFlagCaptured(int id, char rightFlag){
	return true;
}
bool GameManager::forceDetected(int id, float x, float y, float z){
	return true;
}

bool GameManager::peakForceDetected(int id, float x, float y, float z, unsigned long period){
	return true;
}

bool GameManager::gestureDectected(int id, std::string &gestureName){ 
	return true;
}

bool GameManager::batteryChanged(int id, float battery){
	return true;
}
