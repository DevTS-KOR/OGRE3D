#include "debug.h"

#include <windows.h>


#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include "Gmote.h"

#include <time.h>

#include <process.h>	// for _beginthreadex()
#include <math.h>		// for orientation
#include <mmsystem.h>	// for timeGetTime()



#pragma comment(lib, "winmm.lib")	// timeGetTime()


const char SLOTMASK[] = { 0x01, 0x02, 0x04, 0x08, 0x10 };		// 2009.5.1
#define SID_INVALID		0xFFFFFFFF								// 2009.5.1
// ------------------------------------------------------------------------------------
// Revision history
// 2008.2.2 mStatus[i], mNumSensors update를 sample에서 data 수신으로 변경
//						Dongle disconnect counter 비교 횟수를 2로 최종 변경(zmoteTick)
// 2009.5.1 mSlotAvail 값 갱신을 sensor_id 갱신과 동기시켜줌.
//          mSensorID 값이 0xFFFFFFFF이면 invalid 한 값으로 인식하도록 변경
// -----------------------------------------------------------------------------------


// #define USE_GYRO_LOG

#ifdef USE_GYRO_LOG
FILE *fpGyro[5] = {NULL, };
unsigned char logcnt = 0;
#endif

#define		N_GYRO_STEADY	20
#define		N_GYRO_MARGIN	7

unsigned char initGyro[5] = {0, };
int GyroAvgCnt[5], GyroMin[5], GyroMax[5], GyroSum[5];
float GyroOffset[5], GyroAngle[5], gyrofiltered[5]; // , GyroIncrement[5];
int MissCnt;

// helpers
// ------------------------------------------------------------------------------------
template<class T> __forceinline int sign  (const T& val)  { return (val<0)? -1 : 1; }
template<class T> __forceinline T   square(const T& val)  { return val*val; }
#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

