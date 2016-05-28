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
     * @brief       �ν��ϰ��� �ϴ� recognition �� �����. decideGesture�� �̿��Ͽ� �����ĸ� �ν��ϱ� ����, �̸� �ν��ϰ��� �ϴ� ������ ����(���� DB)�� ����ؾ� ��.
     * @param[in]   Recognition ������
     * @see         decideGesture, deleteRecognition
     */
    void registerRecognition(Recognition *recog);


    /**
     * @brief       ���� ��ϵǾ� �ִ� recognition(������ ����)�� �߿���, ������ �̸��� recognition�� ������. 
     * @param[in]   �����ϰ��� �ϴ� recognition �̸�
     * @see         decideGesture, registerRecognition
     */
    void deleteRecognition(std::string& recogName);


    /**
     * @brief       DTW �˰����� �̿��Ͽ� �����ĸ� �ν���. 
     * @param[in]   ��Ʈ�ѷ��� ���ؼ� �Է¹��� ������ ����
     * @return      �νĵ� ������ �̸�
     * @see         decideGesture, registerRecognition
     */
    std::string& DTWRecognizer::decideGesture(std::vector<Ogre::Vector3>& inputPattern);

    /**
     * @brief       DTW �˰����� �̿��Ͽ� �����ĸ� �ν���. ���� DB�� �ܺο��� ������.
     * @param[in]   �ܺο��� �����ϴ� ���� DB
     * @param[in]   ��Ʈ�ѷ��� ���ؼ� �Է¹��� ������ ����
     */
    std::string& DTWRecognizer::decideGesture(std::vector<Recognition *>& recognitionArray, std::vector<Ogre::Vector3>& inputPattern);
private:
    std::vector<Recognition *> mRecognitionArray;
};




#endif // __DTW_RECOGNIZER_H__