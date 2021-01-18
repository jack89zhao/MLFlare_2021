//
//  MLFlare.h
//  MLFlare
//
//  Created by Jackie Wang on 2019/7/2.
//  Copyright © 2019 Jackie Wang. All rights reserved.
//

#ifndef MLFlare_h
#define MLFlare_h

#define MLMaxAxisCount  16      // 最大支持的x轴的数量
#define MLPluseOutMode  0       // 0 - 6 7种脉冲输出模式

#define MLMaxDUTTypeCount   256  // 最大支持的DUT类型数量

#define MLDefaultLogPath    "~/Documents/Flare/Logs"
#define MLConfigFileName    "~/Documents/Flare/profile.ini"

#include <stdio.h>
#include <stdbool.h>

#ifndef DWORD
typedef unsigned long   DWORD;
#endif

#ifndef bool
typedef unsigned char   bool;
#endif

#ifndef uint8
typedef unsigned char   uint8;
#endif

#ifndef WORD
typedef unsigned short  WORD;
#endif

#ifndef int32
typedef int   int32;
#endif

#ifndef int64
typedef long long  int64;
#endif

#ifndef uint32
typedef unsigned int   uint32;
#endif

#ifndef uint64
typedef unsigned long long  uint64;
#endif

#ifndef string
typedef char* string;
#endif

typedef enum {
    MLLogInfo = 0,
    MLLogWarning,
    MLLogError,
} MLLogLevel;   // log等级级别

typedef enum {
    None = 0,
    Serial = 1,     // 串口模式
    Ethernet,       // 网络模式
} MLLinkType;   // 链接模式

typedef enum {
    MLLow,          // 逻辑低电平
    MLHigh,         // 逻辑高电平
} MLLevel;

typedef enum {
    MLNormal = 0,               // 正常停止
    MLIMD_STOP_AT_ALM,          // ALM 立即停止
    MLDEC_STOP_AT_ALM,          // ALM 减速停止
    MLIMD_STOP_AT_LTC,          // LTC 外部触发立即停止
    MLIMD_STOP_AT_EMG,          // EMG 立即停止
    MLIMD_STOP_AT_ELP,          // 正硬限位立即停止
    MLIMD_STOP_AT_ELN,          // 负硬限位立即停止
    MLDEC_STOP_AT_ELP,          // 正硬限位减速停止
    MLDEC_STOP_AT_ELN,          // 负硬限位减速停止
    MLIMD_STOP_AT_SOFT_ELP,     // 正软限位立即停止
    MLIMD_STOP_AT_SOFT_ELN,     // 负软限位立即停止
    MLDEC_STOP_AT_SOFT_ELP,     // 正软限位减速停止
    MLDEC_STOP_AT_SOFT_ELN,     // 负软限位减速停止
    MLIMD_STOP_AT_CMD,          // 命令立即停止
    MLDEC_STOP_AT_CMD,          // 命令减速停止
    MLIMD_STOP_AT_OTHER,        // 其它原因立即停止
    MLDEC_STOP_AT_OTHER,        // 其它原因减速停止
    MLIMD_STOP_AT_UNKOWN,       // 未知原因立即停止
    MLDEC_STOP_AT_UNKOWN,       // 未知原因减速停止
    MLDEC_STOP_AT_DEC,          // 外部 IO 减速停止
} MLStopReason;

typedef enum {
    MLInServoAlarm1     = 0,
    MLInServoAlarm2     = 1,
    MLInServoAlarm3     = 2,
    MLInServoAlarm4     = 3,
    MLInServoAlarm5     = 4,
    MLInServoAlarm6     = 5,
    MLInServoAlarm7     = 6,
    MLInServoAlarm8     = 7,
    MLInDoorLeftClosed  = 8,
    MLInDoorRightClosed = 9,
    MLInCylinderHome    = 10,
    MLInCylinderShop    = 11,
    MLInRaster          = 12,
    MLInEmgStop         = 13,
    MLInStart           = 14,
    MLInStop            = 15,
    MLInReset           = 16,
    MLInDoorLeftOpened  = 17,
    MLInDoorRightOpened = 18,
} MLInSensor;

