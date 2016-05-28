#if !defined(__PREPROCESSOR_H__)
#define __PREPROCESSOR_H__

#include <map>
#include <vector>

#include "OgreVector3.h"

#include "Singleton.h"

#include "Recognition.h"
#include "Gesture.h"


#define preprocessor (Preprocessor::instance())

class Preprocessor : public SisaSingleton<Preprocessor>
{
public:
    void quantize(std::vector<Recognition *>& recognitionArray);
    void quantizeAccelerationArray(std::vector<Ogre::Vector3>& src);
    Ogre::Vector3 calculateMean(std::vector<Ogre::Vector3>& src);
    void removeGravity(std::vector<Ogre::Vector3>& src);
    void removeStationarySamples(std::vector<Ogre::Vector3>& src);
    void normalizeByTime(std::vector<Ogre::Vector3>& src);
    void normalizeByNorm(std::vector<Ogre::Vector3>& src);
    void normalize(std::vector<Ogre::Vector3>& src);
    bool isStationaryAcceleration(Ogre::Vector3& accel);
    void quantizeSample(Ogre::Vector3& v);

private:
    std::vector<Ogre::Vector3>::iterator findLastNotOf(std::vector<Ogre::Vector3>& array);
    std::vector<Ogre::Vector3>::iterator findFirstNotOf(std::vector<Ogre::Vector3>& array);

    Ogre::Vector3 mGravity;
};

#endif
