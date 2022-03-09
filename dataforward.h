#ifndef DATAFORWARD_H
#define DATAFORWARD_H
/**
 * 卡码脸消息转发接口，将根据系统的使用方式，决定是把获取的卡码脸数据发送给控制器还是鉴权模块。
 */

#include <QObject>

typedef enum {
    INFO_CARDSID = 0,   // 发送卡号模式
    INFO_CARDINFO,      // 发送帐号模式
    INFO_QR,            // 发送扫码信息
    INFO_FACE,          // 发送人脸信息
} INFO_TYPE;

typedef struct {

} DATA_INFO;

class DataForward : public QObject
{
    Q_OBJECT
public:
    explicit DataForward(QObject *parent = 0);
    // 提供数据发送接口，数据分为卡、码、脸3类
    static int SendInfo(INFO_TYPE type, void *info);
    static int SendToControler(INFO_TYPE type, void *info);
    static int SendToAuth(INFO_TYPE type, void *info);

signals:

public slots:
};

#endif // DATAFORWARD_H