// ------------------------------------------------------------------------------------
CCommThread ComuPort;
void CALLBACK zmoteTick(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
#define UPLOAD_PERIOD 30

#define D_GAP 25		// 2008.12.3   23

char	t_index = 0;
UINT	timerID = 0;

clock_t oldclock, newclock;		// 2008.12.10 Doohee
FILE *fptime = NULL;			// 2008.12.10 Doohee

bool use_time_log = false;
// #define USE_FPLOG

#ifdef USE_FPLOG
FILE *fpLog = NULL;
#endif
unsigned char pBuff[1024];

// clock_t start = 0, finish = 0;
// double duration= 0.0;
#define PACK_LEN  16

enum upload_stat
{
    INIT,
    X_DATA,
    Y_DATA,
    Z_DATA,
    S_DATA,
    T_DATA,
    B_DATA,
    A_DATA,
    K_DATA,
    Q_DATA,
	D_DATA
} ;

enum upload_stat status = INIT;
unsigned int x_data[5], y_data[5], z_data[5], batt_vtg[5];
unsigned int t_data[5], b_data[5], tt_data[5];
unsigned char psnd[5], sample_num[5];

#define HASH_RES	0x800
#define HASH_MAX	32
unsigned char a_len, a_len0;
unsigned char wbuf[256] = {3, };

// bool calib_flag = TRUE;
unsigned char cmd_buffer[10];

#ifdef DATA_BIG_ENDIAN
// #define byte2short(s, pb, i)	s = pb[i], s = (s<<8)+pb[i+1], i+=2
#else
#define byte2short(s, pb, i)	s = pb[i+1], s = (s<<8)+pb[i], i+=2
#define byte2long(s, pb, i)	    s = pb[i+3], s = (s<<8)+pb[i+2], s = (s<<8)+pb[i+1], s = (s<<8)+pb[i], i+=4
#endif

void CALLBACK zmoteTick(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    int i, len;
    ZMotioner* zm;
    zm = (ZMotioner*)dwUser;


    // 항상, USB dongle을 상태가 정상인지 확인해야 함.
    if (!ComuPort.m_bConnected) {
        // USB dongle이 연결되어 있지 않은 상태이면, 모든 상태를 초기화해주어야 함. 
        // zm->disconnectDongle();
        return;
    }

    while ((len = ComuPort.m_QueueRead.GetSize()) > 0) {
        zm->sendCnt = 0;
        len = min(len, 1024);
        for (i = 0; i < len ; i++) 
            ComuPort.m_QueueRead.GetByte(&pBuff[i]);
        zm->ParseInput(pBuff, len);
        zm->mRcvdCnt = (zm->mRcvdCnt + len) & 0x7FFF;
    } 

    if (zm->mStartFlag == false) { 
        cmd_buffer[0] = 'S';
        cmd_buffer[1] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 1);
        zm->mStartFlag = true;
    } else if (zm->mRebootDongle) { // (zm->mPrevNumSensors == -1) {
        cmd_buffer[0] = 'Z';
        cmd_buffer[1] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 1);
    } else if ((zm->mPrevSlotAvail != zm->mSlotAvail) && (zm->mPrevSlotAvail != 0xFF || zm->mSlotAvail != 0x0F)) { //	else if(zm->mPrevSlotAvail != zm->mSlotAvail) 
		// Gyro 초기화 관련 코드를 추가
		char index=0, newslot = zm->mPrevSlotAvail & ~(zm->mSlotAvail), setflag = 0;
		// newslot : 새로 접속한 센서(노드)를 의미
		// setflag : 이번에 gyro를 초기화시킬 slot 번호 전체
		while(newslot != 0 && index < 4) {	// 새로운 node가 할당되거나 MAX_NODE 이내인 경우 진행
			if(newslot & 1) { initGyro[index] = 0, setflag |= (1<<index); }
			newslot >>= 1;
			index++;
		}
		    
		time_t cur = time(NULL);
        if(zm->errLog == NULL)
            zm->errLog = fopen("errLog.txt", "at");
        if(zm->errLog) {
            fprintf(zm->errLog, "SLOT_CHANGE(%02x) %02x %ld %s ", zm->mSlotAvail, setflag, timeGetTime(), ctime(&cur));
            fclose(zm->errLog);
            zm->errLog = NULL;
        } else {
            zm->errLog = fopen("errLog.txt", "at");
        }
		// slot 상태가 바뀌면 접속 dongle에 대한 정보를 얻기 위해 'K' 명령을 전송
        zm->mSlotChange = true;
        cmd_buffer[0] = 'K';
        zm->mPrevSlotAvail = zm->mSlotAvail;	
        cmd_buffer[1] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 1);
    } else if (zm->mCalibFlag) {	
        cmd_buffer[0] = 'q';
        zm->mCalibFlag = false;
        cmd_buffer[1] = 12;
        for (i=0; i < 12; i++) 
            cmd_buffer[i+2] = zm->mCalibData[i];
        cmd_buffer[14] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 14);
	} else if (zm->mAudioFlag) {	// 2009.12.11
		cmd_buffer[0] = 's';
		zm->mAudioFlag = false;
		cmd_buffer[1] = (zm->mAudioId>>24)&0xFF;
		cmd_buffer[2] = (zm->mAudioId>>16)&0xFF;
		cmd_buffer[3] = (zm->mAudioId>>8)&0xFF;
		cmd_buffer[4] = (zm->mAudioId)&0xFF;
		cmd_buffer[5] = zm->mAudioData;			// soundIndex
		{
			char checksum = 0x55;
			for(i=1; i<6;i++)
				checksum += cmd_buffer[i];
		    cmd_buffer[6] = checksum;
		}
		ComuPort.WriteComm(cmd_buffer, 7);
    } else if (zm->mPairingFlag) {	// 2010.3.27
		cmd_buffer[0] = 'p';
		zm->mPairingFlag = false;
		cmd_buffer[1] = ~cmd_buffer[0];
		cmd_buffer[2] = zm->mPairingData;
		cmd_buffer[3] = ~cmd_buffer[2];
		ComuPort.WriteComm(cmd_buffer, 4);
	} else if (zm->mPairingOpFlag) {	// 2010.4.18
		cmd_buffer[0] = 'd';
        cmd_buffer[1] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 1);
	} else {
        cmd_buffer[0] = 'h';
        cmd_buffer[1] = 0x00;
        ComuPort.WriteComm(cmd_buffer, 1);
    }

    zm->execCnt++;
    zm->sendCnt++;

    if (zm->sendCnt > (UPLOAD_PERIOD/2)) {
		// 연결을 끊은 이후 다시 연결하는 코드가 없음
		// 어플에서 호출해주어야 함.
		// 일단 아래 코드를 임시로 막아놓았음 2009.12.14
        // zm->disconnectDongle();
        zm->sendCnt = 0;
    }
}



// ------------------------------------------------------------------------------------
ZMotioner::ZMotioner ()
{
    // Initialize the critical section one time only.
    InitializeCriticalSection(&cs);

    // Reference : http://msdn.microsoft.com/en-us/library/ms712753(VS.85).aspx
#define TARGET_RESOLUTION 1         // 1-millisecond target resolution

    TIMECAPS tc;
    UINT     wTimerRes;

    if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
    {
        // Error; application can't continue.
    }

    wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
    timeBeginPeriod(wTimerRes);




}
// ------------------------------------------------------------------------------------
ZMotioner::~ZMotioner ()
{
    // tidy up timer accuracy request
    timeEndPeriod(1);		

    DeleteCriticalSection(&cs);


    //$$	fclose(errLog);
    //$$	errLog = NULL;
}

// ------------------------------------------------------------------------------------
char* ZMotioner::getName(void)
{
    return "Zigbee Motioner";
}

// ------------------------------------------------------------------------------------
bool ZMotioner::init(void)
{

    //$$	mNumSensors = 0;
    mDongleConnected = false;

    mPrevSlotAvail = 0xFF;	
    for(int i=0; i<4; i++)
        mSensorID[i] = SID_INVALID;	// 2009.5.1
    //$$	mConnected = false;
    mSlotAvail = 0x0f;			// 4 sensor-nodes are supported presently.
    mSlotChange = false;
    mCalibFlag = false;
    mRebootDongle = false;
    mStartFlag = true;		//$$ false;
    mRcvdCnt = 0;
	mAudioFlag = false;
    mPairingFlag = false;	 		// 2010.3.27
	mPairingOpFlag = false;			// 2010.4.18

	MissCnt = 0;
    sendCnt = 0;
    errLog = fopen("errlog.txt", "at");

    mErrCnt1 = mOKCnt1 = 0;
    mErrCnt2 = mOKCnt2 = 0;

    mAcqCnt = mOldAcqCnt = mDiffAcqCnt = 0;
    old_t = timeGetTime();

    return true;
}

