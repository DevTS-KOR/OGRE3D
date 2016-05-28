#if !defined(__GESTURE_H__)
#define __GESTURE_H__

#include "tinyxml.h"
#include <vector>
#include "Ogre.h"
#include "OgreVector3.h"

//#include "Preprocessor.h"

using namespace std;




class Gesture {
  std::string mName;
  float mSamplingFrequency;
  std::vector<Ogre::Vector3> mLocalAccelerationArray;
  std::vector<Ogre::Vector3> mWorldAccelerationArray;
  std::vector<Ogre::Vector3> mVelocityArray;
  std::vector<Ogre::Vector3> mPositionArray;


  std::vector<int> mSymbolArray;

  //Quantizer* mQuantizer;

  char *getTimeLabel(void)
  {
    time_t timer;
    struct tm *t;

    timer = time(NULL);
    t = localtime(&timer);

    static char s[255];

    sprintf(s, "%04d_%02d_%02d_%02d_%02d_%02d",
      t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
      t->tm_hour, t->tm_min, t->tm_sec);

    return s;
  }


public:
  Gesture(void)
  {
    mName = Ogre::String("Gesture_") + Ogre::String(getTimeLabel());
    mSamplingFrequency = 30.0f;
  }

  Gesture(std::string name)
  {
    mName = name;
    mSamplingFrequency = 30.0f;
  }

  //~Gesture(void);


  std::vector<Ogre::Vector3>& getLocalAccelerationArray(void) { return mLocalAccelerationArray; }
  std::vector<int>& getSymbolArray(void) { return mSymbolArray; }
  void setSymbolArray(std::vector<int>& newSymbolArray) { mSymbolArray = newSymbolArray; }
  void setLocalAccelerationArray(std::vector<Ogre::Vector3>& newSymbolArray) { mLocalAccelerationArray = newSymbolArray; }

  float getMaxAcceleration(void);
  float getMinAcceleration(void);




  void addLocalAcceleration(Ogre::Vector3 &localAcceleration)
  {
    mLocalAccelerationArray.push_back(localAcceleration);
  }

  bool fillByXml(TiXmlElement *gestureElement)
  {
    mName = std::string(gestureElement->Attribute("Name"));
    TiXmlHandle hRoot = TiXmlHandle(gestureElement);

    {
      // parse <LocalAccelerationArray>
      TiXmlElement *currElement = hRoot.FirstChild("LocalAccelerationArray").FirstChild("LocalAcceleration").Element();
      // TODO: need to check empty <LocalAcceleration>
      for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
        // TODO: need to check this element is <LocalAcceleration>
        int index;
        double x, y, z;
        if (TIXML_SUCCESS == currElement->QueryIntAttribute("No", &index)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("x", &x)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("y", &y)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("z", &z)) {
            mLocalAccelerationArray.push_back(Ogre::Vector3(x, y, z));
        } else {
          cout << "Parsing Error " << endl;
          return false;
        }
      }
    }

