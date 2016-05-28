/**
* @file    DTWRecognizer.h
* @brief   Gesture Recognition Algorithm using DTW(Dynamic Time Warping) Algorithm
* @author  Dae-Hyun Lee
*/

#if !defined(__DTW_RECOGNIZER_H__)
#define __DTW_RECOGNIZER_H__

#include <vector>

#include "Singleton.h"

#include "Recognition.h"

#define dtwRecognizer (DTWRecognizer::instance())

class DTWRecognizer : public SisaSingleton<DTWRecognizer>
{
public:

    /**
     * @brief       인식하고자 하는 recognition 을 등록함. decideGesture를 이용하여 제스쳐를 인식하기 전에, 미리 인식하고자 하는 제스쳐 집합(패턴 DB)을 등록해야 함.
     * @param[in]   Recognition 포인터
     * @see         decideGesture, deleteRecognition
     */
    void registerRecognition(Recognition *recog);


    /**
     * @brief       현재 등록되어 있는 recognition(제스쳐 집합)들 중에서, 지정된 이름의 recognition을 제거함. 
     * @param[in]   제거하고자 하는 recognition 이름
     * @see         decideGesture, registerRecognition
     */
    void deleteRecognition(std::string& recogName);


    /**
     * @brief       DTW 알고리즘을 이용하여 제스쳐를 인식함. 
     * @param[in]   컨트롤러를 통해서 입력받은 제스쳐 벡터
     * @return      인식된 제스쳐 이름
     * @see         decideGesture, registerRecognition
     */
    std::string& DTWRecognizer::decideGesture(std::vector<Ogre::Vector3>& inputPattern);

    /**
     * @brief       DTW 알고리즘을 이용하여 제스쳐를 인식함. 패턴 DB를 외부에서 제공함.
     * @param[in]   외부에서 제공하는 패턴 DB
     * @param[in]   컨트롤러를 통해서 입력받은 제스쳐 벡터
     */
    std::string& DTWRecognizer::decideGesture(std::vector<Recognition *>& recognitionArray, std::vector<Ogre::Vector3>& inputPattern);
private:
    std::vector<Recognition *> mRecognitionArray;
};




#endif // __DTW_RECOGNIZER_H__