bool ZMotioner::connectDongle(void)
{
    if (!mDongleConnected) {
		char ComStr[12] = "COM1";	// unicode 및 COM10 등에 대응 크기
		FILE *fp = fopen("dongle.config","rt");
		if(fp != NULL) {
			fscanf(fp,"%s",ComStr);	// ComStr 값의 유효성 체크는 생략
			fclose(fp);
		}        
        // USB 동글의 연결
        mDongleConnected = (bool)ComuPort.OpenPort(ComStr, 250000, 3);
        if (mDongleConnected) {
            // 타이머 함수 설정
            mTimerID = timeSetEvent(UPLOAD_PERIOD, 1, (LPTIMECALLBACK)zmoteTick, (DWORD_PTR)this, TIME_KILL_SYNCHRONOUS | TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
        }
    }

    return mDongleConnected;
}

bool ZMotioner::findDonglePort(void)
{
#define MAX_PORT_NO	10

	int old_offset = -1, offset = 0, i;
	char ComStr[12] = "COM1", ch;	// unicode 및 COM10 등에 대응 크기
	FILE *fp;
	fp = fopen("dongle.config","rt");
	if(fp != NULL) {
		fscanf(fp,"%s",ComStr);	// ComStr 값의 유효성 체크는 생략
		fclose(fp);
		sscanf(ComStr, "%c%c%c%d", &ch,&ch, &ch, &offset);
		offset -= 1;
		old_offset = offset;
	}        

	for(i=0; i<10; i++) {
		// USB 동글의 연결
		sprintf(ComStr,"COM%d", ((i+offset)%10)+1);
		mDongleConnected = (bool)ComuPort.OpenPort(ComStr, 250000, 3);
	    old_t = timeGetTime();
		if (mDongleConnected) {
			unsigned char cmd[2] = "I";
			char step = 0, len, ii, INFO[100];
			ComuPort.WriteComm(cmd, 1);
			while(1){
				if(ComuPort.m_QueueRead.GetSize() > 0) {
					ComuPort.m_QueueRead.GetByte(cmd);
					if(step == 0 && cmd[0] == 'I')
						step = 1;
					else if(step == 1) 
						len = cmd[0]-1, step = 2, ii = 0;
					else if(step == 2) {
						INFO[ii++] = cmd[0];
						len--;
						if(len==0) {
							ComuPort.ClosePort();
					        mDongleConnected = false;
							goto write_result;
						}
					}
				}
			    new_t = timeGetTime();
				// check time-out condition
				if(new_t-old_t > 2000) {
				  ComuPort.ClosePort();
		          mDongleConnected = false;
				  break;
				}
			}	// while(1)
		} // if(mDongleConnected)
	} // for(i=0; i<10; i++)
	return false;

write_result:
	if( (offset+i) != old_offset) {
		fp = fopen("dongle.config","wt");
		if(fp != NULL) {
			fprintf(fp,"COM%d",((i+offset)%10)+1);	
			fclose(fp);
		}
	}        
	return true;

}

std::set<int> ZMotioner::getConnectedSensorIDSet(void)
{
    std::set<int> idSet;
    int i, bitcheck = 1;

    // 동글이 연결이 안되면, 그냥 리턴
    if (!mDongleConnected)
        return idSet;

    // 슬롯을 확인해나가면서, 연결된 슬롯에 해당된 ID를 얻어서 idSet에 저장.
    for(i=0; i<4; i++) {
        if( (mSlotAvail & bitcheck) == 0) {
            //PRINTF("SLOT %d", i);		
            if(mSensorID[i] != SID_INVALID)		// 2009.5.1
                idSet.insert(mSensorID[i]);
        }
        bitcheck <<= 1;
    }

    return idSet;
}

bool ZMotioner::dongleConnected(void)
{
    return mDongleConnected;
}

bool ZMotioner::sensorConnected(int id)
{
    int i, bitcheck = 1;

    // 동글이 연결이 안되면, flase 반환
    if (!mDongleConnected)
        return false;

    // 슬롯을 확인해나가면서, 연결된 슬롯에 해당 ID가 있으면 true 반환.
    for(i=0; i<4; i++) {
        if( (mSlotAvail & bitcheck) == 0) {
            if(mSensorID[i] == (unsigned long)id) 
                return true;
        }
        bitcheck <<= 1;
    }
    // 없으면 false 반환
    return false;

}

bool ZMotioner::disconnectDongle(void)
{
    if (mDongleConnected) {
        timeKillEvent(mTimerID);
        ComuPort.ClosePort();
        mDongleConnected = false;
    }

    return true;
}


void ZMotioner::sample(void)
{

}

#define CAL_LEN	63		// SW_VERSION이 00 0a 이상인 경우, 2008.10.7
// #define CAL_LEN	62		// SW_VERSION이 00 09 인 경우, 2008.10.7
// #define CAL_LEN	60
int ok_cnt[5]={0,}, oldPilot[5]={0,}, oldAcq[5]={0,}, oldSend[5]={0,}, newSend[5];
int miss_cnt = 0;

void ZMotioner::ParseInput(unsigned char *pBuff, int len)
{
    int i = 0, j;
    //$$	int mask;
    int numSensors;

    while(i < len) {
        switch(status)
        {
        case INIT:
            if(pBuff[i] == 'x')		  status = X_DATA;
            else if(pBuff[i] == 'y')  status = Y_DATA;
            else if(pBuff[i] == 'z')  status = Z_DATA;
            else if(pBuff[i] == 's')  status = S_DATA;
            else if(pBuff[i] == 't')  status = T_DATA;
            else if(pBuff[i] == 'b')  status = B_DATA;
            else if(pBuff[i] == 'h')  { status = A_DATA, a_len = 255; }
            else if(pBuff[i] == 'K')  { status = K_DATA, a_len = 255; }
            else if(pBuff[i] == 'Q')  { status = Q_DATA, a_len = 255; }
            else if(pBuff[i] == 'd')  { status = D_DATA, a_len = 255; }
            else					  status = INIT;
            i++;
            break;

        case K_DATA:
            if(a_len == 255) {
                a_len = pBuff[i++];
                a_len0 = 0;
            }
            if(a_len != 255) {
                if(a_len-a_len0 <= len-i) {		// 처리할 만큼 다 올라온 경우
                    int k;
                    memcpy(&wbuf[a_len0], &pBuff[i], a_len-a_len0);
                    i += (a_len-a_len0);
                    status = INIT;

                    for(j=0x55, k=0; k<CAL_LEN; k++) {		
                        j += wbuf[k];
                    }
                    if(wbuf[CAL_LEN] == (unsigned char)(j&0xff)) {
                        for(j=0, k=0; k<5; k++) {	// Note : Big Endian !!
                            struct calibration_info *cal = &CalibrationInfo[k];
                            cal->X0 = (signed)wbuf[j];
                            cal->Y0 = (signed)wbuf[j+1];
                            cal->Z0 = (signed)wbuf[j+2];
                            cal->XG = (signed)wbuf[j+3];
                            cal->YG = (signed)wbuf[j+4];
                            cal->ZG = (signed)wbuf[j+5];
                            BYTE2LONG(mSensorID[k], &wbuf[j+6]);
                            if(SLOTMASK[k] & mSlotAvail)		// 2009.5.1
                                mSensorID[k] = SID_INVALID;
                            BYTE2WORD(mSensorVer[k], &wbuf[j+10]);

                            if((mSensorVer[k]&0xff) == 0x09) {
                                cal->X0<<=2, cal->Y0<<=2, cal->Z0<<=2;
                                cal->XG<<=2, cal->YG<<=2, cal->ZG<<=2;
                            } else if((mSensorVer[k]&0xff) >= 0x0a) {
                                cal->X0+=0x200, cal->Y0+=0x200, cal->Z0+=0x200;
                                cal->XG+=0x200, cal->YG+=0x200, cal->ZG+=0x200;
                            }

                            j+=12;
                            if((cal->X0!=cal->XG)&&(cal->Y0!=cal->YG)&&(cal->Z0!=cal->ZG))
                            {
                                printf("\n");
                                //							calib_flag = FALSE;
                            }
                        }
                        BYTE2WORD(DONGLE_VER, &wbuf[60]);
                        DONGLE_ID = wbuf[62];
                        mOKCnt2++;

                        if(mSlotChange == true){
                            time_t cur = time(NULL);
                            mSlotChange = false;

                            if(errLog == NULL) 			errLog = fopen("errLog.txt", "at");
                            if(errLog) {
                                int j=1;
								fprintf(errLog, "SLOT(%02x:ch=%02d)%02d",mSlotAvail, mChan, mOKCnt2);
                                for(i=0; i<4; i++) {
                                    if( (mSlotAvail & j)==0 ) {
                                        fprintf(errLog, ":%08x",  mSensorID[i]);
                                    } else {
                                        fprintf(errLog, ":________");
                                    }

                                    j <<= 1;
                                }
                                fprintf(errLog, "-%ld-%s", timeGetTime(), ctime(&cur));
                                fclose(errLog);
                                errLog = NULL;
                            } else {
                                errLog = fopen("errLog.txt", "at");
                            }
                        }

                    } else {
                        mErrCnt2++;
                    }

                } else if(len-i > 0) {
                    memcpy(&wbuf[a_len0], &pBuff[i], len-i);
                    a_len0 += (len-i);
                    i += len;
                }
            }
            break; // K_DATA

        case Q_DATA:
            if(a_len == 255) {		// 처음에 길이 정보를 처리
                a_len = pBuff[i++];
                a_len0 = 0;
            }
            if(a_len != 255) {		// 이제 보내준 cal_data를 전부 받는다.
                if(a_len-a_len0 <= len-i) {
                    int k;
                    memcpy(&wbuf[a_len0], &pBuff[i], a_len-a_len0);
                    i += (a_len-a_len0);
                    status = INIT;

                    for(k=0; k<12; k++) {			// Check data validity !!
                        if(this->mCalibData[k] != wbuf[k]) k=20;
                    }
                    if(k < 20)						
                        mPrevSlotAvail = 5 ;		// re-read cal data later !!

                } else if(len-i > 0) {
                    memcpy(&wbuf[a_len0], &pBuff[i], len-i);
                    a_len0 += (len-i);
                    i += len;
                }
            }
            break; // Q_DATA

        case D_DATA:
            if(a_len == 255) {		// 처음에 길이 정보를 처리
                a_len = pBuff[i++];
				if(a_len != 40) {
					a_len = a_len;
				}
                a_len0 = 0;
            }
            if(a_len != 255) {		// 이제 보내준 cal_data를 전부 받는다.
                if(a_len-a_len0 <= len-i) {
                    int k;
                    memcpy(&wbuf[a_len0], &pBuff[i], a_len-a_len0);
                    i += (a_len-a_len0);
                    status = INIT;
					// checksum 확인은 굳이 하지 않아도 될 듯..
					memcpy(&mPI, wbuf, sizeof(mPI));
                } else if(len-i > 0) {
                    memcpy(&wbuf[a_len0], &pBuff[i], len-i);
                    a_len0 += (len-i);
                    i += len;
                }
            }
            break; // D_DATA

        case A_DATA:
            if(a_len == 255) {		// 처음에 길이 정보를 처리
                a_len = pBuff[i++];
                a_len0 = 0;
            }
            if(a_len != 255) {		// 이제 가속도 data를 전부 받는다.
                if(a_len-a_len0 <= len-i) {		// 다 올라왔으면 처리한다 !!
                    int jx, jy, jz, k;
                    // 일단 처리를 위해 wbuf에 다 넣어준다.
                    memcpy(&wbuf[a_len0], &pBuff[i], a_len-a_len0);
                    i += (a_len-a_len0);
                    status = INIT;

                    for(j=0x55, k=0; k<a_len-1; k++) {
                        //					for(j=0x55, k=0; k<50; k++) { // 5*9+5 = 50
                        j += wbuf[k];
                    }
                    if(wbuf[a_len-1] == (unsigned char)(j&0xff)) {
                        GmoteState gmoteState;
                        char bitcheck = 1;
                        numSensors = 0;
                        //$$						mask = 0x01;
                        for(j=0, k=0; k<4; k++, bitcheck <<= 1) {	// 2008.11.29
                            struct calibration_info *cal = &CalibrationInfo[k];
                            int id = mSensorID[k];

							if(k==0) rl_Status = wbuf[2*D_GAP-1];			// 2010.3.23
							
							sw_data[k] = wbuf[j++];		// wbuf[0]
                            gmoteState.buttons = (~sw_data[k]) & 0x3f;

                            unsigned long ltmp;

                            psnd[k] = wbuf[j++];	// wbuf[1]
                            sample_num[k] = psnd[k] >> 6;
                            j+= 2;		// ref_timer 
                            // psnd[k] &= 0x3F;			// PMASK
                            for(int nn=0; nn<sample_num[k]+1; nn++) {
                                byte2long(ltmp, wbuf, j);	// wbuf[5*nn+2 ~ 5*nn+5]
                                x_data[k] = (ltmp >> 22) & 0x3FF;
                                y_data[k] = (ltmp >> 12) & 0x3FF;
                                z_data[k] = (ltmp >>  2) & 0x3FF;
                                t_data[k] = ((ltmp & 3) << 8) + (wbuf[j++] & 0xff) ;	// gyro data = wbuf[5*nn+6]
								if(sample_num[k] < 3 && nn==0) {		// 2009.12.15
									if((wbuf[j+13] & wbuf[j+14]) == 0) {
										if(wbuf[j+13] != 0)
										    batt_vtg[k] = (int)(1000*1.25*(72-wbuf[j+13])/27);
									}
								}
								
                                if(onBuffering[id] && ((mSlotAvail & bitcheck) == 0) ) {
                                    if(cal->XG == cal->X0)  gmoteState.accelX = 0.0;
                                    else  gmoteState.accelX = -((float)x_data[k]-cal->X0)/((float)cal->XG-cal->X0); 
                                    if(cal->YG == cal->Y0)  gmoteState.accelY = 0.0;
                                    else  gmoteState.accelZ = ((float)y_data[k]-cal->Y0)/((float)cal->YG-cal->Y0); 
                                    if(cal->ZG == cal->Z0)  gmoteState.accelZ = 0.0;
                                    else  gmoteState.accelY = ((float)z_data[k]-cal->Z0)/((float)cal->ZG-cal->Z0); 
                                    gmoteState.gyro = t_data[k];
									gmoteState.battery = (float)batt_vtg[k]*0.001;

                                    //--------------------------------------
                                    // (3000/0.67/1024) x Gyro 은 초당 동안 변한 각도(dps)를 의미
                                    // Gyro 값에서 각도 구하기
                                    // 전원에 따른 offset 값의 변동 : H/W 방식은 Pin을 하나 더 소모하므로 S/W 방식으로 해결
                                    // - offset estimation algorithm
                                    // 1) 그냥 low-pass filter 이용
                                    //   : 수렴 속도의 문제가 있음
                                    // 2) 비선형 filter 이용
                                    //   : 연속한 N(=20)개의 data의 값 변동이 기준값(AN=5) 이내이면
                                    //     그 평균값을 offset으로 설정
                                    //     (GyroAvgCnt, GyroSum, GyroMin, GyroMax 이용, Goffset = (int)(GyroSum / N))
                                    // gyro(int) : 8-bit ADC 출력값
                                    // GyroOffset(float) : gyro 평균 추정값
                                    // gyrofiltered(float) : (gyro - Goffset) 필터링한 값
                                    // YawVelocity : gyrofiltered * (3000/0.67/1024)
                                    // GyroAngle(float) : YawVelocity를 적분한 값
                                    if(initGyro[k] == 0) {
                                        initGyro[k] = 1;
                                        GyroAvgCnt[k] = 0;
                                        GyroOffset[k] = 0;
                                        gyrofiltered[k] = 0;
                                        GyroAngle[k] = 0;
                                    }
                                    // Remove invalid value : 임시방편
                                    //// if(gmoteState.gyro > 200) 
                                    ////	gmoteState.gyro = 0;
                                    // simple IIR lowpass filter
                                    // Acceleration[s].Goffset = 0.7*Acceleration[s].Goffset + 0.3*Acceleration[s].Gyro;
                                    // nonlinear filter
                                    if(GyroAvgCnt[k] < N_GYRO_STEADY) {	
                                        GyroAvgCnt[k]++;
                                        if(GyroAvgCnt[k] == 1) {
                                            GyroMin[k] = GyroMax[k] = GyroSum[k] = gmoteState.gyro;
                                        }
                                        else if(GyroMax[k] - GyroMin[k] > N_GYRO_MARGIN) {	// not steady state
                                            GyroAvgCnt[k] = 0;
                                        } else {
                                            if(GyroMax[k] < gmoteState.gyro) 
                                                GyroMax[k] = gmoteState.gyro;
                                            if(GyroMin[k] > gmoteState.gyro) 
                                                GyroMin[k] = gmoteState.gyro;
                                            GyroSum[k] += gmoteState.gyro;
                                        }
                                    } else {
                                        float newOffset = GyroSum[k] / GyroAvgCnt[k];
                                        if(GyroOffset[k] == 0)
                                            GyroOffset[k] = newOffset;
                                        else if( fabsf(GyroOffset[k]-newOffset)< 20.0 )
                                            GyroOffset[k] = 0.02*GyroOffset[k] + 0.98*newOffset;
                                        GyroAvgCnt[k] = 0;
                                    }
                                    if(GyroOffset[k] == 0) 
                                        gmoteState.gyro = 0;
                                    // 
                                    // gyrofiltered[k] = 0.01*gyrofiltered[k] + 0.99*(gmoteState.gyro +1 - GyroOffset[k]); 
									// for test LSB effect !! --> 1 LSB = 3mV = 4.37 dps ( 1 dps = 0.67 mV )
                                    gyrofiltered[k] = 0.01*gyrofiltered[k] + 0.99*(gmoteState.gyro - GyroOffset[k]);
                                    //
                                    gmoteState.yawVelocity = (3000/0.67/1024)*gyrofiltered[k];
                                    GyroAngle[k] += 0.01*gmoteState.yawVelocity ;
#ifdef USE_GYRO_LOG
                                    logcnt++;
                                    if(logcnt++ > 100) {
                                        logcnt = 100;
                                        if(fpGyro[k] == NULL) {
                                            char FNAME[] = "gyro?.m";
                                            FNAME[4] = '0'+k;
                                            fpGyro[k] = fopen(FNAME,"wt");
                                            fprintf(fpGyro[k], "g%d = [\n", k);
                                        }
                                        else {
                                            fprintf(fpGyro[k], "%d %f %f %f;\n", gmoteState.gyro, GyroOffset[k], gmoteState.yawVelocity*0.01, GyroAngle[k]);
                                        }
                                    }
#endif
                                    //--------------------------------------




                                    if( !((x_data[k] == 0) && (y_data[k]==0) && (z_data[k]==0)) ) {		// 2008.12.5

										if(rl_Status != 0) {
											if(k==0) {
												if(rl_Status == 1) gmoteState.rightFlag = false;
												else						gmoteState.rightFlag = true;
											} else {
												if(rl_Status == 1) gmoteState.rightFlag = true;
												else						gmoteState.rightFlag = false;
											}
										} else {
											gmoteState.rightFlag = true;
										}

//										gmoteState.rightFlag = rl_Status;
                                        pushState(id, gmoteState);

										if(use_time_log && nn==0) {		// 2009.12.14 Doohee                                            newclock = clock();
                                            if(fptime == NULL) fptime = fopen("time.txt","wt");
                                            else 
                                                if(oldclock != 0) fprintf(fptime, "%ld = %ld - %ld(%d)\n", newclock-oldclock, newclock, oldclock, sample_num[k]);
                                            oldclock = newclock;
                                        }
                                    }
									else {
										if((k==0) && (nn==0) ) {
											if((MissCnt/10) != ((MissCnt+1)/10))
										       PRINTF("MISS_CNT = %d", MissCnt++);
										}
									}
#ifdef USE_FPLOG
                                    if(fpLog != NULL) {
                                        fprintf(fpLog, "idx=%02x: x=%04d y=%04d z=%04d g=%03d\n", (psnd[k]+nn)&0x3F, x_data[k], y_data[k], z_data[k], t_data[k]);
                                    } else {
                                        fpLog = fopen("log.dat","at");
                                        fprintf(fpLog, "\n#################\n");
                                    }
#endif								
                                }
                            }
                            j += ((3-sample_num[k])*5);



                            // test 용으로 gyro_data 자리에 참조 data를 넣어주었음
                            // t_data[k] = wbuf[6];
                            // t_data[k] = (t_data[k]<<8) + wbuf[11];	// ref_timer here !!

                            // t_data[k] = k;	// for test only !!
                            // --------------------------
                            // put data to DataArray
                            /*
                            {
                            short idx = Dat[k].acq_index;
                            Dat[k].s[idx].xdata = x_data[k];
                            Dat[k].s[idx].ydata = y_data[k];
                            Dat[k].s[idx].zdata = z_data[k];
                            Dat[k].s[idx].gdata = t_data[k];
                            idx++;
                            if(idx >= DBUF_SIZE) idx = 0;
                            Dat[k].acq_index = idx;
                            }
                            */
                            // --------------------------
                            if(k==0) {
								mSlotAvail = wbuf[j] & 0x0F;	// 2009.10.31 doohee : 2009.12.13
								mChan = (wbuf[j]>>4);			// 2009.10.31 doohee : 2009.12.13
                            }
                            else if(k==1) {
                                mAcqCnt = wbuf[j];				// 2008.1.24 doohee
                            }
                            else if(k==2) {
                                mPilotCntOld = mPilotCnt;		// 2008.1.27 doohee
                                mPilotCnt = wbuf[j];				// 2008.1.24 doohee
                                if(mPilotCntOld == mPilotCnt) {	// 2008.1.27 doohee ...
                                    if(this->IsConnected()) mDeadLockCnt++;
                                    if(mDeadLockCnt == 3) {
                                        // if(this->IsConnected())
                                        // 	mRebootDongle = true;
                                        if(errLog == NULL) errLog = fopen("errLog.txt", "at");
                                        if(errLog) {
                                            time_t cur = time(NULL);
                                            fprintf(errLog, "DEADLOCK:RESET) %ld %s ", timeGetTime(), ctime(&cur));
                                            //PRINTF("DEADLOCK:RESET) %ld %s ", timeGetTime(), ctime(&cur));
                                            fclose(errLog);
                                            errLog = NULL;
                                        } else {  // 혹시 안열릴까봐 : 불필요해 보이는 듯..
                                            errLog = fopen("errLog.txt", "at");
                                        }
                                    } else if(mDeadLockCnt > 3)
                                        mDeadLockCnt = 4;
                                } else
                                    mDeadLockCnt = 0;			// ....................
                            }
                            else if(k==3)
                                gu8RTxMode = wbuf[j];				// 2008.1.24 doohee

                            j += 1;								// 2008.1.1 doohee

                            jx=x_data[k] / HASH_RES;
                            // if(jx >=HASH_MAX) jx = HASH_MAX;
                            jy=y_data[k] / HASH_RES;
                            // if(jy >=HASH_MAX) jy = HASH_MAX;
                            jz=z_data[k] / HASH_RES;
                            // if(jz >=HASH_MAX) jz = HASH_MAX;

                            {
                                if(x_data[k] != 0 && y_data[k] != 0 && z_data[k] != 0) 
                                {
                                    ParseAccel(&wbuf[k*D_GAP+4], k);
                                }
                            }
                        }
                        mOKCnt1++;
                    } else {
                        mErrCnt1++;
                    }

                } else if(len-i > 0) {
                    memcpy(&wbuf[a_len0], &pBuff[i], len-i);
                    a_len0 += (len-i);
                    i += len;
                }
            }
            break;

        default:
            status = INIT;
            i++;
            break;
        }
    }
}
/*
void ZMotioner::SendReqAudio(int id, int index)	// 2009.12.11
{
	unsigned char data = 0x01, k;

    for(k=0; k<3; k++, data<<=1) 
		if(id == mSensorID[k]) break;
	if(k==3) {
		if(id==0) data = 0x0f;	// play all sensor !
		else {
			PRINTF("Invalid SendReqAudio Command\n");
			return;
		}
	} else {
		data >>= 1;
	}
    if(index == 1) data |= 0x10;
	if(index == 2) data |= 0x20;
	if(index == 3) data |= 0x30;
	mAudioData = data;
	mAudioFlag = true;
}
*/
void ZMotioner::SendCalibData (unsigned char *data)
{
    for(int i=0; i<14; i++) {
        mCalibData[i] = data[i];
    }
    mCalibFlag = true;
}


short ZMotioner::GetGyroData (char index)
{
    if((index < 6) && ((index & 0x80)==0)) {
        return t_data[index];
    } else {
        return 0;
    }
}


// ------------------------------------------------------------------------------------
int ZMotioner::ParseAccel (BYTE* buff, char s)
{

    int changed = 0, j=0 ;
    unsigned long ltmp;
    byte2long(ltmp, buff, j);
    WORD raw_x = (ltmp >> 22) & 0x3FF;
    WORD raw_y = (ltmp >> 12) & 0x3FF;
    WORD raw_z = (ltmp >>  2) & 0x3FF;
    if(s >= 5) return 0;

    if((raw_x != Acceleration[s].RawX) ||
        (raw_y != Acceleration[s].RawY) ||
        (raw_z != Acceleration[s].RawZ))
        changed |= ACCEL_CHANGED;

    Acceleration[s].RawX = raw_x;
    Acceleration[s].RawY = raw_y;
    Acceleration[s].RawZ = raw_z;

    if(CalibrationInfo[s].X0 == CalibrationInfo[s].XG) return 0;
    if(CalibrationInfo[s].Y0 == CalibrationInfo[s].YG) return 0;
    if(CalibrationInfo[s].Z0 == CalibrationInfo[s].ZG) return 0;

    Acceleration[s].X = ((float)Acceleration[s].RawX  - CalibrationInfo[s].X0) / 
        ((float)CalibrationInfo[s].XG - CalibrationInfo[s].X0);
    Acceleration[s].Y = ((float)Acceleration[s].RawY  - CalibrationInfo[s].Y0) /
        ((float)CalibrationInfo[s].YG - CalibrationInfo[s].Y0);
    Acceleration[s].Z = ((float)Acceleration[s].RawZ  - CalibrationInfo[s].Z0) /
        ((float)CalibrationInfo[s].ZG - CalibrationInfo[s].Z0);

    // see if we can estimate the orientation from the current values
    if(EstimateOrientationFrom(Acceleration[s], s)) {
        changed |= ORIENTATION_CHANGED;
    }

    return changed;
}
// ------------------------------------------------------------------------------------
bool ZMotioner::EstimateOrientationFrom (struct acceleration &accel, int s)
{
    // Orientation estimate from acceleration data (shared between wiimote and nunchuk)
    //  return true if the orientation was updated

    //  assume the controller is stationary if the acceleration vector is near
    //  1g for several updates (this may not always be correct)
    float length_sq = square(accel.X) + square(accel.Y) + square(accel.Z);

    // TODO: as I'm comparing squared length, I really need different
    //		  min/max epsilons...
#define DOT(x1,y1,z1, x2,y2,z2)	((x1*x2) + (y1*y2) + (z1*z2))

    static const float epsilon = 0.5f;
    ////	static const float epsilon = 0.2f;
    if((length_sq >= (1.f-epsilon)) && (length_sq <= (1.f+epsilon)))
    {
        ////		if(++ZmoteNearGUpdates[s] < 5)
        if(++ZmoteNearGUpdates[s] < 2)
            return false;

        // wiimote seems to be stationary:  normalize the current acceleration
        //  (ie. the assumed gravity vector)
        float inv_len = 1.f / sqrtf(length_sq);
        float x = accel.X * inv_len;
        float y = accel.Y * inv_len;
        float z = accel.Z * inv_len;

        // copy the values
        accel.Orientation.X = x;
        accel.Orientation.Y = y;
        accel.Orientation.Z = z;

        // and extract pitch & roll from them:
        // (may not be optimal)
        float pitch = -asinf(y) * 57.2957795f;
        float roll  =  asinf(x) * 57.2957795f;
        if(z < 0) {
            pitch = (y < 0)?  180 - pitch : -180 - pitch;
            roll  = (x < 0)? -180 - roll  :  180 - roll;
        }

        accel.Orientation.Pitch = pitch;
        accel.Orientation.Roll  = roll;

        // show that we just updated orientation
        accel.Orientation.UpdateAge = 0;
#ifdef BEEP_ON_ORIENTATION_ESTIMATE
        Beep(2000, 1);
#endif
#if 1
        // printf("x=%03d, y=%03d, z=%03d, p=% 7.2f, r=% 7.2f", accel.RawX, accel.RawY, accel.RawZ, pitch, roll);
#else
        printf("x=%f, y=%f, z=%f, p=%f, r=%f", x,y,z, pitch, roll);
#endif
        return true; // updated
    }
    else
    {
        // printf("x=%d, y=%d, z=%d", accel.RawX, accel.RawY, accel.RawZ);
    }
    // not updated this time:
    ZmoteNearGUpdates[s]	= 0;
    // age the last orientation update
    accel.Orientation.UpdateAge++;
    return false;
}
