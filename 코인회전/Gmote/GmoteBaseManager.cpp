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


    // USB ������ ����� COM ��Ʈ�� ã�´�.
	if(mGmote->findDonglePort()) {
		PRINTF("Dongle Connected Successfully\n Press Any Key to Continue...\n");
	} else {
		PRINTF("Check Dongle Connected Properly and Middle LED of Dongle Blink Periodically\n");
//		getchar();
	}

    // ���� ���ῡ �� 1�� ��ٸ���.
    masterGameTimer->start();
    while (!mGmote->connectDongle() && masterGameTimer->getTime() <= 1000)
    {
    }

    // �ᱹ ������ �ȵǸ� �ʱ�ȭ ����
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
    // ���۸��� �����Ͽ�, ���� ���� �Ͽ��� ����Ÿ�� �����ϵ��� ��.
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

    // ���� ��Ȳ�� ������Ʈ��.
    // ���� ���Ӱ� ����� sensor ID ��, ���Ӱ� ���� sensor ID set�� ����.
    std::set<int> curIDSet = mGmote->getConnectedSensorIDSet();

    std::set<int> newIDSet, oldIDSet;

    set_difference(curIDSet.begin(), curIDSet.end(), mPrevIDSet.begin(), mPrevIDSet.end(), std::inserter(newIDSet, newIDSet.begin()));
    set_difference(mPrevIDSet.begin(), mPrevIDSet.end(), curIDSet.begin(), curIDSet.end(), std::inserter(oldIDSet, oldIDSet.begin()));

    // ���Ӱ� ����� sensor ID�� ���� ó�� ��û.
    std::set<int>::iterator itr;
    for (itr = newIDSet.begin(); itr != newIDSet.end(); itr++)
        mListener->connected(*itr);

    // ���Ӱ� ���� sensor ID�� ���� ó�� ��û
    for (itr = oldIDSet.begin(); itr != oldIDSet.end(); itr++)
        mListener->disconnected(*itr);

	mPrevIDSet = curIDSet;


	// ����� ��� ���� ID ���� ���� ��Ȳ Ȯ��
	for (itr = curIDSet.begin(); itr != curIDSet.end(); itr++)
	{
		GmoteState curState;
		bool result = false;

		// buffer�� ����� ��� ���ÿ� ���ؼ� ó�� ����.
		while (mGmote->getState(*itr, curState))
		{
			// battery ���� ��ȭ Ȯ�� �� �̿� ���� listener call
			if (mPrevState[*itr].battery != curState.battery)
				result |= mListener->batteryChanged(*itr, curState.battery);

			// button ���� ��ȭ Ȯ�� �� ���� listerer call
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

