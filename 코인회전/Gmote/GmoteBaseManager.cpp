#include "debug.h"
#include "WindowGameTimer.h"


#include <conio.h>

#include "GmoteBaseManager.h"


GmoteBaseManager::GmoteBaseManager(void)
{
    mGmote = NULL;
}

GmoteBaseManager::~GmoteBaseManager(void)
{
}

bool GmoteBaseManager::init(void)
{
    ASSERT(mGmote == NULL);

    mGmote = new Gmote;
    mGmote->init();


    // USB 동글이 연결된 COM 포트를 찾는다.
	if(mGmote->findDonglePort()) {
		PRINTF("Dongle Connected Successfully\n Press Any Key to Continue...\n");
	} else {
		PRINTF("Check Dongle Connected Properly and Middle LED of Dongle Blink Periodically\n");
//		getchar();
	}

    // 동글 연결에 약 1초 기다린다.
    masterGameTimer->start();
    while (!mGmote->connectDongle() && masterGameTimer->getTime() <= 1000)
    {
    }

    // 결국 연결이 안되면 초기화 실패
    if (!mGmote->connectDongle())
    {
        Beep(330, 10);
        Beep(330, 10);
        Beep(330, 10);
        PRINTF("Cannot connect Gmote Dongle !!");
        return false;
    }

    Beep(330, 500);
    PRINTF("Gmote Dongle Connected !!");
    return true;
}


void GmoteBaseManager::playSound(int id, int soundIndex)
{
    mGmote->playSound(id, soundIndex);
	PRINTF("Gmote sound play : %x - %x", id, soundIndex);
}

void GmoteBaseManager::startGetPairingInfo()
{
    mGmote->startGetPairingInfo();
	PRINTF("Gmote start get pairing info");
}

void GmoteBaseManager::startPairing()
{
    mGmote->startPairing();
	PRINTF("Gmote start pairing");
}

void GmoteBaseManager::endPairingWithSave()
{
    mGmote->endPairingWithSave();
	PRINTF("Gmote end pairing with save");
}

void GmoteBaseManager::endPairingWithoutSave()
{
    mGmote->endPairingWithoutSave();
	PRINTF("Gmote end pairing without save");
}

struct PairingInfo GmoteBaseManager::getPairingInfo()
{
	PRINTF("Gmote get Pairing Info");
	return mGmote->getPairingInfo();
}

void GmoteBaseManager::enableBuffering(int id)
{
    // 버퍼링을 시작하여, 센서 모듈로 하여금 데이타를 저장하도록 함.
    mGmote->enableBuffering(id);
    mPrevState[id] = GmoteState();
}

void GmoteBaseManager::disableBuffering(int id)
{
    mGmote->disableBuffering(id);
    mPrevState[id] = GmoteState();
}

int GmoteBaseManager::capture(void)
{

    // 연결 상황을 업데이트함.
    // 먼저 새롭게 연결된 sensor ID 와, 새롭게 끊긴 sensor ID set을 구함.
    std::set<int> curIDSet = mGmote->getConnectedSensorIDSet();

    std::set<int> newIDSet, oldIDSet;

    set_difference(curIDSet.begin(), curIDSet.end(), mPrevIDSet.begin(), mPrevIDSet.end(), std::inserter(newIDSet, newIDSet.begin()));
    set_difference(mPrevIDSet.begin(), mPrevIDSet.end(), curIDSet.begin(), curIDSet.end(), std::inserter(oldIDSet, oldIDSet.begin()));

    // 새롭게 연결된 sensor ID에 대한 처리 요청.
    std::set<int>::iterator itr;
    for (itr = newIDSet.begin(); itr != newIDSet.end(); itr++)
        mListener->connected(*itr);

    // 새롭게 끊긴 sensor ID에 대한 처리 요청
    for (itr = oldIDSet.begin(); itr != oldIDSet.end(); itr++)
        mListener->disconnected(*itr);

	mPrevIDSet = curIDSet;


	// 연결된 모든 센서 ID 별로 변동 상황 확인
	for (itr = curIDSet.begin(); itr != curIDSet.end(); itr++)
	{
		GmoteState curState;
		bool result = false;

		// buffer에 저장된 모든 샘플에 대해서 처리 진행.
		while (mGmote->getState(*itr, curState))
		{
			// battery 상태 변화 확인 및 이에 따른 listener call
			if (mPrevState[*itr].battery != curState.battery)
				result |= mListener->batteryChanged(*itr, curState.battery);

			// button 상태 변화 확인 및 연관 listerer call
			for (int i = 0; i < GB_END; i++)
			{
				if (curState.buttonDown(i) && !mPrevState[*itr].buttonDown(i))
					result |= mListener->buttonPressed(*itr, i, curState.buttons);
				if (!curState.buttonDown(i) && mPrevState[*itr].buttonDown(i))
					result |= mListener->buttonReleased(*itr, i, curState.buttons);
			}
			result |= mListener->accelerationCaptured(*itr, curState.accelX, curState.accelY, curState.accelZ);
			result |= mListener->rightFlagCaptured(*itr, curState.rightFlag);
			result |= mListener->gyroCaptured(*itr, curState.yawVelocity);

            mPrevState[*itr] = curState;
        }
        //if (result)
        //    mGmote->clearBuffer(*itr);
    }

    return true;
}


int GmoteBaseManager::quit(void)
{
    return true;
}

