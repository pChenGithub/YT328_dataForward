#include "dataforward.h"
#include "global.h"
#include "task/card/interfacecardopt.h"
#include "task/qrcode/interfaceqropt.h"
#include "task/face/taskface.h"
#include "signalsender.h"

static char s_cardid[16] = {0};

DataForward::DataForward(QObject *parent) : QObject(parent)
{

}

int DataForward::SendInfo(INFO_TYPE type, void *info)
{
    LOG_I("处理一条卡号\n");
#if (DEVICE_USE_TYPE==READER)
    return SendToControler(type, info);
#elif ( (DEVICE_USE_TYPE==DCTR_40) || (DEVICE_USE_TYPE==DCTR_YT))
    return SendToAuth(type, info);
#endif
}

int DataForward::SendToControler(INFO_TYPE type, void *info)
{
    rs485_req* req = (rs485_req*)malloc(sizeof(rs485_req));
    const FORMAT_CARD_INFO* cardInfo = (const FORMAT_CARD_INFO*)info;
    const FORMAT_QR_INFO* qrInfo = (const FORMAT_QR_INFO*)info;
    const FACE_DETECED* face = (const FACE_DETECED*)info;
    switch (type) {
    case INFO_CARDSID:
        req->req_type = REQ_PUSH_CARDID;
        // 填充卡号
        req->arg.cardid_info.readtype = 1;
        memcpy(req->arg.cardid_info.cardid, (char*)info, 4);
        break;
    case INFO_CARDINFO:
        req->req_type = REQ_PUSH_CARD_INFO;
        // 帐号
        memcpy(req->arg.c_info.account, cardInfo->ucCardAccount, 4);
        // 卡内编号
        memcpy(req->arg.c_info.num, cardInfo->ucCardNumber, 3);
        // 卡号
        memcpy(req->arg.c_info.cardid, cardInfo->cCardTempSID, 4);
        // 有效时间
        memcpy(req->arg.c_info.validity, cardInfo->ucValidTime, 2);
        // 卡号状态
        req->arg.c_info.card_state = cardInfo->cCardState;
        // 账户姓名
        memcpy(req->arg.c_info.name, cardInfo->cUserName, 10);
        break;
    case INFO_QR:
        req->req_type = REQ_PUSH_SCAN_CODE_INFO;
        memcpy(req->arg.scan_code.account, qrInfo->account, 4);
        memcpy(req->arg.scan_code.card_id, qrInfo->card_id, 4);
        memcpy(req->arg.scan_code.name, qrInfo->name, 10);
        memcpy(req->arg.scan_code.order_num, qrInfo->order_num, 4);
        break;
    case INFO_FACE:
        req->req_type = REQ_PUSH_FACE_INFO;
        // name card_id account长度16
        // 填充名字
        memcpy(req->arg.f_info.name, face->name, 15);
        LOG_D("人脸名称 %s", req->arg.f_info.name);
        // 填充卡号
        memcpy(req->arg.f_info.card_id, face->cardno, 15);
        // 填充帐号
        memcpy(req->arg.f_info.account, face->accnum, 15);
        break;
    default:
        break;
    }
#if 0
    LOG_I("刷卡任务提交到485发出");
    rs485_svc_CommitReq(req);
#endif
    return 0;
}

int DataForward::SendToAuth(INFO_TYPE type, void *info)
{
    const FORMAT_CARD_INFO* cardInfo = (const FORMAT_CARD_INFO*)info;
    const FORMAT_QR_INFO* qrInfo = (const FORMAT_QR_INFO*)info;
    const FACE_DETECED* face = (const FACE_DETECED*)info;
    SignalSender* sender = SignalSender::getInstance();

    LOG_I("校验权限来源类型 %d\n", type);
    switch (type) {
    case INFO_CARDSID:
        snprintf(s_cardid, 16, "%u", *(unsigned int*)cardInfo->cCardTempSID);
        LOG_I("校验权限卡号 %s\n", s_cardid);
        sender->sendSignalSendAcc(s_cardid);
        break;
    case INFO_CARDINFO:
        break;
    case INFO_QR:
        snprintf(s_cardid, 16, "%u", *(unsigned int*)(qrInfo->card_id));
        LOG_I("校验权限卡号 %s\n", s_cardid);
        sender->sendSignalSendAcc(s_cardid);
        break;
    case INFO_FACE:
        LOG_I("校验权限卡号 %s\n", face->cardno);
        sender->sendSignalSendAcc(face->cardno);
        break;
    default:
        break;
    }
    return 0;
}

