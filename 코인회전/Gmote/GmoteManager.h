#if !defined(_GMOTE_MANAGER_H__)
#define _GMOTE_MANAGER_H__

#include <string>
#include "Singleton.h"
#include "recognition.h"



enum ButtonID
{
  //B_EXTRA, B_UP, B_LEFT, B_SELECT, B_RIGHT, B_DOWN, B_END
  B_UP, B_DOWN , B_LEFT, B_RIGHT, B_SELECT, B_EXTRA, B_END
};


#define gmoteManager (GmoteManager::instance())
class GmoteListener
{
public:
  virtual ~GmoteListener() {}

  virtual bool disconnected(int id) = 0;
  virtual bool connected(int id) = 0;

  virtual bool accelerationCaptured(int id, float x, float y, float z) = 0;
  virtual bool buttonPressed(int id, int button, int buttons) = 0;
  virtual bool buttonReleased(int id, int button, int buttons) = 0;

  virtual bool rightFlagCaptured(int id, char rightFlag) = 0;

  virtual bool forceDetected(int id, float x, float y, float z) = 0;

  virtual bool peakForceDetected(int id, float x, float y, float z, unsigned long period) = 0;

  virtual bool gestureDectected(int id, std::string& gestureName) = 0;

  virtual bool batteryChanged(int id, float battery) = 0;

};


class GmoteManager : public SisaSingleton<GmoteManager>
{
public:

  bool init(void);

  void setEventListener(GmoteListener *listener);

  void readGestureFile(const char *fileName);

  void registerGestureSet(int id, ...);
  void registerGestureSetAll(int id);

  void deleteGestureSet(int id, ...);
  void deleteGestureSetAll(int id);

  // 감지하고자 하는 힘의 크기를 등록. force vector크기의 제곱임.
  void registerForce(int id, float force);

  // 감지하고자 하는 peak force의 최소값을 등록.
  void registerPeakForce(int id, float force);

  void enableButtonInterface(int id);
  void disableButtonInterface(int id);

  void enableForceInterface(int id);
  void disableForceInterface(int id);

  void enablePeakForceInterface(int id);
  void disablePeakForceInterface(int id);

  void enableGestureInterface(int id);
  void disableGestureInterface(int id);

  // 매 프레임마다 주기적으로 반드시 호출해주어야 함.
  void capture(void);

  void quit(void);

private:
  void _registerRecognition(int id, Recognition *recog);
  void _deleteRecognition(int id, std::string& recogName);
  void _error(const std::string& loc, const std::string& msg = "");
};


#endif // _GMOTE_MANAGER_H__