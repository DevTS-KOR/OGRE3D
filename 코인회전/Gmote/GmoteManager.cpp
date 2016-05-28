#include "Debug.h"
#include "WindowGameTimer.h"

#include "GestureFile.h"
#include "GmoteBaseManager.h"
#include "DTWRecognizer.h"
#include "Preprocessor.h"
#include "GmoteManager.h"


// GmoteManager Member Variable
GmoteListener *gmoteRecognitionListener;
std::map<int, bool> gestureInterfaceOn, dualGestureInterfaceOn, buttonInterfaceOn, forceInterfaceOn, peakForceInterfaceOn;

std::map<int, float> forceCondition;
std::map<int, float> peakForceCondition;

int mLeftID, mRightID;

std::map<int, std::vector<Recognition *>> mRecognitionArray;




//static char *buttonName[] = { "EXTRA", "UP", "LEFT", "SELECT", "RIGHT", "DOWN" };
static char *buttonName[] = { "UP", "DOWN", "LEFT", "RIGHT", "SELECT", "EXTRA" };






class EventListener : public GmoteBaseListener
{
public:
    EventListener(void) {}

    ~EventListener() {}

    virtual bool disconnected(int id)
    {
        PRINTF("Gmote(ID:#%08x) disconnected\n", id);
        gmoteBaseManager->disableBuffering(id);
        gmoteRecognitionListener->disconnected(id);

        return true;
    }

    virtual bool connected(int id)
    {
        //PRINTF("Gmote(ID:#%08x) connected\n", id);

        gmoteBaseManager->enableBuffering(id);

        gmoteRecognitionListener->connected(id);

        clearRecognitionState(id);

        return true;
    }

	virtual bool rightFlagCaptured(int id, char rightFlag)
	{
		gmoteRecognitionListener->rightFlagCaptured(id, rightFlag);
		return true;
	}

    virtual bool gyroCaptured(int id, float yawVelocity)
    {
        return true;
    }

    virtual bool accelerationCaptured(int id, float x, float y, float z)
    {
        //PRINTF("Gmote(ID:#%08x) Acceleration (%5.3f, %5.3f, %5.3f) captured\n", id, x, y, z);
        if (buttonInterfaceOn[id])
            gmoteRecognitionListener->accelerationCaptured(id, x, y, z);

        if (forceInterfaceOn[id] && _checkForceDetection(id, x, y, z))
            gmoteRecognitionListener->forceDetected(id, x, y, z);


        unsigned long period;
        if (peakForceInterfaceOn[id] && _checkPeakForceDetection(id, x, y, z, period))
            gmoteRecognitionListener->peakForceDetected(id, mCurPeakX[id], mCurPeakY[id], mCurPeakZ[id], period);


        std::string gesture;
        if (gestureInterfaceOn[id] && _checkGestureDetection(id, x, y, z, gesture))
        {
            gmoteRecognitionListener->gestureDectected(id, gesture);
        }

        return true;
    }

    virtual bool buttonPressed(int id, int button, int buttons)
    {
        //PRINTF("Gmote(ID:#%08x) Button %s pressed\n", id, buttonName[button]);
        gmoteRecognitionListener->buttonPressed(id, button, buttons);

        return true;
    }

    virtual bool buttonReleased(int id, int button, int buttons)
    {
        //PRINTF("Gmote(ID:#%08x) Button %s released\n", id, buttonName[button]);
        gmoteRecognitionListener->buttonReleased(id, button, buttons);

        return true;
    }

    virtual bool batteryChanged(int id, float battery)
    {
        //PRINTF("Gmote Battery %f\n", battery);
        gmoteRecognitionListener->batteryChanged(id, battery);

        return true;
    }


    void clearRecognitionState(int id)
    {
        mDynamicCount[id] = 0;
        mState[id] = GCS_START;
        mSkipCount[id] = 0;


        mFcState[id] = FCS_START;
        mForceWaitCount[id] = 0;

        mPfcState[id] = PFCS_START;
        mPeakForceWaitCount[id] = 0;

        mDynamicSampleBuffer[id].clear();
        mPrevRotation[id] = Ogre::Quaternion::IDENTITY;
    }

private:

