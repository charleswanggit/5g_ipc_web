/* ************************************************************************
 *    File Name:     web_enum.h
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月28日 星期二 11时51分52秒
 *  Description:   
 ************************************************************************/
#ifndef __WEB_ENUM_H
#define __WEB_ENUM_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 * 
摄像头设备配置内容
码流设置(RtspBitrate)：2Mbps 、4Mbps、8Mbps(默认)、12Mbps、16Mbps、20Mbps[DONE]
分辨率设置(RtspResolution)：1440P，720P，480P[DONE]
水平镜像(SensorHrzMirror)：开(open)、关(close)
垂直镜像(SensorTiltMirror)：开(open)、关(close)
黑白模式(Daynight)：自动(auto)、彩色(color)、黑白(whiteblack)
白光灯(WhiteLed)：自动(auto)、打开(open)、关闭(close)
本地录像(RecordEnable)：开(open)、关(close)。开的话按最高分辨率、最低帧率进行TF卡录像。
时间同步(SyncTimeEnable)：开(open)、关(close)
 */
typedef enum
{
    // web settings
    CONFIG_RTSP_BITRATE,
    CONFIG_RESOLUTION,
    CONFIG_FRAME_RATE,
    CONFIG_SENSOR_HRZ_MIRROR,
    CONFIG_SENSOR_TILT_MIRROR,
    CONFIG_DAYNIGHT,
    CONFIG_WHITE_LED,
    CONFIG_RECORD_ENABLE,
    CONFIG_SYNCTIME_ENABLE,
    CONFIG_NET,
    CONFIG_RTSP_PORT,
    CONFIG_PING_IP,
    CONFIG_IP_ADDRESS,
    CONFIG_NTPSERVER1,
    CONFIG_NTPSERVER2,
    CONFIG_TIMEZONE,
    CONFIG_HEARTBEATIP,
    CONFIG_END,
    // 以上是配置信息，以下是用于事件传递
    CONFIG_UPGRADE,
    // web_server专用枚举
    CONFIG_PRIV_USR = 100,
    CONFIG_PRIV_PASS,
    CONFIG_PRIV_TOKEN,
}CONFIG_ITEM;

typedef enum
{
    STATUS_SIM,
    STATUS_IMEI,
    STATUS_IMSI,
    STATUS_REG,
    STATUS_CHANNEL,
    STATUS_RSRP,
    STATUS_SINR,
    STATUS_IP,
    STATUS_MAC,
    STATUS_GATEWAY,
    STATUS_DNS,
    STATUS_END
}STATUS_ITEM;

#ifdef __cplusplus
}
#endif

#endif /* end of __WEB_ENUM_H */

