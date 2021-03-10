#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <QByteArray>

#include "protocol.h"

EM_CMDLIST gCurrCmd = CMDUNKNOWN;

void PrintBuffer (void *pBuff, unsigned int nLen)
{
    if (NULL == pBuff || 0 == nLen) {
        return;
    }

    const int nBytePerLine = 16;
    unsigned char *p = (unsigned char *)pBuff;
    char szHex[3 * nBytePerLine + 1] = {0};
    printf ("-----------------begin-------------------\n");

    for (unsigned int i = 0; i < nLen; ++i) {
        int idx = 3 * (i % nBytePerLine);

        if (0 == idx) {
            memset (szHex, 0, sizeof (szHex));
        }

#ifdef WIN32
        sprintf_s (&szHex[idx], 4, "%02x ", p[i]); // buff长度要多传入1个字节
#else
        snprintf (&szHex[idx], 4, "%02x ", p[i]); // buff长度要多传入1个字节
#endif

        // 以16个字节为一行，进行打印
        if (0 == ((i + 1) % nBytePerLine)) {
            printf ("%s\n", szHex);
        }
    }

    // 打印最后一行未满16个字节的内容
    if (0 != (nLen % nBytePerLine)) {
        printf ("%s\n", szHex);
    }

    printf ("------------------end-------------------\n");
}
/**
 * @brief
 *
 * @param pdata
 * @param len
 * @return true
 * @return false
 */
bool isAdd8Vaild (unsigned char *pdata, unsigned int len)
{
    uint8_t Add8 = 0;
    len--;

    while (len--) {
        Add8 += *pdata++;
    }

    if (Add8 != *pdata++) {
        return false;
    }

    return true;
}

/**
 * @brief 设定编号
 * ╔═══╦══════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ 0xFE │ 0x01 │ SnH  │ SnL   │ No. ║
 * ╟───╫───────┼──────┼──────┼──────┼───────┼─────╢
 * ║ A ║ ChIdx │ No.* │ SnH* │ SnL* │ 0x00* │ Add8║
 * ╚═══╩═══════╧══════╧══════╧══════╧═══════╧═════╝
 *
 * @param uBdIdx 通道号通道号
 * @param uSN 序列号
 * @param uNo 编号
 * @return int
 */
int reqSetLayerNo (unsigned int uBdIdx, unsigned int uSN, unsigned int uNo, REQ_SETINFO *pstReqSetLayerNo)
{
    //build struct
    REQ_SETINFO stReqSetLayerNo;
    pstReqSetLayerNo->ChIdx    = uBdIdx;
    pstReqSetLayerNo->No       = 0xFE;
    pstReqSetLayerNo->ID       = CMD_SET_NO;
    pstReqSetLayerNo->VH       = ((uSN >> 8) & 0xFF);
    pstReqSetLayerNo->VL       = (uSN & 0xFF);
    pstReqSetLayerNo->Fix      = uNo;
    gCurrCmd = CMD_SET_NO;
    //send
    //setTimer
    PrintBuffer (&stReqSetLayerNo, sizeof (stReqSetLayerNo));
    return 0;
}

bool rspSetLayerNo (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETNO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据超长
        return false;
    }

    RSP_SETNO stRspSetLayerNo;
    memcpy (&stRspSetLayerNo, pch, uLen);

    if (stRspSetLayerNo.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    PrintBuffer (&stRspSetLayerNo, uLen);
    return true;
}