    std::map<int, Ogre::Quaternion> mPrevRotation;
    std::map<int, int> mDynamicCount;
    enum { GCS_START = 0, GCS_STATIONARY, GCS_DYNAMIC, GCS_CAPTURING, GCS_SKIP };
    std::map<int, int> mState;
    std::map<int, int> mSkipCount;
    std::map<int, std::vector<Ogre::Vector3>> mDynamicSampleBuffer;


    std::map<int, int> mForceWaitCount;
    enum { FCS_START = 0, FCS_SKIP };
    std::map<int, int> mFcState;


    std::map<int, float> mCurPeakForce;
    std::map<int, float> mCurPeakX, mCurPeakY, mCurPeakZ;
    std::map<int, int> mPeakForceWaitCount;
    std::map<int, int> mPfcState; // Peak force check state
    enum { PFCS_START = 0, PFCS_PEAK, PFCS_SKIP, PFCS_FREQSTART };
    // PFCS_FREQSTART : peak detection의 주기를 측정하기 위해 한번 detection 된 후에는 FREQSTART로부터 시작됨.




    bool _checkGestureDetection(int id, float x, float y, float z, std::string& gesture)
    {
        bool curStationary;

        Ogre::Vector3 sample(x, y, z);
        curStationary = preprocessor->isStationaryAcceleration(sample);
        if (curStationary)
        {
            Ogre::Vector3 normalizedSample = sample;
            normalizedSample.normalise();
            mPrevRotation[id] = normalizedSample.getRotationTo(Ogre::Vector3::UNIT_Y);
        }
        else
        {
            // stationary
            sample = mPrevRotation[id] * sample;
        }
        preprocessor->quantizeSample(sample);


        bool gestureCaptured = false;

        switch (mState[id])
        {
        case GCS_START:
            mState[id] = curStationary ? GCS_STATIONARY : GCS_DYNAMIC; 
            break;
        case GCS_STATIONARY: 
            if (!curStationary) 
            {
                mState[id] = GCS_DYNAMIC;
                mDynamicCount[id] = 1;
                mDynamicSampleBuffer[id].push_back(sample);
                PRINTF("GCS_DYNAMIC");
            }
            break;
        case GCS_DYNAMIC:
            if (curStationary)
            {
                mState[id] = GCS_STATIONARY;
                mDynamicSampleBuffer[id].clear();
                PRINTF("GCS_STATIONARY");
            }
            else 
            {
                mDynamicCount[id]++;
                mDynamicSampleBuffer[id].push_back(sample);
                if (mDynamicCount[id] == 10) 
                {
                    mState[id] = GCS_CAPTURING;
                    PRINTF("GCS_CAPTURING");
                }
            }
            break;
        case GCS_CAPTURING:
            mDynamicCount[id]++;
            mDynamicSampleBuffer[id].push_back(sample);
            if (mDynamicCount[id] == 32)
            {
                gestureCaptured = true;
                mState[id] = GCS_SKIP;
                mSkipCount[id] = 0;
                PRINTF("GCS_SKIP");
            }
            break;
        case GCS_SKIP:
            mDynamicSampleBuffer[id].clear();
            mSkipCount[id]++;
            if (mSkipCount[id] == 20)
                mState[id] = GCS_STATIONARY;
            break;
        }


        if (gestureCaptured)
        {
            gmoteManager->disableGestureInterface(id);
            gesture = dtwRecognizer->decideGesture(mRecognitionArray[id], mDynamicSampleBuffer[id]);
            PRINTF("%d Samples is recognized as %s", mDynamicSampleBuffer[id].size(), gesture.c_str());
            mDynamicSampleBuffer[id].clear();
            gmoteManager->enableGestureInterface(id);
            return true;
        }
        else
        {
            gesture = "NO_DETECT";
            return false;
        }
    }




    bool _checkForceDetection(int id, float x, float y, float z)
    {

        Ogre::Vector3 sample(x, y, z);
        bool result;

        switch (mFcState[id])
        {
        case FCS_START:
            if (sample.squaredLength() >= forceCondition[id])
            {
                mFcState[id] = FCS_SKIP;
                mForceWaitCount[id] = 0;
                result = true;
            }
            else
                result = false;

              break;
        case FCS_SKIP:
            mForceWaitCount[id]++;
            if (mForceWaitCount[id] > 30)
            {
                mForceWaitCount[id] = 0;
                mFcState[id] = FCS_START;
            }
            result = false;
            break;
        }

        return result;
    }


