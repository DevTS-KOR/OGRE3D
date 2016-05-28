#if !defined(__GMOTE_BASE_MANAGER_H__)
#define __GMOTE_BASE_MANAGER_H__

#define gmoteBaseManager (GmoteBaseManager::instance())

#include "Singleton.h"


#include <set>
#include <map>
#include <algorithm>
#include <string>

#include "Gmote.h"



class GmoteBaseListener
{
public:
    virtual ~GmoteBaseListener() {}

    virtual bool disconnected(int id) = 0;
    virtual bool connected(int id) = 0;

    virtual bool accelerationCaptured(int id, float x, float y, float z) = 0;
	virtual bool rightFlagCaptured(int id, char rightFlag) = 0;
	virtual bool gyroCaptured(int id, float yawVelocity) = 0;

    // button : updated button no, buttons : all button state bits
    virtual bool buttonPressed(int id, int button, int buttons) = 0;
    virtual bool buttonReleased(int id, int button, int buttons) = 0;

    virtual bool batteryChanged(int id, float battery) = 0;
};


class GmoteBaseManager : public SisaSingleton<GmoteBaseManager>
{
public:
    GmoteBaseManager(void);
    ~GmoteBaseManager(void);

    bool init(void);
    int quit(void);

    void setEventListener(GmoteBaseListener *gl) { mListener = gl; }
    void enableBuffering(int id);
    void disableBuffering(int id);
    void playSound(int id, int soundIndex);
    void startGetPairingInfo();
    void startPairing();
    void endPairingWithoutSave();
    void endPairingWithSave();
	struct PairingInfo getPairingInfo();

    int capture(void);

private:

    GmoteBaseListener* mListener;
    Gmote* mGmote;
    std::set<int> mPrevIDSet;
    std::map<int, GmoteState> mPrevState;
};


#endif 