/* ************************************************************************
 *    File Name:     config.h
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月22日 星期三 15时58分01秒
 *  Description:   
 ************************************************************************/
#pragma once

#include "web_enum.h"
#include <json/json.h>
#include <iostream>
#include <stdint.h>
//#include <vector>

int config_load();
int config_save();
int config_reset();
int config_set(CONFIG_ITEM item, int value);
int config_set(CONFIG_ITEM item, std::string value);
int config_get(CONFIG_ITEM item);
std::string config_get_string(CONFIG_ITEM item);
int config_get_item_type(CONFIG_ITEM item);
std::string config_get_item_name(CONFIG_ITEM item);
int config_set_string(CONFIG_ITEM item, std::string value);
//int config_save();

int config_set_json(std::string json_str);
std::string config_get_json();

//TODO
extern Json::Value g_json_root;