    // peak force 감지 처리



    bool _checkPeakForceDetection(int id, float x, float y, float z, unsigned long& period)
    {
        static std::map<int, bool> prevPeakDetected;
        static std::map<int, long> prevDetectedTime;
        Ogre::Vector3 sample(x, y, z);
        bool result = false;

        switch (mPfcState[id])
        {
        case PFCS_START:
            prevPeakDetected[id] = false;
            if (sample.squaredLength() >= peakForceCondition[id])
            {
                mPfcState[id] = PFCS_PEAK;
                mCurPeakX[id] = x; mCurPeakY[id] = y; mCurPeakZ[id] = z;
                mCurPeakForce[id] = sample.squaredLength();
                //PRINTF("State PFCS_PEAK");
            }
            break;
        case PFCS_FREQSTART:
            if (sample.squaredLength() >= peakForceCondition[id])
            {
                mPfcState[id] = PFCS_PEAK;
                mCurPeakX[id] = x; mCurPeakY[id] = y; mCurPeakZ[id] = z;
                mCurPeakForce[id] = sample.squaredLength();
                //PRINTF("State PFCS_PEAK");
            }
            break;
        case PFCS_PEAK:
            if (sample.squaredLength() >= mCurPeakForce[id])
            {
                mCurPeakForce[id] = sample.squaredLength();
                mCurPeakX[id] = x; mCurPeakY[id] = y; mCurPeakZ[id] = z;
            }
            else
            {
                result = true;
                mPfcState[id] = PFCS_SKIP;
                mPeakForceWaitCount[id] = 1;
                if (prevPeakDetected[id])
                {
                    period = masterGameTimer->getTime() - prevDetectedTime[id];
                    prevDetectedTime[id] += period;
                }
                else
                {
                    period = 0;
                    prevPeakDetected[id] = true;
                    prevDetectedTime[id] = masterGameTimer->getTime();
                }
            }
            break;
        case PFCS_SKIP:
            mPeakForceWaitCount[id]++;
            if (mPeakForceWaitCount[id] > 20)
            {
                mPeakForceWaitCount[id] = 0;
                mPfcState[id] = PFCS_FREQSTART;
                //PRINTF("State PFCS_SKIP");
            }
            break;
        }

        return result;
    }
};




EventListener* mListener;



bool GmoteManager::init(void)
{
    gmoteRecognitionListener = NULL;
    mLeftID = mRightID = 0;

    if (!gmoteBaseManager->init())
        return false;

    mListener = new EventListener;
    gmoteBaseManager->setEventListener(mListener);

    return true;
}

void GmoteManager::readGestureFile(const char *fileName)
{
    PRINTF("Reading Gesture File '%s' ................", fileName);
    if (!gestureFile->readXmlFile(fileName))
        _error("readGestureFile", "cannot read gesture file");
    PRINTF("---------------------------------Done!!");
}

void GmoteManager::setEventListener(GmoteListener *listener)
{
    gmoteRecognitionListener = listener;
}







void GmoteManager::registerGestureSet(int id, ...)
{
    va_list ap;
    char *arg;
    int count = 0;

    va_start(ap,id);
    while (true)
    {
        count++;
        arg = va_arg(ap,char*);
        if (NULL == arg)
            break;
        if (count > 100)
            ASSERT_MSG(false, "Need to put NULL to the end of gesture set");

        Recognition *recog = gestureFile->getRecognition(std::string(arg));
        ASSERT_MSG(recog != NULL, "Undefined Gesture Name");
        _registerRecognition(id, recog);
    }
    va_end(ap);
}




void GmoteManager::registerGestureSetAll(int id)
{
    std::vector<Recognition *>::iterator itr;

    for (itr = gestureFile->getRecognitionArray().begin(); itr != gestureFile->getRecognitionArray().end(); itr++)
    {
        _registerRecognition(id, *itr);
    }
}