/**
 * @brief 读取压力
 * ╔═══╦════════════════════════════════════════════════════════╗
 * ║ A ║ ChIdx │ No.  │ 0x02    │ 0x00     │ 0x00    │ 0x00     ║
 * ╟───╫───────┼──────┼─────────┼──────────┼─────────┼──────────╬═════╗
 * ║ B ║ ChIdx │ No.* │ Press1* │ Press1L* │ Press2* │ Press2L* │ Add8║
 * ╚═══╩═══════╧══════╧═════════╧══════════╧═════════╧══════════╧═════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetPress (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetPress;
    stReqGetPress.ChIdx = uBdIdx;
    stReqGetPress.No    = uNo;
    stReqGetPress.ID    = CMD_GET_PRESSURE;
    gCurrCmd = CMD_GET_PRESSURE;
    //send
    //setTimer
    PrintBuffer (&stReqGetPress, sizeof (stReqGetPress));
    return 0;
}
bool rspGetPress (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETPRESSURE)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据超长
        return false;
    }

    RSP_GETPRESSURE stRspGetPress;
    memcpy (&stRspGetPress, pch, uLen);
    unsigned int ulPress1   = ((stRspGetPress.Pr1H << 8) & stRspGetPress.Pr1L) / 100;
    unsigned int ulPress2   = ((stRspGetPress.Pr2H << 8) & stRspGetPress.Pr2L) / 100;
    PrintBuffer (&stRspGetPress, uLen);
    return true;
}


/**
 * @brief 读取流量设定值
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ A ║ ChIdx │ No.  │ 0x04  │ 0x00  │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ B ║ ChIdx │ No.* │ FlvH* │ FlvL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetFlTgt (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetFlTgt;
    stReqGetFlTgt.ChIdx    = uBdIdx;
    stReqGetFlTgt.No       = uNo;
    stReqGetFlTgt.ID       = CMD_GET_FLOWTARGET;
    gCurrCmd = CMD_GET_FLOWTARGET;
    //send
    //setTimer
    PrintBuffer (&stReqGetFlTgt, sizeof (stReqGetFlTgt));
    return 0;
}
bool rspGetFlTgt (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据超长
        return false;
    }

    RSP_SETNO stRspGetFlTgt;
    memcpy (&stRspGetFlTgt, pch, uLen);
    PrintBuffer (&stRspGetFlTgt, uLen);

    if (stRspGetFlTgt.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulTgtFl = ((stRspGetFlTgt.HSN << 8) & stRspGetFlTgt.LSN) / 100;
    return true;
}


/**
 * @brief 读取工况
 * ╔═══╦═══════════════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No. │ 0x05    │ 0x00     │ 0x00    │ 0x00     ║
 * ╟───╫───────┼─────┼─────────┼──────────┼─────────┼──────────╬═══════╤══════╤═══════╤═══════╤════════╤════════╤════╤════╤═══════╤═════╗
 * ║ A ║ ChIdx │ No. │ Press1* │ Press1L* │ Press2* │ Press2L* │ FlvH* │ Flv* │ TmpH* │ TmpL* │ OpDrH* │ OpDrL* │ V* │ A* │ Stat* │ Add8║
 * ╚═══╩═══════╧═════╧═════════╧══════════╧═════════╧══════════╧═══════╧══════╧═══════╧═══════╧════════╧════════╧════╧════╧═══════╧═════╝
 * ╔═════╤════════╤══════════════════╗
 * ║ Bit │   0    │         1        ║
 * ║  0  │ 无故障 │       马达故障   ║
 * ║  1  │ 无故障 │ 位移传感器故障   ║
 * ║  2  │ 无故障 │     高压偏低     ║
 * ║  3  │ 无故障 │     温度偏高     ║
 * ║  4  │ 无故障 │ 外压力传感器故障 ║
 * ║  5  │ 无故障 │ 内压力传感器故障 ║
 * ║  6  │ 无故障 │    流量计故障    ║
 * ║  7  │开环状态│  流量控制过程中  ║
 * ╚═════╧════════╧══════════════════╝
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetStatus (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetStatus;
    stReqGetStatus.ChIdx    = uBdIdx;
    stReqGetStatus.No       = uNo;
    stReqGetStatus.ID       = CMD_GET_STATUS;
    gCurrCmd = CMD_GET_STATUS;
    //send
    //setTimer
    PrintBuffer (&stReqGetStatus, sizeof (stReqGetStatus));
    return 0;
}
bool rspGetStatus (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETSTATUS)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETSTATUS stRspGetStatus;
    memcpy (&stRspGetStatus, pch, uLen);
    unsigned int ulPress1   = ((stRspGetStatus.Pr1H << 8) & stRspGetStatus.Pr1L) / 100;
    unsigned int ulPress2   = ((stRspGetStatus.Pr2H << 8) & stRspGetStatus.Pr2L) / 100;
    unsigned int ulFl       = ((stRspGetStatus.FlvH << 8) & stRspGetStatus.FlvL) / 100;
    unsigned int ulTmp      = ((stRspGetStatus.TmpH << 8) & stRspGetStatus.TmpL) / 100;
    unsigned int ulOpDgr    = ((stRspGetStatus.OpDrH << 8) & stRspGetStatus.OpDrL) / 100;
    float fHV               = stRspGetStatus.HV * 150 / 25;
    unsigned int ulHA       = stRspGetStatus.HA / 255;
    uint8_t uFlg            = stRspGetStatus.FLG;
    PrintBuffer (&stRspGetStatus, uLen);
    return true;
}


/**
 * @brief 设置开度
 * ╔═══╦═══════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x06   │ OpDrH  │ OpDrL │ 0x00 ║
 * ╟───╫───────┼──────┼────────┼────────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ OpDrH* │ OpDrL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧════════╧════════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @param OpDr 开度
 * @return int
 */
