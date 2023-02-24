/* ************************************************************************
 *    File Name:     config.h
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月22日 星期三 15时58分01秒
 *  Description:   
 ************************************************************************/
#pragma once

#include <json/json.h>
#include <iostream>
#include <stdint.h>
//#include <vector>

#define PIC_LIST_MAX_SIZE 5

typedef enum
{
    // web settings
    CONFIG_BIT_RATE,
    CONFIG_RESOLUTION,
    CONFIG_HORIZONTAL_IMAGE,
    CONFIG_VERTICAL_IMAGE,
    CONFIG_COLOR_MODE,
    CONFIG_WHITE_LIGHT,
    CONFIG_RECORD,
    CONFIG_NTP,
    CONFIG_RTSP_PORT,
    CONFIG_LOCAL_IP,
}CONFIG_ITEM;

int config_load();
int config_reset();
int config_set(CONFIG_ITEM item, int value);
int config_get(CONFIG_ITEM item);
//int config_save();

int config_set_json(std::string json_str);
std::string config_get_json();

//extern Json::Value g_json_root;



