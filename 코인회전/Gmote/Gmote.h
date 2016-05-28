#if !defined(__GMOTE_H__)
#define __GMOTE_H__

#include "Commthread.h"

#include "Motioner.h"



#if 0
// ���� ���� ����
struct Sample {
    bool mLowBattery;
    float mAccelX, mAccelY, mAccelZ;
    bool mButtonUp, mButtonDown, mButtonLeft, mButtonRight;
    bool mButtonSelect, mButtonExtra;
    int	mGyro;

    Sample() 
    {
        mLowBattery = false;
        mAccelX = mAccelY = mAccelZ = 0.0f;
        mButtonUp = mButtonDown = mButtonLeft = mButtonRight = mButtonSelect = mButtonExtra;
    }

    void randomize(void)
    {
        mAccelX = (rand() % 600) / 100.0f;
        mAccelY = (rand() % 600) / 100.0f;
        mAccelZ = (rand() % 600) / 100.0f;
        mButtonUp = (bool)(rand() % 2);
        mButtonDown = (bool)(rand() % 2);
        mButtonLeft = (bool)(rand() % 2);
        mButtonRight = (bool)(rand() % 2);
        mButtonSelect = (bool)(rand() % 2);
        mButtonExtra = (bool)(rand() % 2);
        mLowBattery = (bool)(((rand() % 100) < 20));
    }
};
#endif








// struct acceleration;
// accelerometers state:


struct acceleration {
    volatile BYTE  RawX, RawY, RawZ;
    volatile float X, Y, Z;

    struct orientation {
        volatile float X, Y, Z;
        volatile unsigned UpdateAge; // how many acceleration updates ago the last
        //  orientation estimate was made (if this
        //  value is high, the values are out-of-date
        //  and probably shouldn't be used).
        // Euler angle support (useful for some things).
        // * note that decomposing to Euler angles is complex, not always reliable,
        //    and also depends on your assumptions about the order each component
        //    is applied in.  you may need to handle this yourself for more
        //    complex scenarios *
        volatile float	 Pitch;	// in degrees (-180 - +180)
        volatile float	 Roll;	// "
        //volatile float Yaw;	
    } Orientation;
} ;


enum zmote_changed_flags {
    NO_CHANGE					 = 0,
    CONNECTION_LOST				 = 0x01,

    BATTERY_CHANGED				 = 0x02,
    LEDS_CHANGED				 = 0x04, // (probably redudant as zmmote never
    BUTTONS_CHANGED				 = 0x08, //  changes them unless requested)
    ACCEL_CHANGED				 = 0x10,
    ORIENTATION_CHANGED			 = 0x20,
    IR_CHANGED					 = 0x40,

    CHANGED_ALL = CONNECTION_LOST|BATTERY_CHANGED|LEDS_CHANGED|BUTTONS_CHANGED|
    ACCEL_CHANGED|ORIENTATION_CHANGED|IR_CHANGED,
};

#define Gmote ZMotioner

class ZMotioner : public Motioner
{
    // ���� read data�� ������ �ʰ�, buffer�� ��°�? 
    // ���� onBuffering ���¿�����, ���� ���α׷��� �������� ����� ���ؼ� �Է��� �޾Ƶ���.
    // ���� ���α׷�����, ������ ��� ���ο� ���� buffering ���θ� �������־�� ��.
    std::map<int, bool> onBuffering;

    // ����(ID)���� �Ҵ�� Buffer ��, ���� ID�� ������, �ش� buffer�� ������ �� ����.
    // ��) mSampleBuffer[345] --> ID #345 �� ������ Sample Buffer
    std::map<int, std::list<GmoteState>> mSampleBuffer;



    CRITICAL_SECTION cs;

public:
    ZMotioner(void);
    ~ZMotioner();

    void pushState(int id, GmoteState &state)
    {

        if (onBuffering[id]) 
        {
            EnterCriticalSection(&cs);
            mSampleBuffer[id].push_back(state);
            // buffer ���̴� 100
            if (mSampleBuffer[id].size() > 100)
            {
                NOTICE("Buffer Overrun");
                mSampleBuffer[id].pop_front();
            }
            LeaveCriticalSection(&cs);
        }

    }

    void enableBuffering(int id) { onBuffering[id] = true; }
    void disableBuffering(int id) { onBuffering[id] = false; }
    void clearBuffer(int id) { mSampleBuffer[id].clear(); }

	//-----------------------------------------------------------------
	// soundIndex�� ���� 4 bits : sound source �� index (1,2,3)
	// soundIndex�� ���� 4 bits : ����� sound volume ��(0,1,2,3)
	// soundIndex & 0x0F == 0 --> no sound !!
	//-----------------------------------------------------------------
    void playSound(int id, int soundIndex)
    {
		mAudioData = soundIndex;
		mAudioId = id;
		soundIndex = 1;
		if(soundIndex & 0x0F != 0)
		   mAudioFlag = true;
        Beep(340+50*(soundIndex&0x0F), 100 + 100 * (soundIndex&0x0F));
    }

	// data�� ���� 4 bit�� pairng�� �� slot ��ȣ�� �ǹ�
	// ���翡�� 0,2�� slot�� �����ϹǷ� data�� 0x05�� �־��־�� ��.
	// pairing ����� ������ ��Ʈ�ѷ��� ���� �̿��Ͽ� �� �� ����
	void startPairing()					// 2010.3.27
	{
		mPairingData = 0x05;
		mPairingFlag = true;
		mPairingOpFlag = true;
	}

	void startGetPairingInfo()			// 2010.3.27
	{
		mPairingOpFlag = true;
	}

