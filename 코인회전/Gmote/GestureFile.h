#if !defined(__GESTUREFILE_H__)
#define __GESTUREFILE_H__

#include <vector>
#include <list>
#include <map>
#include <sstream>


#include "Singleton.h"


#include "Gesture.h"
#include "Preprocessor.h"
#include "Recognition.h"


#define gestureFile (GestureFile::instance())

class GestureFile : public SisaSingleton<GestureFile>
{
    std::vector<Recognition *> mRecognitionArray;
    std::map<std::string, Recognition*> mRecognitionMap;
    std::string mName;
    std::string mFileName;

public:

    GestureFile() { mFileName = ""; }

    ~GestureFile() {}

    std::vector<Recognition *>& getRecognitionArray(void) { return mRecognitionArray; }

    Recognition* getRecognition(const std::string& name)
    {
        if (mRecognitionMap.find(name) == mRecognitionMap.end())
            return NULL;
        return mRecognitionMap[name];
    }

    void addRecognition(Recognition *recog)
    {
        mRecognitionArray.push_back(recog);
        mRecognitionMap[recog->getName()] = recog;
    }

    std::string generateFileName(void)
    {
        time_t timer;
        struct tm *t;

        timer = time(NULL);
        t = localtime(&timer);

        std::ostringstream nameStream;
        nameStream << "GestureRecognizer_" 
            << t->tm_year + 1900 << "_"
            << t->tm_mon + 1 << "_"
            << t->tm_mday << "_"
            << t->tm_hour << "_"
            << t->tm_min << "_"
            << t->tm_sec
            << ".xml";

        return nameStream.str();
    }


    void clear(void)
    {
        std::vector<Recognition *>::iterator itr;
        for (itr = mRecognitionArray.begin(); itr != mRecognitionArray.end(); itr++)
            delete (*itr);

        mRecognitionArray.clear();
        mRecognitionMap.clear();
    }


    bool readXmlFile(const char *fileName)
    {
        mFileName = fileName;
        TiXmlDocument doc(fileName);
        if (!doc.LoadFile()) {
            cout << "Error Loading " << fileName << endl;
            return false;
        }

        TiXmlHandle hDoc(&doc);
        TiXmlElement* currElement;
        TiXmlHandle hRoot(0);

        currElement = hDoc.FirstChildElement().Element();
        // should always have a valid root but handle gracefully if it does
        if (!currElement) {
            cout << "Error Parsing" << endl;
            return false;
        }


        // check <GestureRecognizer   > token
        if (std::string("GestureRecognizer") != currElement->Value()) {
            cout << "Error finding <" << "GestureRecognizer" << ">" << endl;
            return false;
        }

        // get <GestureRecognizer Name="?">
        mName = currElement->Attribute("Name");


        hRoot = TiXmlHandle(currElement);

#if defined(QUANTIZATION_NEEDED)
        currElement = hRoot.FirstChild("Quantizer").Element();
        if (currElement) {
            bool ok = getQuantizer().fillByXml(currElement);
            if (!ok)
                return false;
        }
#endif

        currElement = hRoot.FirstChild("Recognition").Element();
        for (currElement; currElement; currElement = currElement->NextSiblingElement()) {
            Recognition *recognition = new Recognition();
            bool ok = recognition->fillByXml(currElement);
            if (!ok) 
                return false;
            mRecognitionArray.push_back(recognition);
            mRecognitionMap[recognition->getName()] = recognition;
        }

        return true;
    }


    void saveXmlFile(void) 
    {
        if (mFileName != "")
            saveXmlFile(mFileName);
        else
            saveXmlFile(generateFileName()); 
    }

    void saveXmlFile(std::string fileName)
    {
        TiXmlDocument doc;
        TiXmlDeclaration *decl = new TiXmlDeclaration( "1.0", "", "" );
        doc.LinkEndChild(decl);

        TiXmlElement *topElement = new TiXmlElement( "GestureRecognizer" );
        topElement->SetAttribute("Name", "AutomaticallyGenerated");

        std::vector<Recognition *>::iterator itr;




        for (itr = mRecognitionArray.begin(); itr < mRecognitionArray.end(); itr++) {
            TiXmlElement *recognitionElement = (*itr)->createXmlElement();
            topElement->LinkEndChild(recognitionElement);
        }

        doc.LinkEndChild(topElement);

        doc.SaveFile(fileName);
    }


    void train(void)
    {
    }

};


#endif