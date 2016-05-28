#if !defined(__MOTIONER_H__)
#define __MOTIONER_H__


#include "Debug.h"
#include "Singleton.h"


#include <assert.h>
#include <list>
#include <set>
#include <map>



//! Button ID for gmote
enum GmoteButtonID
{
    GB_UP, GB_DOWN , GB_LEFT, GB_RIGHT, GB_SELECT, GB_EXTRA, GB_END
};

struct ss_info {
	int ss_id, old_ss_id;
};

struct PairingInfo {
	struct ss_info si[4];
	char sw_ver0, sw_ver1, dongle_id, slot_avail, param_flag;
};

struct GmoteState
{
    GmoteState() : buttons(0), battery(1.0f), yawVelocity(0.0f), gyro(0), rightFlag(0) {};


    float battery;

    float accelX, accelY, accelZ;

    float yawVelocity;
    int gyro;

	char rightFlag;

    //! represents all buttons - bit position indicates button down
    int buttons;

    //! Button down test
    inline bool buttonDown( int button ) const
    {
        return ((buttons & ( 1L << button )) == 0) ? false : true;
    }

    //! Clear all the values
    void clear()
    {
        battery = 1.0f;
        accelX = accelY = accelZ = 0.0f;
        buttons = 0;
    }
};


class Motioner 
{

public:
	Motioner() {}
	~Motioner() {}

	virtual bool init(void) = 0;

	virtual bool connectDongle(void) = 0;
	virtual bool disconnectDongle(void) = 0;

	// USB Dongle이 USB 포트에 꽃혀있고, 제대로 초기화가 되었는가?
	virtual bool dongleConnected(void) = 0;

	// 특정 센서(ID)가 연결되었는가?
	virtual bool sensorConnected(int id) = 0;

	// 현재 연결된 센서들의 ID list를 return 함.
	virtual std::set<int> getConnectedSensorIDSet(void) = 0;


	virtual void enableBuffering(int id) = 0;
	virtual void disableBuffering(int id) = 0;
    virtual void clearBuffer(int id) = 0;


    // sound play 용 API
    virtual void playSound(int id, int soundIndex) = 0;

	// pairing 용 API
    virtual void startPairing() = 0;
    virtual void endPairingWithSave() = 0;
    virtual void endPairingWithoutSave() = 0;

	virtual bool getState(int id, GmoteState &state) = 0;


	virtual char *getName(void) = 0;
};

    
#define motionerModel (MotionerModel::instance())

class MotionerModel : public SisaSingleton<MotionerModel>
{
public:
	void init(std::string name) { mName = name; }

	void registerConnection(int id) { mModel[id] = GmoteState(); PRINTF("# of connected motioners %d", mModel.size()); }
    void unregisterConnection(int id) { mModel.erase(id); PRINTF("# of connected motioners %d", mModel.size()); mNumDisconnections[id] += 1; }
	void updateAcceleration(int id, float x, float y, float z) { mModel[id].accelX = x; mModel[id].accelY = y; mModel[id].accelZ = z; }
    void updateYawVelocity(int id, float yawVelocity) { mModel[id].yawVelocity = yawVelocity; }
	void updateButton(int id, int buttons) { mModel[id].buttons = buttons; PRINTF("New Button Status %c", (buttons&0x20)==0?'0':'_'); 
	     PRINTF("%c%c", (buttons&0x10)==0?'0':'_', (buttons&0x08)==0?'0':'_'); 
	     PRINTF("%c%c", (buttons&0x04)==0?'0':'_', (buttons&0x02)==0?'0':'_'); 
	     PRINTF("%c\n", (buttons&0x01)==0?'0':'_'); }
	void updateBattery(int id, float battery) { mModel[id].battery = battery;  }
	void updateRightFlag(int id, char rightFlag) { mModel[id].rightFlag = rightFlag;  }

    int getNumDisconnections(int id) { return mNumDisconnections[id]; }
	int getNumConnectedIds(void) { return mModel.size(); }
	std::map<int, GmoteState>& getGmoteStateMap(void) { return mModel; }

	std::string getName(void) { return mName; }

private:
	std::string mName;
    std::map<int, GmoteState> mModel;
    std::map<int, int> mNumDisconnections;
};

#endif

