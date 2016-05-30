#define CLIENT_DESCRIPTION "InformationOverlay"

#include <Ogre.h>
#include <OIS/OIS.h>

using namespace Ogre;
using namespace std;

//게임매니저헤더, 게임스테이트헤더 
#include "GameManager.h"
#include "PlayState.h"
#include "StartState.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char *argv[])
#endif
	{
		// 게임 매니저 오브젝트
		GameManager game;


		try {
			//게임 초기화 및 첫 번째 상태넣기
			game.init();
			game.changeState(StartState::getInstance());
			game.go();

		} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
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

