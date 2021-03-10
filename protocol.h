#ifndef PROTOCOL_H
#define PROTOCOL_H


#include <vector>
#include <string>

typedef struct {
    std::string strLName; //层名
    unsigned int uSN; //序列号
    unsigned int uIndex; //编号
    unsigned int uLDepth; //层深 (0~uWellDepth]
    unsigned int uFlowRate; //目标流量值
    unsigned int uOpenDegree; //开度
    unsigned int uFlowAdjInr; //流量调整间隔
}
LAYERINFO;

typedef struct {
    std::string strWName; //井名
    unsigned int uWDepth; //井深
    unsigned int uBoardNo; //板号
    std::vector < LAYERINFO > vLayer; //层
}
WELLINFO;

typedef struct {
    std::string strPName;
    std::vector < LAYERINFO > vLayer;
}
PLATFORMINFO;

typedef struct {
    unsigned int BaudRate;
    unsigned short WordLength;
    unsigned short StopBits;
    unsigned short Parity;
    unsigned short Mode;
    unsigned short HwFlowCtl;
    unsigned int Timeout; //超时时间 default 130ms
    unsigned int nRetry; //重试次数
    unsigned int uLoopInr; //轮询间隔 default 20 [5~65535]
}
UART_InitTypeDef;

//通用-Get信息结构体
typedef struct {
    uint8_t ChIdx = 0x00;
    uint8_t No = 0x00;
    uint8_t ID = 0x00;
    uint8_t Fix_1 = 0x00;
    uint8_t Fix_2 = 0x00;
    uint8_t Fix_3 = 0x00;
} REQ_GETINFO;

typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t VH;
    uint8_t VL;
    uint8_t Fix;
    uint8_t Checksum;
} RSP_GETINFO;

//通用-Set信息结构体
typedef struct {
    uint8_t ChIdx = 0x00;
    uint8_t No = 0x00;
    uint8_t ID = 0x00;
    uint8_t VH = 0x00;
    uint8_t VL = 0x00;
    uint8_t Fix = 0x00;
} REQ_SETINFO;

typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t VH;
    uint8_t VL;
    uint8_t Fix;//0x00
    uint8_t Checksum;
} RSP_SETINFO;

//指定编号
typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t HSN;
    uint8_t LSN;
    uint8_t Fix;//0x00
    uint8_t Checksum;
} RSP_SETNO;

//读取压力
typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t Pr1H;
    uint8_t Pr1L;
    uint8_t Pr2H;
    uint8_t Pr2L;
    uint8_t Checksum;
} RSP_GETPRESSURE;

//询问流量设定值--Get通用结构体

//查询工况
typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t Pr1H; //压力1H
    uint8_t Pr1L; //压力1L
    uint8_t Pr2H; //压力2H
    uint8_t Pr2L; //压力2L
    uint8_t FlvH; //流量H
    uint8_t FlvL; //流量L
    uint8_t TmpH; //温度H
    uint8_t TmpL; //温度L
    uint8_t OpDrH;//开度H
    uint8_t OpDrL;//开度L
    uint8_t HV;   //高压
    uint8_t HA;   //高压电流
    uint8_t FLG;  //状态标志位
    uint8_t Checksum;
} RSP_GETSTATUS;
/*状态标志位
Bit0    马达故障
Bit1    位移传感器故障
Bit2    高压偏低
Bit3    温度偏高
Bit4    外压力传感器故障
Bit5    内压力传感器故障
Bit6    流量计故障
Bit7    流量控制过程中
*/


//开度设定--Set通用结构体
//流量设定--Set通用结构体
//电机复位
//读取流量调整间隔--Get通用结构体
//设置流量调整间隔--Get通用结构体
//流量询问--Get通用结构体
//读取高压/电流--Get通用结构体
//进入闭环--Set通用结构体
//退出闭环--Set通用结构体
//询问开度值--Get通用结构体
//上位机高压开/关
typedef struct {
    uint8_t ChIdx;
    uint8_t No;
    uint8_t ID;
    uint8_t UPPERHVONOFF;
    uint8_t Fix_1;
    uint8_t Fix_2;
} REQ_RSP_UPPERHV;