void GmoteManager::deleteGestureSetAll(int id)
{
    std::vector<Recognition *>::iterator itr;

    for (itr = gestureFile->getRecognitionArray().begin(); itr != gestureFile->getRecognitionArray().end(); itr++)
    {
        _deleteRecognition(id, (*itr)->getName());
    }
}



void GmoteManager::deleteGestureSet(int id, ...)
{
    va_list ap;
    char *arg;
    int count = 0;

    va_start(ap,id);
    while (true)
    {
        count++;
        arg = va_arg(ap,char*);
        if (NULL == arg)
            break;
        if (count > 100)
            ASSERT_MSG(false, "Need to put NULL to the end of gesture set");

       _deleteRecognition(id, std::string(arg));
    }
    va_end(ap);
}







void GmoteManager::registerForce(int id, float force)
{
    forceCondition[id] = force;
}


void GmoteManager::registerPeakForce(int id, float force)
{
    peakForceCondition[id] = force;
}



void GmoteManager::enableButtonInterface(int id)
{
    gmoteBaseManager->enableBuffering(id);
    buttonInterfaceOn[id] = true;
}

void GmoteManager::disableButtonInterface(int id)
{
    buttonInterfaceOn[id] = false;
    if (!gestureInterfaceOn[id] && !dualGestureInterfaceOn[id] && !forceInterfaceOn[id] && !peakForceInterfaceOn[id])
        gmoteBaseManager->disableBuffering(id);

}


void GmoteManager::enableForceInterface(int id)
{
    mListener->clearRecognitionState(id);
    gmoteBaseManager->enableBuffering(id);
    forceInterfaceOn[id] = true;
}

void GmoteManager::disableForceInterface(int id)
{
    forceInterfaceOn[id] = false;
    if (!gestureInterfaceOn[id] && !dualGestureInterfaceOn[id] && !buttonInterfaceOn[id] && !peakForceInterfaceOn[id])
        gmoteBaseManager->disableBuffering(id);
}


void GmoteManager::enablePeakForceInterface(int id)
{
    mListener->clearRecognitionState(id);
    gmoteBaseManager->enableBuffering(id);
    peakForceInterfaceOn[id] = true;
}

void GmoteManager::disablePeakForceInterface(int id)
{
    peakForceInterfaceOn[id] = false;
    if (!dualGestureInterfaceOn[id] && !gestureInterfaceOn[id] && !buttonInterfaceOn[id] && !forceInterfaceOn[id])
        gmoteBaseManager->disableBuffering(id);
}


void GmoteManager::enableGestureInterface(int id)
{
    mListener->clearRecognitionState(id);
    if (dualGestureInterfaceOn[id])
        _error("enableGestureInterface", "gesture interface and dual gesture interface cannot be both enabled");

    gmoteBaseManager->enableBuffering(id);
    gestureInterfaceOn[id] = true;
}

void GmoteManager::disableGestureInterface(int id)
{
    gestureInterfaceOn[id] = false;
    if (!dualGestureInterfaceOn[id] && !buttonInterfaceOn[id] && !forceInterfaceOn[id] && !peakForceInterfaceOn[id])
        gmoteBaseManager->disableBuffering(id);
}


void GmoteManager::capture(void)
{
    gmoteBaseManager->capture();
}

void GmoteManager::quit(void)
{
    gmoteBaseManager->quit();
}



void GmoteManager::_registerRecognition(int id, Recognition *recog)
{
    mRecognitionArray[id].push_back(recog);
}


void GmoteManager::_deleteRecognition(int id, std::string& recogName)
{
    // linear search
    std::vector<Recognition *>::iterator itr;

    for (itr = mRecognitionArray[id].begin(); itr != mRecognitionArray[id].end(); itr++)
    {
        if ((*itr)->getName() == recogName)
        {
            mRecognitionArray[id].erase(itr);
            break;
        }
    }
}


//-----------------------------------------------------------------------
void GmoteManager::_error(const std::string& loc, const std::string& msg)
{
    std::cout << std::endl << "Fatal Error! GmoteManager::" << loc << " - " << msg << std::endl; 
    exit(-1);
}

