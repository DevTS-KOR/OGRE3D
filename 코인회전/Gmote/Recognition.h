#if !defined(__RECOGNITION_H__)
#define __RECOGNITION_H__

#include <algorithm>

#include "Gesture.h"


class Recognition {

    std::vector<Gesture *> mGestureArray;
    std::string mName;

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
    Recognition(void)
    {
        mName = Ogre::String("Recognition_") + Ogre::String(getTimeLabel());
    }

    Recognition(std::string name)
    {
        mName = name;
    }

    std::vector<Gesture *>& getGestureArray(void) { return mGestureArray; }

	std::string& getName(void) { return mName; }

	// 
	void fillTrainingSet(std::vector<std::vector<float>> &trainingSet)
	{
		for (int i = 0; i < mGestureArray.size(); i++) {
			trainingSet.push_back(std::vector<float>());
			std::vector<int>::iterator p = mGestureArray[i]->getSymbolArray().begin();
            std::vector<int>::iterator q = mGestureArray[i]->getSymbolArray().end();
			copy(p, q, std::back_inserter(trainingSet.back()));
		}

	}

    bool fillByXml(TiXmlElement *recognitionElement)
    {
        TiXmlElement* currElement;
        TiXmlHandle hRoot(0);

        mName = std::string(recognitionElement->Attribute("Name"));

        hRoot = TiXmlHandle(recognitionElement);
        currElement = hRoot.FirstChild("Gesture").Element();
        for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
            Gesture *gesture = new Gesture();
            bool ok = gesture->fillByXml(currElement);
            if (!ok) 
                return false;
            mGestureArray.push_back(gesture);
        }

        return true;
    }

    TiXmlElement *createXmlElement(void)
    {
        // <Recognition Name="...." />
        TiXmlElement *recognitionElement = new TiXmlElement( "Recognition" );
        recognitionElement->SetAttribute("Name", mName.c_str());

        std::vector<Gesture *>::iterator itr;
        
        for (itr = mGestureArray.begin(); itr < mGestureArray.end(); itr++) {
            TiXmlElement *gestureElement = (*itr)->createXmlElement();
            recognitionElement->LinkEndChild(gestureElement);
        }

        return recognitionElement;
    }

};







#endif // __RECOGNITION_H__