typedef enum {
    MLOutServoBrake1        = 0,
    MLOutServoBrake2        = 1,
    MLOutServoBrake3        = 2,
    MLOutStepperBrake1      = 3,
    MLOutStepperBrake2      = 4,
    MLOutLightPower         = 5,
    MLOutAxisEnabled1       = 6,
    MLOutAxisEnabled2       = 7,
    MLOutAxisEnabled3       = 8,
    MLOutAxisEnabled4       = 9,
    MLOutAxisEnabled5       = 10,
    MLOutAxisEnabled6       = 11,
    MLOutAxisEnabled7       = 12,
    MLOutAxisEnabled8       = 13,
    MLOutSpotPower          = 14,
    MLOutCylinderHome       = 15,
    MLOutCylinderShop       = 16,
    MLOutLeftDoorOpen       = 17,
    MLOutLeftDoorClose      = 18,
    MLOutRightDoorOpen      = 19,
    MLOutRightDoorClose     = 20,
    MLOutLight              = 21,
    MLOutLightStop          = 22,
    MLOutLightStart         = 23,
    MLOutLightReset         = 24,
    MLOutLaserPower         = 25,
    MLOutIlluminanceLight   = 26,
} MLOutSensor;

typedef enum {
    MLAxisX = 11,
    MLAxisY = 12,
    MLAxisClampUp = 5,
    MLAxisClampLeft = 8,
    MLAxisClampDown = 7,
    MLAxisClampRight = 6,
    MLAxisLifter = 4,
    MLAxisRotationX = 2,
    MLAxisRotationY = 3,
    MLAxisRotation = 1,
    MLAxisLight =9,
    MLAxisLaser = 10,
} MLAxis;

typedef struct {
    int axis;
    double startSpeed;
    double runSpeed;
    double stopSpeed;
    double homeSpeed;
    double accTime;
    double equiv;
    double backlash;
    int homeDirect;
    MLLevel homeLevel;
    WORD mode;
    double ppratio;
    // init location after go home.
    double posToInit;       // pulse unit(absolute)
    double posToTest;       // pulse unit(absolute)
    bool isRuning;
} AxisParam;

typedef struct {
    int type;
    string name;
    double posStandbyFront;
    double posStandbyBack;
    double posStandbyLeft;
    double posStandbyRight;
    double posHoldFront;
    double posHoldBack;
    double posHoldLeft;
    double posHoldRight;
    double posLifter;
    double posAxisX;
    double posAxisY;
} MLDUT;

typedef void (* func)(double val1, double val2);

char* GetLibraryVersion(void);

/**
 *@ingroup MLFlare
 *@brief Connect Leisai motion controller.
 *@par Descrption:
 *This API is used to initialization motion controller.
 *
 *@attention
 *<ul>
 *<li>None.</li>
 *</ul>
 *
 *@param ip [IN] A IP address to connect motion controller.
 *
 *@return true  Success to connect motion controller, otherwise fail to connect motion controller.
 *
 *@par Dependency:
 *<ul><li>MLFlare.h</li></ul>
 *@see None.
 *@since Microtest Co,.LTD
 */

bool Connect(char *ip);
/**
 *@brief Disconnect motion controller.
 */
void Disconnect(void);
/**
 *@brief Move the axis to the specified position in absolute mode
 *@param axis the axis need to move.
 *@param position the position which axis need move to.
 *@param blocked block flag, if true, will wait move finish then return
 *@param pthread Is runing it in thread.
 *
 *@return the axis's current position.
 */
long MoveAxisToPosition(int axis, double position, bool blocked, bool pthread);
/**
 *@brief Move the axis to the specified position in relative mode
 *@param axis the axis need to move.
 *@param distance the distance which axis need move to.
 *@param blocked block flag, if true, will wait move finish then return
 *@param pthread Is runing it in thread.
 *
 *@return the axis's current position.
 */
bool MoveAxisDistance(int axis, double distance, bool blocked, bool pthread);
/**
 *@brief Move one axis go to the zero position.
 *@param axis the axis need to move.
 *@param blocked block flag, if true, will wait move finish then return
 *
 */
void AxisGoHome(int axis, bool blocked);
/**
 *@brief Move all axises go to zero position.
 *@param pthread Is runing it in thread.
 */
void AllAxisGoHome(bool pthread);
/**
 *@brief Move some axises go to zero position.
 *@param axises the axises need to move.
 *@param axisCount the count of axises which need move to.
 *@param pthread Is runing it in thread.
 *
 *@attention The API running at block mode.
 */
