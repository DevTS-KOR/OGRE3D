#include "Debug.h"

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <limits>

#include "OgreVector3.h"

#include "Libresample.h"

#include "Gesture.h"
#include "Recognition.h"
#include "Preprocessor.h"

bool Preprocessor::isStationaryAcceleration(Ogre::Vector3& accel)
{
    float len = accel.squaredLength();
    if (0.81f <= len && len <= 1.21) 
    {
        mGravity = accel;
        //PRINTF("New Gravity [%4.2f, %4.2f, %4.2f] (%4.2f) captured", accel.x, accel.y, accel.z, accel.length());
    }
    //return (0.16f <= len && len <= 2.56);
    return (len <= 4.0f);
}


std::vector<Ogre::Vector3>::iterator Preprocessor::findLastNotOf(std::vector<Ogre::Vector3>& array)
{
    std::vector<Ogre::Vector3>::iterator itr = array.end();
    while (--itr != array.begin() && isStationaryAcceleration(*itr))
        ;
    return itr;
}

std::vector<Ogre::Vector3>::iterator Preprocessor::findFirstNotOf(std::vector<Ogre::Vector3>& array)
{
    std::vector<Ogre::Vector3>::iterator itr = array.begin();
    while (itr != array.end() && isStationaryAcceleration(*itr))
        itr++;
    return itr;
}

void Preprocessor::quantizeSample(Ogre::Vector3& v)
{
    v.x = (int)(v.x * 10) / 10.0f; 
    v.y = (int)(v.y * 10) / 10.0f; 
    v.z = (int)(v.z * 10) / 10.0f; 
}

void Preprocessor::quantizeAccelerationArray(std::vector<Ogre::Vector3>& src)
{
    std::vector<Ogre::Vector3>::iterator itr;
    for (itr = src.begin(); itr != src.end(); itr++)
    {
        quantizeSample((*itr));
    }
}

void Preprocessor::removeStationarySamples(std::vector<Ogre::Vector3>& src)
{
    src.erase(findLastNotOf(src) + 1, src.end());
    src.erase(src.begin(), findFirstNotOf(src));
}

Ogre::Vector3 Preprocessor::calculateMean(std::vector<Ogre::Vector3>& src)
{
    Ogre::Vector3 mean(Ogre::Vector3::ZERO);
    std::vector<Ogre::Vector3>::iterator itr;
    for (itr = src.begin(); itr != src.end(); itr++)
        mean += *itr;

    mean /= src.size();
    PRINTF("Mean Value %f %f %f", mean.x, mean.y, mean.z);
    return mean;
}

void Preprocessor::removeGravity(std::vector<Ogre::Vector3>& src)
{
    //Ogre::Vector3 mean = calculateMean(src);
    std::vector<Ogre::Vector3>::iterator itr;
    for (itr = src.begin(); itr != src.end(); itr++)
        *itr -= mGravity;
}


void Preprocessor::normalizeByTime(std::vector<Ogre::Vector3>& src)
{
    static float ix[200], iy[200], iz[200];
    static float ox[50], oy[50], oz[50];
    std::vector<Ogre::Vector3> dest;

    for (int i = 0; i < src.size(); i++) 
    {
        ix[i] = src[i].x; iy[i] = src[i].y; iz[i] = src[i].z;
    }
    float factor = 32.0f / (float)src.size();
    void *handle = resample_open(1, factor, factor);
    int fwidth = resample_get_filter_width(handle);
    int inBufferUsed = 0;
    int outSampleCount = resample_process(handle, factor, 
        ix, src.size(), 
        true, &inBufferUsed,
        ox, 32);
    outSampleCount = resample_process(handle, factor, 
        iy, src.size(), 
        true, &inBufferUsed,
        oy, 32);
    outSampleCount = resample_process(handle, factor, 
        iz, src.size(), 
        true, &inBufferUsed,
        oz, 32);
    for (int i = 0; i < 32; i++)
    {
        dest.push_back(Ogre::Vector3(ox[i], oy[i], oz[i]));
    }

    swap(src, dest);
}


void Preprocessor::normalize(std::vector<Ogre::Vector3>& src)
{
    normalizeByTime(src);
    //normalizeByNorm(src);
}

void Preprocessor::normalizeByNorm(std::vector<Ogre::Vector3>& src)
{
    float norm = 0.0f;
    std::vector<Ogre::Vector3>::iterator itr;
    for (itr = src.begin(); itr != src.end(); itr++)
        norm += (*itr).squaredLength();
    norm /= src.size();

    for (itr = src.begin(); itr != src.end(); itr++)
        *itr /= norm;
}


void Preprocessor::quantize(std::vector<Recognition *>& recognitionArray)
{
    std::vector<Recognition *>::iterator itr;

    for (itr = recognitionArray.begin(); itr != recognitionArray.end(); itr++)
    {
        Recognition* recog = *itr;
        std::vector<Gesture *> gestureArray = recog->getGestureArray();
        std::vector<Gesture *>::iterator gitr;
        for (gitr = gestureArray.begin(); gitr != gestureArray.end(); gitr++)
        {
            Gesture* gesture = *gitr;
            quantizeAccelerationArray(gesture->getLocalAccelerationArray());
            removeStationarySamples(gesture->getLocalAccelerationArray());
        }
    }
}