typedef struct {
    uint8_t ChIdx;
    uint8_t No;     //0xFD
    uint8_t VH;
    uint8_t VL;
    uint8_t AH;
    uint8_t AL;
    uint8_t Fix;    //0x12
    uint8_t Checksum;
} RSP_GETINCTRLBOXHVA;


typedef enum {
    CMD_SET_UPPERHVONOFF  = 0x00,   //设置上位机高压开关
    CMD_SET_NO  = 0x01,             //指定编号
    CMD_GET_PRESSURE  = 0x02,       //读取压力
    CMD_GET_FLOWTARGET = 0x04,  //询问流量设定值
    CMD_GET_STATUS = 0x05,          //查询工况
    CMD_SET_OPENDEGREE = 0x06,      //开度设定
    CMD_SET_FLOW = 0x07,            //流量设定
    CMD_RESET_MOTOR = 0x08,         //电机复位
    CMD_GET_FLOWADJINR = 0x09,      //读取流量调整间隔
    CMD_SET_FLOWADJINR = 0x0A,      //设置流量调整间隔
    CMD_GET_FLOW = 0x0B,            //流量询问
    CMD_GET_HVA = 0x0C,             //读取高压/电流
    CMD_SET_CLOOP = 0x0D,           //进入闭环
    CMD_SET_OLOOP = 0x0E,           //退出闭环
    CMD_GET_OPENDEGREE = 0x0F,      //询问开度值
    CMD_GET_INCTRLBOXHVA  = 0xFD,   //上位机高压开/关
    CMDUNKNOWN
} EM_CMDLIST;

void PrintBuffer (void *pBuff, unsigned int nLen);
int reqSetLayerNo (unsigned int uBdIdx, unsigned int uSN, unsigned int uNo, REQ_SETINFO *pstReqSetLayerNo);
bool rspSetLayerNo (char *pch, unsigned int uLen);
int reqGetPress (unsigned int uBdIdx, unsigned int uNo);
bool rspGetPress (char *pch, unsigned int uLen);
int reqGetFlTgt (unsigned int uBdIdx, unsigned int uNo);
bool rspGetFlTgt (char *pch, unsigned int uLen);
int reqGetStatus (unsigned int uBdIdx, unsigned int uNo);
bool rspGetStatus (char *pch, unsigned int uLen);
int reqSetOpenDgr (unsigned int uBdIdx, unsigned int uNo, unsigned int OpDr);
bool rspSetOpenDgr (char *pch, unsigned int uLen);
int reqSetFlv (unsigned int uBdIdx, unsigned int uNo, unsigned int Flv);
bool rspSetFlv (char *pch, unsigned int uLen);
int reqSetMotorRst (unsigned int uBdIdx, unsigned int uNo);
int reqGetFlvAdjInr (unsigned int uBdIdx, unsigned int uNo);
bool rspGetFlvAdjInr (char *pch, unsigned int uLen);
int reqSetFlvInr (unsigned int uBdIdx, unsigned int uNo, unsigned int FlvAdjInr);
bool rspSetFlvInr (char *pch, unsigned int uLen);
int reqGetFlv (unsigned int uBdIdx, unsigned int uNo);
bool rspGetFlv (char *pch, unsigned int uLen);
int reqGetHVA (unsigned int uBdIdx, unsigned int uNo);
bool rspGetHVA (char *pch, unsigned int uLen);
int reqSetLoopOnOff (unsigned int uBdIdx, unsigned int uNo, bool bLoop);
bool rspSetLoopOnOff (char *pch, unsigned int uLen);
int reqGetOpDr (unsigned int uBdIdx, unsigned int uNo);
bool rspGetOpDr (char *pch, unsigned int uLen);
int reqSetUpperHV (unsigned int uBdIdx, bool bStatus);
bool rspSetUpperHV (char *pch, unsigned int uLen);
int reqGetInCtrlBoxHVA (unsigned int uBdIdx);
bool rspGetInCtrlBoxHVA (char *pch, unsigned int uLen);



#endif