void ManyAxisGoHome(int *axises, int axisCount, bool pthread);
/**
 *@brief Check some axises's state.
 *@param axises the axises need to move.
 *@param axisCount the count of axises which need move to.
 *@param pthread Is runing it in thread.
 *
 *@attention The API will only return true, if axis not stop, will wait it forever.
 *
 *@return if all axises have stop, return true, otherwise block it and wait axis stop.
 */
bool CheckAxisState(int *axises, int axisCount, bool pthread);
/**
 *@brief Check one axis's IO state, include servo alarm, negative/positive limit, ORG signal, EMG.
 *0 -> Servo drive alarm, 1 -> positive limit, 2 -> negative limit, 3 -> EMG, 4 -> ORG
 *@param axis the axis need to check.
 *
 *@return normally return 10, if one sensor is actived, will return 1,2,3,4
 */
int CheckAxisIOState(int axis);
/**
 *@brief Check some axises's state.
 *@see `CheckAxisState(int*, int)`
 */
bool CheckMutliAxisState(int *axises, int axisCount, func callbacck, double value1, double value2);
/**
 *@brief Move one axis using j-mode.
 *@param axis the axis need to move.
 *@param direction the direction of axis moving.
 *@param pthread Is runing it in thread.
 */
void JMoveAxis(int axis, int direction, bool pthread);
/**
 *@brief Move one axis using j-mode and will block it.
 *@param axis the axis need to move.
 *@param direction the direction of axis moving.
 *@param pthread Is runing it in thread.
 *
 *@attention The API will block to wait axis stop, If the axis stops abnormally, will wait it forever.
 *
 *@see `JMoveAxis(int, int)`
 *
 */
void JMoveAxisWithBlock(int axis, int direction, bool pthread);
/**
 *@brief Move one axis using j-mode, the speed is fixed (5000 pluse).
 *@param axis the axis need to move.
 *@param direction the direction of axis moving.
 *
 *@see `JMoveAxis(int, int)`
 */
void JHoldMoveAxis(int axis, int direction);
void ChangeAxisParam(int axis, double startSpeed, double runSpeed, double acc);
void ConfigSpeed(int axis, int speed, int mode);
/**
 *@brief stop one axis.
 *@param axis the axis need to stop.
 */
void AxisStop(int axis);
/**
 *@brief stop all axises.
 */
void AllAxisStop(void);

bool IsStopActived(void);
bool IsMoveFinished(void);
void InactiveStop(void);
void MoveToInitPos(int *axises, int count);
void CheckStopActivedSignal(void);

char *GetErrorMessage(void);

void SetAxisRatio(int axis, double ratio);
void Cylinder(int bit, MLLevel level);
void stopSingleChange(void);

void Encoder(void);
void SetEncoderUnit(void);
void SetBitState(int bit, MLLevel level);
MLLevel GetBitState(int bit);

// get axis's param.
AxisParam GetAxisParam(int axis);
bool SetAxisParam(int axis, AxisParam param);

// Illuminometer API
bool ConnectIlluminometer(string portName);
void DisconnectIlluminometer(void);
double GetIlluminanceValue(int timeout);
double GetColorX(int timeout);
double GetColorY(int timeout);
void ConfigIlluminometerPortName(string portName);

// Laser API
//int LoadLaser(string portName);
double GetLaserDiffOfHeight(string portName);

// Loadcell API
double GetLoadCellValue(string portName);
void ConfigLoadCellPortName(string frontPortName, string backPortName, string leftPortName, string rightPortName);

// Angle API
void ConfigAnglePortName(string portName);
double GetAngleValue(string portName,char*bufx,char*bufy,char*buft);
double GetAngleDoubleValue(string portName, double *anglex, double *angley, double *tem);

// check limit sensor state, if return value is less than 0, some axis's limit is broken.
int CheckAllLimitSensor(void);

// laser DUT measure position.(unit:fmm)
double GetLaserDUTPos(void);
// laser spot measure postion.(unit:mm)
double GetLaserSpotPos(void);

// set the current position of all axes as the base data.
// mode=0: just save current position;
// mode=1: save current position and change the calibration data of all projects
void SetCalibrationBaseData(int mode);
// get the offset data of calibration
void GetCalibrationOffset(float *offsetData);