int reqSetOpenDgr (unsigned int uBdIdx, unsigned int uNo, unsigned int OpDr)
{
    //build struct
    REQ_SETINFO stReqSetOpDr;
    stReqSetOpDr.ChIdx  = uBdIdx;
    stReqSetOpDr.No     = uNo;
    stReqSetOpDr.ID     = CMD_SET_OPENDEGREE;
    stReqSetOpDr.VH     = ((OpDr >> 8) & 0xFF);
    stReqSetOpDr.VL     = (OpDr & 0xFF);
    gCurrCmd = CMD_SET_OPENDEGREE;
    //send
    //setTimer
    PrintBuffer (&stReqSetOpDr, sizeof (stReqSetOpDr));
    return 0;
}
bool rspSetOpenDgr (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_SETINFO stRspSetOpenDgr;
    memcpy (&stRspSetOpenDgr, pch, uLen);

    if (stRspSetOpenDgr.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulOpenDgr = ((stRspSetOpenDgr.VH << 8) & stRspSetOpenDgr.VL) / 100;
    PrintBuffer (&stRspSetOpenDgr, uLen);
    return true;
}


/**
 * @brief 设置流量
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x07  │ FlvH  │ FlvL  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ FlvH* │ FlvL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @param Flv 流量
 * @return int
 */
int reqSetFlv (unsigned int uBdIdx, unsigned int uNo, unsigned int Flv)
{
    //build struct
    REQ_SETINFO stReqSetFlv;
    stReqSetFlv.ChIdx   = uBdIdx;
    stReqSetFlv.No      = uNo;
    stReqSetFlv.ID      = CMD_SET_FLOW;
    stReqSetFlv.VH      = ((Flv >> 8) & 0xFF);
    stReqSetFlv.VL      = (Flv & 0xFF);
    gCurrCmd = CMD_SET_FLOW;
    //send
    //setTimer
    PrintBuffer (&stReqSetFlv, sizeof (stReqSetFlv));
    return 0;
}
bool rspSetFlv (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_SETINFO stRspSetFlv;
    memcpy (&stRspSetFlv, pch, uLen);

    if (stRspSetFlv.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulFlv = ((stRspSetFlv.VH << 8) & stRspSetFlv.VL) / 100;
    PrintBuffer (&stRspSetFlv, uLen);
    return true;
}

/**
 * @brief 设置电机复位
 * ╔═══╦═════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No. │ 0x08 │ 0x00 │ 0x00 │ 0x00 ║
 * ╚═══╩═══════╧═════╧══════╧══════╧══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqSetMotorRst (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_SETINFO stReqSetMotorRst;
    stReqSetMotorRst.ChIdx  = uBdIdx;
    stReqSetMotorRst.No     = uNo;
    stReqSetMotorRst.ID     = CMD_RESET_MOTOR;
    gCurrCmd = CMD_RESET_MOTOR;
    //send
    //setTimer
    PrintBuffer (&stReqSetMotorRst, sizeof (stReqSetMotorRst));
    return 0;
}


/**
 * @brief 读取Flv调整间隔
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x09  │ 0x00  │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ InrH* │ InrL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetFlvAdjInr (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetFlvAdjInr;
    stReqGetFlvAdjInr.ChIdx = uBdIdx;
    stReqGetFlvAdjInr.No    = uNo;
    stReqGetFlvAdjInr.ID    = CMD_GET_FLOWADJINR;
    gCurrCmd = CMD_GET_FLOWADJINR;
    //send
    //setTimer
    PrintBuffer (&stReqGetFlvAdjInr, sizeof (stReqGetFlvAdjInr));
    return 0;
}
bool rspGetFlvAdjInr (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETINFO stRspGetFlvAdjInr;
    memcpy (&stRspGetFlvAdjInr, pch, uLen);

    if (stRspGetFlvAdjInr.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulFlvAdjInr = ((stRspGetFlvAdjInr.VH << 8) & stRspGetFlvAdjInr.VL) / 100;
    PrintBuffer (&stRspGetFlvAdjInr, uLen);
    return true;
}

/**
 * @brief 设置Flv调整间隔
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x0A  │ InrH  │ InrL  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ InrH* │ InrL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @param FlvAdjInr 流量调整间隔
 * @return int
 */
int reqSetFlvInr (unsigned int uBdIdx, unsigned int uNo, unsigned int FlvAdjInr)
{
    //build struct
    REQ_SETINFO stReqSetFlvAdjInr;
    stReqSetFlvAdjInr.ChIdx = uBdIdx;
    stReqSetFlvAdjInr.No    = uNo;
    stReqSetFlvAdjInr.ID    = CMD_SET_FLOWADJINR;
    stReqSetFlvAdjInr.VH    = ((FlvAdjInr >> 8) & 0xFF);
    stReqSetFlvAdjInr.VL    = (FlvAdjInr & 0xFF);
    gCurrCmd = CMD_SET_FLOWADJINR;
    //send
    //setTimer
    PrintBuffer (&stReqSetFlvAdjInr, sizeof (stReqSetFlvAdjInr));
    return 0;
}
bool rspSetFlvInr (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_SETINFO stRspSetFlvInr;
    memcpy (&stRspSetFlvInr, pch, uLen);

    if (stRspSetFlvInr.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulFlvInr = ((stRspSetFlvInr.VH << 8) & stRspSetFlvInr.VL) / 100;
    PrintBuffer (&stRspSetFlvInr, uLen);
    return true;
}


/**
 * @brief 读取Flv
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x0B  │ 0x00  │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ FlvH* │ FlvL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetFlv (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetFlv;
    stReqGetFlv.ChIdx   = uBdIdx;
    stReqGetFlv.No      = uNo;
    stReqGetFlv.ID      = CMD_GET_FLOWADJINR;
    gCurrCmd = CMD_GET_FLOWADJINR;
    //send
    //setTimer
    PrintBuffer (&stReqGetFlv, sizeof (stReqGetFlv));
    return 0;
}
bool rspGetFlv (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETINFO stRspGetFlv;
    memcpy (&stRspGetFlv, pch, uLen);

    if (stRspGetFlv.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulFlowV = ((stRspGetFlv.VH << 8) & stRspGetFlv.VL) / 100;
    PrintBuffer (&stRspGetFlv, uLen);
    return true;
}


/**
 * @brief 读取高压、电流
 * ╔═══╦═══════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x0C │ 0x00 │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼──────┼──────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ V*   │ A*   │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧══════╧══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetHVA (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetHVA;
    stReqGetHVA.ChIdx   = uBdIdx;
    stReqGetHVA.No      = uNo;
    stReqGetHVA.ID      = CMD_GET_HVA;
    gCurrCmd = CMD_GET_HVA;
    //send
    //setTimer
    PrintBuffer (&stReqGetHVA, sizeof (stReqGetHVA));
    return 0;
}
bool rspGetHVA (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETINFO stRspGetHVA;
    memcpy (&stRspGetHVA, pch, uLen);

    if (stRspGetHVA.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    float ulHV = stRspGetHVA.VH * 150 / 250;
    float ulHA = stRspGetHVA.VL * 150 / 250;
    PrintBuffer (&stRspGetHVA, uLen);
    return true;
}

/**
 * @brief 进入0x0D/退出0x0E闭环模式
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x0D/E│ 0x00  │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ 0x00* │ 0x00* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @param bLoop 是否闭环
 * @return int
 */
int reqSetLoopOnOff (unsigned int uBdIdx, unsigned int uNo, bool bLoop)
{
    //build struct
    REQ_SETINFO stReqSetCLoop;
    stReqSetCLoop.ChIdx = uBdIdx;
    stReqSetCLoop.No    = uNo;
    stReqSetCLoop.ID    = bLoop ? CMD_SET_OLOOP : CMD_SET_CLOOP;
    gCurrCmd = bLoop ? CMD_SET_OLOOP : CMD_SET_CLOOP;
    //send
    //setTimer
    PrintBuffer (&stReqSetCLoop, sizeof (stReqSetCLoop));
    return 0;
}
bool rspSetLoopOnOff (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_SETINFO stRspSetLoopOnOff;
    memcpy (&stRspSetLoopOnOff, pch, uLen);

    if ((stRspSetLoopOnOff.VH != 0x00) || (stRspSetLoopOnOff.VL != 0x00) || (stRspSetLoopOnOff.Fix != 0x00)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    PrintBuffer (&stRspSetLoopOnOff, uLen);
    return true;
}


/**
 * @brief 读取开度值
 * ╔═══╦═══════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ No.  │ 0x0F   │ 0x00   │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼────────┼────────┼───────┼──────╢
 * ║ A ║ ChIdx │ No.* │ OpDrH* │ OpDrL* │ 0x00* │ Add8 ║
 * ╚═══╩═══════╧══════╧════════╧════════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param uNo 编号
 * @return int
 */
int reqGetOpDr (unsigned int uBdIdx, unsigned int uNo)
{
    //build struct
    REQ_GETINFO stReqGetOpDr;
    stReqGetOpDr.ChIdx  = uBdIdx;
    stReqGetOpDr.No  = uNo;
    stReqGetOpDr.ID     = CMD_GET_OPENDEGREE;
    gCurrCmd = CMD_GET_OPENDEGREE;
    //send
    //setTimer
    PrintBuffer (&stReqGetOpDr, sizeof (stReqGetOpDr));
    return 0;
}
bool rspGetOpDr (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETINFO stRspGetOpDr;
    memcpy (&stRspGetOpDr, pch, uLen);

    if (stRspGetOpDr.Fix != 0x00) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    unsigned int ulOpDr = ((stRspGetOpDr.VH << 8) & stRspGetOpDr.VL) / 100;
    PrintBuffer (&stRspGetOpDr, uLen);
    return true;
}


/**
 * @brief 上位机高压 开0/关1
 * ╔═══╦═════════════════════════════════════════════╗
 * ║ Q ║ ChIdx │  0   │ 0x09  │0x00/1 │ 0x00  │ 0x00 ║
 * ╟───╫───────┼──────┼───────┼───────┼───────┼──────╢
 * ║ A ║ ChIdx │  0   │ 0x09  │0x00/1*│ 0x00* │ 0x00 ║
 * ╚═══╩═══════╧══════╧═══════╧═══════╧═══════╧══════╝
 *
 * @param uBdIdx 通道号
 * @param bStatus 开true/关false
 * @return int
 */
int reqSetUpperHV (unsigned int uBdIdx, bool bStatus)
{
    //build struct
    REQ_SETINFO stReqSetUpperHV;
    stReqSetUpperHV.ChIdx   = uBdIdx;
    stReqSetUpperHV.ID      = 0x09;
    stReqSetUpperHV.VH      = bStatus ? 0 : 1;
    gCurrCmd = CMD_SET_UPPERHVONOFF;
    //send
    //setTimer
    PrintBuffer (&stReqSetUpperHV, sizeof (stReqSetUpperHV));
    return 0;
}
bool rspSetUpperHV (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_SETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_SETINFO stRspSetUpperHV;
    memcpy (&stRspSetUpperHV, pch, uLen);

    if ((stRspSetUpperHV.No != 0x00) || (stRspSetUpperHV.VH != 0x09) || ((stRspSetUpperHV.VL != 0x00) && (stRspSetUpperHV.VL != 0x01)) || (stRspSetUpperHV.Fix != 0x00)
            || (stRspSetUpperHV.Checksum != 0x00)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    PrintBuffer (&stRspSetUpperHV, uLen);
    return true;
}


/**
 * @brief 读取进口控制箱高压和电流(10s)
 * ╔═══╦═══════════════════════════════════════════╗
 * ║ Q ║ ChIdx │ 0XFD  │ 0x09 │ 0x00 │ 0x00 │ 0x00 ║
 * ╟───╫───────┼───────┼──────┼──────┼──────┼──────╬═══════╤═════╗
 * ║ A ║ ChIdx │ 0XFD* │ VH*  │ VL*  │ AH*  │ AL*  │ 0X12* │ Add8║
 * ╚═══╩═══════╧═══════╧══════╧══════╧══════╧══════╧═══════╧═════╝
 *
 * @param uBdIdx 通道号
 * @return int
 */
int reqGetInCtrlBoxHVA (unsigned int uBdIdx)
{
    //build struct
    REQ_GETINFO stReqGetOpDr;
    stReqGetOpDr.ChIdx  = uBdIdx;
    stReqGetOpDr.No     = CMD_GET_INCTRLBOXHVA;
    stReqGetOpDr.ID     = 0x09;
    gCurrCmd = CMD_GET_INCTRLBOXHVA;
    //send
    //setTimer
    PrintBuffer (&stReqGetOpDr, sizeof (stReqGetOpDr));
    return 0;
}
bool rspGetInCtrlBoxHVA (char *pch, unsigned int uLen)
{
    if (uLen != sizeof (RSP_GETINFO)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据长度异常
        return false;
    }

    RSP_GETINCTRLBOXHVA stRspGetInCtrlBoxHVA;
    memcpy (&stRspGetInCtrlBoxHVA, pch, uLen);

    if ((stRspGetInCtrlBoxHVA.No != 0xFD) || (stRspGetInCtrlBoxHVA.Fix != 0x12)) {
        fprintf (stderr, "[%04d:%s]\r\n", __LINE__, __FUNCTION__); //数据异常
        return false;
    }

    PrintBuffer (&stRspGetInCtrlBoxHVA, uLen);
    return true;
}

bool isCmdVaild (QByteArray buf)
{
    char cChIdx = buf[0];
    char cNo = buf[1];

    if (cChIdx == 0) {
        return false;//通道号!=0
    }

    if (gCurrCmd == CMDUNKNOWN) {
        return false;//请求命令超时/异常数据接收
    }

    if ((gCurrCmd != CMD_SET_UPPERHVONOFF) && (cNo == 0)) {
        return false;//编号!=0
    } else if ((gCurrCmd == CMD_SET_UPPERHVONOFF) && (cNo != 0)) {
        return false;//上位机高压开/关 编号==0
    } else {
        return true;
    }
}

int cmdProcess (QByteArray buf)
{
    if (!isCmdVaild (buf)) {
        return -1;//数据校验失败
    }

    bool bRet = false;
    char cChIdx = buf[0];
    char cNo = buf[1];
    unsigned int len = buf.size();

    switch (gCurrCmd) {
        case CMD_SET_NO :
            bRet = rspSetLayerNo (buf.data(), len);
            break;

        case CMD_GET_PRESSURE :
            break;

        case CMD_GET_FLOWTARGET :
            break;

        case CMD_GET_STATUS :
            break;

        case CMD_SET_OPENDEGREE :
            break;

        case CMD_SET_FLOW :
            break;

        case CMD_RESET_MOTOR :
            break;

        case CMD_GET_FLOWADJINR :
            break;

        case CMD_SET_FLOWADJINR :
            break;

        case CMD_GET_FLOW :
            break;

        case CMD_GET_HVA :
            break;

        case CMD_SET_CLOOP :
            break;

        case CMD_SET_OLOOP :
            break;

        case CMD_GET_OPENDEGREE :
            break;

        case CMD_SET_UPPERHVONOFF :
            break;

        case CMDUNKNOWN :
            return 0;
            break;

        default:
            return 0;
            break;
    }
}
