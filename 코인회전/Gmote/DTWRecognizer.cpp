#include "dtw.h"
#include "DTWRecognizer.h"
#include "GestureFile.h"
#include <functional>

#include "OgreVector3.h"

#include "Debug.h"




using namespace std;

float ogreVectorLength(Ogre::Vector3 v)
{
  return v.squaredLength();
}

void DTWRecognizer::registerRecognition(Recognition *recog)
{
    mRecognitionArray.push_back(recog);
}


void DTWRecognizer::deleteRecognition(std::string& recogName)
{
    // linear search
    std::vector<Recognition *>::iterator itr;

    for (itr = mRecognitionArray.begin(); itr != mRecognitionArray.end(); itr++)
    {
        if ((*itr)->getName() == recogName)
        {
            mRecognitionArray.erase(itr);
            break;
        }
    }
}


std::string& DTWRecognizer::decideGesture(std::vector<Ogre::Vector3>& inputPattern)
{
  float minDistance = numeric_limits<float>::max();
  Recognition *minRecog = NULL;
  int minNumSamples;
  static std::string noDetect("NO_DETECT");

  std::vector<Recognition *>::iterator itr1 = mRecognitionArray.begin();


  for ( ; itr1 != mRecognitionArray.end(); itr1++)
  {
      Recognition *dataRecog = *itr1;
      std::vector<Gesture *>::iterator itr2 = dataRecog->getGestureArray().begin();
      for ( ; itr2 != dataRecog->getGestureArray().end(); itr2++)
      {
          Gesture *dataGesture = *itr2;
          SimpleDTW<Ogre::Vector3>  dtw(inputPattern, dataGesture->getLocalAccelerationArray(), ptr_fun(ogreVectorLength));
          float result = dtw.calculateDistance();
          PRINTF("(%10s(%2d)<-->Input(%2d) : %f", 
              dataRecog->getName().c_str(), dataGesture->getLocalAccelerationArray().size(),
              inputPattern.size(), result);
          if (result < minDistance) 
          {
              minDistance = result;
              minRecog = dataRecog;
              minNumSamples = dataGesture->getLocalAccelerationArray().size();
          }
      }
  }

  //PRINTF("DECISION: %10s(%2d, %2d)(distance:%5.1f)", minRecog->getName().c_str(), minNumSamples, inputPattern.size(), minDistance);
  //printf("DECISION: %10s(%2d, %2d)(distance:%5.1f)\n", minRecog->getName().c_str(), minNumSamples, inputPattern.size(), minDistance);

  return minRecog->getName();
  //return (minDistance < 256.0f ? minRecog->getName() : noDetect);
}


std::string& DTWRecognizer::decideGesture(std::vector<Recognition *>& recognitionArray, std::vector<Ogre::Vector3>& inputPattern)
{
  float minDistance = numeric_limits<float>::max();
  Recognition *minRecog = NULL;
  int minNumSamples;
  static std::string noDetect("NO_DETECT");

  std::vector<Recognition *>::iterator itr1 = recognitionArray.begin();


  for ( ; itr1 != recognitionArray.end(); itr1++)
  {
      Recognition *dataRecog = *itr1;
      std::vector<Gesture *>::iterator itr2 = dataRecog->getGestureArray().begin();
      for ( ; itr2 != dataRecog->getGestureArray().end(); itr2++)
      {
          Gesture *dataGesture = *itr2;
          SimpleDTW<Ogre::Vector3>  dtw(inputPattern, dataGesture->getLocalAccelerationArray(), ptr_fun(ogreVectorLength));
          float result = dtw.calculateDistance();
          PRINTF("(%10s(%2d)<-->Input(%2d) : %f", 
              dataRecog->getName().c_str(), dataGesture->getLocalAccelerationArray().size(),
              inputPattern.size(), result);
          if (result < minDistance) 
          {
              minDistance = result;
              minRecog = dataRecog;
              minNumSamples = dataGesture->getLocalAccelerationArray().size();
          }
      }
  }

  PRINTF("DECISION: %10s(%2d, %2d)(distance:%5.1f)", minRecog->getName().c_str(), minNumSamples, inputPattern.size(), minDistance);

  return minRecog->getName();

  return (minDistance < 256.0f ? minRecog->getName() : noDetect);
}