// ================= Union API =====================
/**
 *@ingroup MLFlare
 *@brief Initial system, load config parameters.
 *@par Description:
 *This API should be called **before** `Connect(ip)`
 *@return return true if success, otherwise return false.
 *
 *@attention None
 *
 *@see None.
 */
bool InitializeSystem(void);
/**
 *@ingroup MLFlare
 *@brief Release and save config parameters.
 *@par Description:
 *This API should be called **after** `Disconnect`. When exit system, this api should be called.
 *@return return true if success, otherwise return false.
 *
 *@attention When exit system, this api should be called.
 *
 *@see None.
 */
bool ReleaseSysResource(void);
/**
 *@ingroup MLFlare
 *@brief Reset all axis to ORG.
 *@par Description:
 *Will stop all axises firstly, then all axis go to ORG.
 *
 *@attention None
 *
 *@see None.
 */
void ResetAxisSystem(void);
/**
 *@ingroup MLFlare
 *@brief It will check input sensor include **raster**,**emg stop**, **stop**, **start** etc by call `CheckSensor()`.
 *@par Description:
 *None.
 *
 *@attention This API will be called at `Connect()`, so please not call it again.
 *
 *@see None.
 */
void CheckInputSignal(void);
/* called by `CheckInputSignal` in thread. */
int CheckSensor(void);
/**
 *@ingroup MLFlare
 *@brief Connect luxmeter, then read the luxmeter value, finally disconnect it.
 *@par Description:
 *This API called the `ConnectIlluminometer(char *)`, `GetIlluminanceValue(int)` and `DisconnectIlluminometer()`,
 *The default timeout is 5s.
 *
 *@param portName device name.
 *@param offset This value is used to check whether the automatic calibration is up to standard.
 *@param timeout The timeout of read one value every time.
 *@attention None.
 *
 *@see None.
 *
 *@return Actual offset。
 */
double IlluminanceMeasure(string portName, double offset, int timeout);
/**
 *@ingroup MLFlare
 *@brief Connect laser, then read the laser value, finally disconnect it.
 *@par Description:
 *This API will call laser functions and motion move functions to complete the auto calibration.
 *
 *@param offset This value is used to check whether the automatic calibration is up to standard.
 *@attention None.
 *
 *@see None.
 *
 *@return Actual offset。
 */
double LaserCalibrator(double offset);
/**
 *@ingroup MLFlare
 *@brief Open auto-door.
 *@par Description:
 *When open auto-door, the light will open also.
*@return the result of auto-door
 *
 *@attention None.
 *
 *@see None.
 *
 */
bool DoorOpen(void);
/**
 *@ingroup MLFlare
 *@brief close auto-door.
 *@par Description:
 *When close auto-door, the light will close also.
 *@return the result of auto-door
 *
 *@attention None.
 *
 *@see None.
 *
 */
bool DoorClose(void);
/**
 *@ingroup MLFlare
 *@brief Auto hold the holder, when the loadcell value is ok, stop move and get the current postion of axis.
 *@par Description:
 *There are four axises moving, the left and front axis move firstly, then the right
 *and back axis move it. When moving, will read the loadcell value, if the value is
 *more than the value we set, stop the moving.
 *         front
 *       ---------
 *      |         |
 * left |   DUT   | right
 *      |         |
 *      |         |
 *       ---------
 *         back
 *
 *@param dutType DUT type. The DUT type defined as 0, 1, 2, 3...
 *@param force1 the force need be check at right loadcell.
 *@param force2 the force need be check at back loadcell.
 *
 *@attention Potential risk is when loadcell broken, the axis will not stop it.
 *
 *@see None.
 *
 */
void ManualHoldDut(int dutType, double force1, double force2);
/**
 *@ingroup MLFlare
 *@brief Adjust hold location.
 *
 *@param direction The moving direction, including 0(left), 1(right), 2(front), 3(back).
 *@param step the step which aixs need moving, the unit is mm.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void AdjustAxisPosistion(int direction, int step);
/**
 *@ingroup MLFlare
 *@brief Auto hold the holder, will not check the loadcell value, just go to the position which saved before.
 *@par Description:
 *         front
 *       ---------
 *      |         |
 * left |   DUT   | right
 *      |         |
 *      |         |
 *       ---------
 *         back
 *
 *@param dutType DUT type. The DUT type defined as 0, 1, 2, 3...
 *
 *@attention None.
 *
 *@see None.
 *
 */