    {
      // parse <WorldAccelerationArray>
      TiXmlElement *currElement = hRoot.FirstChild("WorldAccelerationArray").FirstChild("WorldAcceleration").Element();
      // TODO: need to check empty <WorldAcceleration>
      for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
        // TODO: need to check this element is <WorldAcceleration>
        int index;
        double x, y, z;
        if (TIXML_SUCCESS == currElement->QueryIntAttribute("No", &index)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("x", &x)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("y", &y)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("z", &z)) {
            mWorldAccelerationArray.push_back(Ogre::Vector3(x, y, z));
        } else {
          cout << "Parsing Error " << endl;
          return false;
        }
      }
    }

    {
      // parse <VelocityArray>
      TiXmlElement *currElement = hRoot.FirstChild("VelocityArray").FirstChild("Velocity").Element();
      // TODO: need to check empty <Velocity>
      for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
        // TODO: need to check this element is <Velocity>
        int index;
        double x, y, z;
        if (TIXML_SUCCESS == currElement->QueryIntAttribute("No", &index)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("x", &x)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("y", &y)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("z", &z)) {
            mVelocityArray.push_back(Ogre::Vector3(x, y, z));
        } else {
          cout << "Parsing Error " << endl;
          return false;
        }
      }
    }


    {
      // parse <PositionArray>
      TiXmlElement *currElement = hRoot.FirstChild("PositionArray").FirstChild("Position").Element();
      // TODO: need to check empty <Position>
      for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
        // TODO: need to check this element is <Position>
        int index;
        double x, y, z;
        if (TIXML_SUCCESS == currElement->QueryIntAttribute("No", &index)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("x", &x)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("y", &y)
          && TIXML_SUCCESS == currElement->QueryDoubleAttribute("z", &z)) {
            mPositionArray.push_back(Ogre::Vector3(x, y, z));
        } else {
          cout << "Parsing Error " << endl;
          return false;
        }
      }
    }

    {
      // parse <SymbolArray>
      TiXmlElement *currElement = hRoot.FirstChild("SymbolArray").FirstChild("Symbol").Element();
      // TODO: need to check empty <Symbol>
      for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
        // TODO: need to check this element is <Symbol>
        int index, value;
        if (TIXML_SUCCESS == currElement->QueryIntAttribute("No", &index)
          && TIXML_SUCCESS == currElement->QueryIntAttribute("Value", &value)) {
            mSymbolArray.push_back(value);
        } else {
          cout << "Parsing Error " << endl;
          return false;
        }
      }
    }


    return true;
  }


  TiXmlElement *createXmlElement(void)
  {
    // <Gesture Name="...." />
    TiXmlElement *gestureElement = new TiXmlElement( "Gesture" );
    gestureElement->SetAttribute("Name", mName.c_str());

    // <Property Name="SamplingFrequency" Value="30" />
    TiXmlElement *propertyElement = new TiXmlElement( "Property");
    propertyElement->SetAttribute("Name", "SamplingFrequency");
    propertyElement->SetDoubleAttribute("Value", mSamplingFrequency);
    gestureElement->LinkEndChild(propertyElement);

    // <LocalAccelerationArray> </LocalAccelerationArray>
    TiXmlElement *localAccelerationArrayElement = new TiXmlElement( "LocalAccelerationArray");
    for (int i = 0; i < mLocalAccelerationArray.size(); i++) {
      TiXmlElement *localAccelerationElement = new TiXmlElement( "LocalAcceleration");
      localAccelerationElement->SetAttribute("No", i);
      localAccelerationElement->SetDoubleAttribute("x", mLocalAccelerationArray[i].x);
      localAccelerationElement->SetDoubleAttribute("y", mLocalAccelerationArray[i].y);
      localAccelerationElement->SetDoubleAttribute("z", mLocalAccelerationArray[i].z);
      localAccelerationArrayElement->LinkEndChild(localAccelerationElement);
    }
    gestureElement->LinkEndChild(localAccelerationArrayElement);


#if defined(QUANTIZATION_NEEDED)
    TiXmlElement *symbolArrayElement = new TiXmlElement( "SymbolArray");
    for (int i = 0; i < mSymbolArray.size(); i++) {
      TiXmlElement *symbolElement = new TiXmlElement( "Symbol");
      symbolElement->SetAttribute("No", i);
      symbolElement->SetAttribute("Value", mSymbolArray[i]);
      Ogre::Vector3 symbolAcceleration = Quantizer::Instance().getSymbolAcceleration(mSymbolArray[i]);
      symbolElement->SetDoubleAttribute("x", symbolAcceleration.x);
      symbolElement->SetDoubleAttribute("y", symbolAcceleration.y);
      symbolElement->SetDoubleAttribute("z", symbolAcceleration.z);

      symbolArrayElement->LinkEndChild(symbolElement);
    }
    gestureElement->LinkEndChild(symbolArrayElement);
#endif

    return gestureElement;
  }


};

#endif // __GESTURE_H__