	// pairing�� �����ϰ��� �ϴ� ��� data�� 0�� �־���.
	void endPairingWithoutSave()		// 2010.4.18
	{
		mPairingData = 0x00;
		mPairingFlag = true;
		mPairingOpFlag = false;
	}

	void endPairingWithSave()			// 2010.4.18
	{
		mPairingData = 0x80;
		mPairingFlag = true;
		mPairingOpFlag = false;
	}

	struct PairingInfo getPairingInfo()
	{
		return mPI;
	}

    bool getState(int id, GmoteState &state)
    {
        EnterCriticalSection(&cs);
        int size = mSampleBuffer[id].size();
        if (size) {
            state = mSampleBuffer[id].front();
            mSampleBuffer[id].pop_front();
        }
        LeaveCriticalSection(&cs);
        return (bool)size;
    }


    char *getName(void);

    bool init(void);

    bool connectDongle(void);

	bool findDonglePort(void);	
	// 2009.12.15 : Motioner.h�� �߻��Լ��� �������� ���� �ٶ��ϴ�.

    // USB Dongle�� USB ��Ʈ�� �����ְ�, ����� �ʱ�ȭ�� �Ǿ��°�?
    bool dongleConnected(void);

    // Ư�� ����(ID)�� ����Ǿ��°�?
    bool sensorConnected(int id);

    // ��� �������� ������ ����.
    bool disconnectDongle(void);

    std::set<int> getConnectedSensorIDSet(void);

    // ���� ������ ��� ������ ���� ���ø�.
    void sample(void);

    void ParseInput(unsigned char *pBuff, int len);
    short GetData (int slot, unsigned short* buff);
    short GetLastData (int slot, unsigned short* buff);

public: // convenience accessors:
    inline unsigned char	IsConnected		 () const { 
        unsigned char jjj=0;
        if((mSlotAvail & 0x01) == 0) jjj++;
        if((mSlotAvail & 0x02) == 0) jjj++;
        if((mSlotAvail & 0x04) == 0) jjj++;
        if((mSlotAvail & 0x08) == 0) jjj++;
        //if((mSlotAvail & 0x10) == 0) jjj++;
        return (jjj);	
    }	


    int   ParseAccel(BYTE* buff, char slot);
    bool  EstimateOrientationFrom(struct acceleration &accel, int s);
    short GetGyroData (char index);
    void  SendCalibData(unsigned char *data);
	// void  SendReqAudio(int id, int index);		// 2009.12.11


    bool mDongleConnected;
    UINT mTimerID;

    //int   mPrevNumSensors;
    //volatile bool  bStatusReceived; // for output method detection
    unsigned char  mSlotAvail;
    unsigned char  mPrevSlotAvail;
    bool mSlotChange;
    bool mCalibFlag;
	bool mAudioFlag;				// 2009.12.15
    bool mPairingFlag;				// 2010.3.27
	bool mPairingOpFlag;			// 2010.4.18
	struct PairingInfo mPI;			// 2010.4.18
    unsigned char  mChan;			// 2009.12.15
    unsigned char  mAudioData;		// 2009.12.15
	unsigned int   mAudioId;
    unsigned char mPairingData;		// 2010.3.27

    unsigned char mCalibData[14];
    unsigned char sendCnt;
    unsigned char execCnt;

    unsigned int mRcvdCnt;

    unsigned char AutoCalibFlag;
    // calibration information (stored on the zmote)
    struct calibration_info {
        volatile WORD X0, Y0, Z0;
        volatile WORD XG, YG, ZG;
    } CalibrationInfo[5], AutoCalib;

    struct raw_acc_data {
        volatile WORD X, Y, Z;
    } RawAccData, MinData, MaxData;

    unsigned char StopCnt;		// for Auto-Calibration

	unsigned char rl_Status;		// left or right sensor indication : 2010.3.23
    unsigned char mAcqCnt;		// counter for sensor data update
    unsigned char mPilotCnt;		// counter for broadcast of DONGLE
    unsigned char mPilotCntOld;	// old value of counter for broadcast of DONGLE
    unsigned char mDeadLockCnt;	// counter to determine deadlock state of DONGLE
    unsigned char gu8RTxMode;		
    unsigned char T1CNT;		

    unsigned long old_t, new_t;
    unsigned char mOldAcqCnt;		// counter for sensor data update
    unsigned char mDiffAcqCnt;	// counter for sensor data update

    bool mRebootDongle;
    bool mStartFlag;
    FILE *errLog;
    unsigned int mErrCnt1, mOKCnt1, mErrCnt2, mOKCnt2;

    unsigned long mSensorID[5];
    unsigned char mSensorVer[5];
    unsigned char sw_data[5];
    unsigned char DONGLE_ID;
    unsigned char DONGLE_VER;

    // accelerometers state:
    struct acceleration Acceleration[6];
    struct acceleration OldAccel;
    unsigned char hold_cnt;

    // orientation estimation
    unsigned	ZmoteNearGUpdates[6];


#define DBUF_SIZE	1000
    struct data_queue	{
        struct slot_data {
            short xdata, ydata, zdata, gdata;
        } s[DBUF_SIZE];
        short acq_index;
        short use_index;
    } Dat[5];

};


#define BYTE2LONG(l, px)	l=*(px), l=(l<<8)+*(px+1), l=(l<<8)+*(px+2), l=(l<<8)+*(px+3)
#define BYTE2WORD(l, px)	l=*(px), l=(l<<8)+*(px+1)

#endif // __GMOTE_H__