bool HoldDut(int dutType);
/**
 *@ingroup MLFlare
 *@brief unhold the DUT.
 *@par Description:
 *__This API not verify right now__.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void UnholdDut(int dutType);
/**
 *@ingroup MLFlare
 *@brief Swing some angle.
 *@par Description:None.
 *
 *@param angleX the angle of x-rotate axis need to swing.
 *@param angleY the angle of y-rotate axis need to swing.
 *@param rotate the angle of r-rotate axis need to swing.
 *@param directionX the direction of x-rotate axis need to swing, it should 1 or -1.
 *@param directionY the direction of y-rotate axis need to swing, it should 1 or -1.
 *@param directionR the direction of r-rotate axis need to swing, it should 1 or -1.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void SwingAngle(double angleX, double angleY, double rotate, int directionX, int directionY, int directionR);
/**
 *@ingroup MLFlare
 *@brief Angle swing test, which including r-rotate rotate 45, 90, 135 degress, y-rotate swing degress
 *from -90 to 90 degress.
 *@par Description:None.
 *
 *@param dutType the dut type which need to swing.
 *@param force1 the force need be check at right loadcell.
 *@param force2 the force need be check at back loadcell.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void SwingAngleTest(int dutType, double force1, double force2,double angleV,int testCount);
/**
 *@ingroup MLFlare
 *@brief Change the laser's portname.
 *@par Description:None.
 *
 *@param portName new laser's portname.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void ConfigLaserPortName(string portName);
/**
 *@ingroup MLFlare
 *@brief Change the laser's portname.
 *@par Description:None.
 *
 *@param axis the axis need to config ratio.
 *@param ppratio the ratio of pluse/mm
 *
 *@attention None.
 *
 *@see None.
 *
 */
void ConfigPPRatio(int axis, double ppratio);
//void RotateAngle(double angle);
//void SwingAngle(double angle, int direction);
//void SwingOneAngle(int direction);
double ReadDDAngle(void);
/**
 *@ingroup MLFlare
 *@brief many axis go back to negative limit.
 *@par Description:None.
 *
 *@param axises the axises need to go back negative limit.
 *@param axisCount the count of axis.
 *@param pthread Is runing it in thread.
 *
 *@attention Potential risk is two axis will collision.
 *
 *@see None.
 *
 */
void ManyAxisGoBack(int *axises, int axisCount, bool pthread);
bool SetActivedDUT(int type);
/**
 *@ingroup MLFlare
 *@brief Save one DUT type. How do determine the type of dut? First,
 *call `GetCountOfDUTType()` to get the current count,
 *then call `AddDUTTypeAndName(int, string)`.
 *
 *@example
 *```
 *  int cnt = GetCountOfDUTType();
 *  AddDUTTypeAndName(cnt,"DemoDUT");
 *```
 *
 *@param type dut type, should be 0,1,2...
 *@param name dut's name.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void AddDUTTypeAndName(int type, string name);
/**
 *@ingroup MLFlare
 *@brief Save one DUT type. How do determine the type of dut? First,
 *call `GetCountOfDUTType()` to get the current count,
 *then call `AddDUTTypeAndName(int, string, ...)`.
 *
 *@example
 *```
 *  int cnt = GetCountOfDUTType();
 *  AddDUTTypeAndName(cnt,"DemoDUT", 1, 1, 1, 1);
 *```
 *
 *@param type dut type, should be 0,1,2...
 *@param name dut's name.
 *@param posFrontHold postion to hold DUT from front side.
 *@param posBackHold postion to hold DUT from back side.
 *@param posLeftHold postion to hold DUT from left side.
 *@param posRightHold postion to hold DUT from right side.
 *
 *@attention None.
 *
 *@see None.
 *
 */
void AddDUTType(int type, string name, double posFrontHold, double posRightHold, double posBackHold, double posLeftHold,double posAxisX,double posAxisY,double posLifter);
/**
 *@ingroup MLFlare
 *@brief Get the current DUT type's count.
 *@attention None.
 *@see AddDUTTypeAndName, AddDUTType
 *
 *@return The count of DUT type.
 */
int GetCountOfDUTType(void);
/**
 *@ingroup MLFlare
 *@brief Get the dut's name when give the dut type.
 *@attention None.
 *@see GetCountOfDUTType()
 *
 *@return The name of DUT.
 */
const char* GetDUTName(int type);
/**
 *@ingroup MLFlare
 *@brief Active DUT, the hold axises will run to the test position.
 *
 *@param type dut type.
 *@param name dut name.
 *
 *@attention None.
 *
 *@see GetCountOfDUTType()
 *
 */
void LookDUTTypeAndName(int type, string name);
/**
 *@ingroup MLFlare
 *@brief Hold one DUT.
 *
 *@param dutType dut type.
 *@param force1 the force need be check at right loadcell.
 *@param force2 the force need be check at back loadcell.
 *
 *@attention None.
 *
 *@see None
 *
 */
void HoldOneDUTDemo(int dutType,int force1,int force2);
// read the encoder value
double ReadEncoderValue(int axis);
//get dut type name
const char* GetTypeName(int type);
//adjust hold position
void AdjustHoldPosition(int *axises, double distance, bool blocked);
//enable axis
void PowerOn(void);
//disenable axis
void PowerOff(void);

//Illuminance Power On
//bool IlluminancePowerOn(string portName);
//Get Illuminance Value
double IlluminanceValue(void);
//Illuminance Power Off
//void IlluminancePowerOff(void);

void LightSourceON(void);
void LightSourceOFF(void);
bool LuxMeterON(string portName);
void LuxMeterOFF(void);

/*
 * Connect luxmeter and read value as calibrate value.
 */
double GetLuxmeter(string portName);
void LuxmeterMeasure(double measureValues[], int size, int timeout);
bool MoveBackLuxmeter(void);

double MLLaserCalibrator(double offset);
void MLManualHoldDut(double force1, double force2);

//Swing Angle Absolute
void SwingAngleAbsolute(double angleX, double angleY, double rotate, int directionX, int directionY, int directionR);

void MLSwingAngleAbsolute(double angleX, double angleY, double rotate, int directionX, int directionY, int directionR, bool pthread);

void MLSwingAngle(double angleX, double angleY, double rotate, int directionX, int directionY, int directionR, bool pthread);
//Option Swing Angle
void SwingDegreesOption(double angleStep,double fromAngle,double toAngle);

void MLSwingDegreesOption(double angleStep,double fromAngle,double toAngle,int delay, bool pthread);
//get axis position
double GetAxisPosition(int axis);

//get Illuminance Value
double MLGetIlluminanceValue(void);

//Connect Illuminometer port
bool MLConnectIlluminometer(string portName);

//Connect XJC608T Port
int MLConnectXJCPort(const char* portName);

//Read XJC608T Value
double MLReadXJCExisting(int fd,char *str);

//Disconnect XJC608T Port
void MLDisconnectXJCPort(int fd);

// Zero XJC608T value
void MLZeroXJCValue(int fd);

//Debug Method
void DebugManyAxisMovePAndN(int *axises, int axisCount,int testCount);
void DebugLaserCalibrator(int testCount);
void DebugIlluminanceMeasure(int testCount);
//void DebugManualHoldDut(int testCount);
void DebugLookDUTTypeAndName(int type, string name);
bool DebugHoldDut(int dutType, double force1, double force2,int testCount);
void DebugHold1KMove(int *axises, int axisCount,int testCount);
void DebugSwingAngleTest(int dutType, double force1, double force2,double angleV,int testCount);
void DebugSwing90Degrees(double angleValue,int testCount);
void DebugRotationAngleTest(double angleV,int testCount);

char* GetProjectName(void);

// thread methods
void TManyAxisGoHome(void *args);
void TJMoveAxisWithBlock(void *args);
void TMoveAxisToPosition(void *args);
void TMoveAxisDistance(void *args);
void TAllAxisGoHome(void);
void TManyAxisGoBack(void *args);
void TJMoveAxis(void *args);
bool CheckStartSignal(void);
bool CheckStopSignal(void);
void CheckStart(void);
void CheckStop(void);
void CheckEmgStop(void);
bool CheckEmgStopSignal(void);
#endif /* MLFlare_h */
                        
