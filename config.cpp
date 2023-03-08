/* ************************************************************************
 *    File Name:     config.cpp
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月22日 星期三 15时53分24秒
 *  Description:   
 ************************************************************************/
#include "config.h"
#include <json/json.h>
#include <iostream>
#include <fstream>

//#define offsetof(TYPE, MEMBER) ((size_t) (&((TYPE *)0)->MEMBER))
#define ENUM_TYPE_AND_STRING(x) x, #x

struct config_t
{
    int default_value;
    int range_value; // [0, v)
    CONFIG_ITEM item;
    //char item_name[64];
    std::string item_name;
    int type; // int: 0 string: 1
    std::string default_string;
};

static const config_t g_config_list[] =
{
    {2, 6, ENUM_TYPE_AND_STRING(CONFIG_RTSP_BITRATE)}, //Mbps
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_RESOLUTION)},
    {0, 4, ENUM_TYPE_AND_STRING(CONFIG_FRAME_RATE)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_SENSOR_HRZ_MIRROR)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_SENSOR_TILT_MIRROR)},
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_DAYNIGHT)},
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_WHITE_LED)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_RECORD_ENABLE)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_SYNCTIME_ENABLE)},
    {0, 4, ENUM_TYPE_AND_STRING(CONFIG_NET)},
    //{554, 65536, ENUM_TYPE_AND_STRING(CONFIG_RTSP_PORT)},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_RTSP_PORT), 1, "554"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_PING_IP), 1, "0.0.0.0"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_IP_ADDRESS), 1, "0.0.0.0"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_NTPSERVER1), 1, "ntp1.aliyun.com"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_NTPSERVER2), 1, "time1.cloud.tencent.com"},
    //{0, 0, ENUM_TYPE_AND_STRING(CONFIG_TIMEZONE), 1, "Asia/Shanghai"},
    {28, 34, ENUM_TYPE_AND_STRING(CONFIG_TIMEZONE)},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_HEARTBEATIP), 1, "0"},

    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_PRIV_USR), 1, "admin"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_PRIV_PASS), 1, "pass0"},
    {0, 0, ENUM_TYPE_AND_STRING(CONFIG_PRIV_TOKEN), 1, "admin_token"},
};

//static const char g_config_path[] = "/etc/config/sc1a03_config.json";
static const char g_config_path[] = "./config.json";
Json::Value g_json_root;

int config_save()
{
    //TODO 备份配置文件
    std::ofstream outfile;
    outfile.open(g_config_path, std::ofstream::binary);
    if (!outfile.good()) {
        printf("open %s failed!\n", g_config_path);
        return -1;
    }
    outfile << g_json_root.toStyledString();
    outfile.close();
    return 0;
}

std::string config_get_json()
{
    return g_json_root.toStyledString();
    //return g_json_root.toString();
}

int config_set_json(std::string json_str)
{
    Json::Reader reader;
    Json::Value root;
    Json::Value tmp;
    if (!reader.parse(json_str, root)) {
        printf("json parse failed!\n");
        return -1;
    }
    if (root.type() != Json::objectValue) {
        printf("json type error!\n");
        return -1;
    }

    Json::Value::Members mem = root.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++) {
        tmp = g_json_root.get((*iter).c_str(), Json::Value::nullSingleton());
        if (tmp != Json::Value::nullSingleton()) {

            for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
                if (g_config_list[i].item_name == (*iter) &&
                    root[*iter].type() == Json::intValue &&
                    0 <= root[*iter].asInt() &&
                    root[*iter].asInt() < g_config_list[i].range_value) {
                    printf("%s : %d\n", (*iter).c_str(), root[*iter].asInt());       // 打印键名
                    //g_json_root[(*iter).c_str()] = root[*iter].asInt();
                    g_json_root[(*iter).c_str()] = root[*iter];

                }
                else if (g_config_list[i].item_name == (*iter) &&
                    root[*iter].type() == Json::stringValue) {
                    g_json_root[(*iter).c_str()] = root[*iter];
                }

            }
        }
        else {
            printf("json key error! %s\n", (*iter).c_str());
        }
    }
    config_save();
    return 0;
}

int config_load()
{
    printf("load config\n");
    Json::Reader reader;

    std::ifstream infile;
    infile.open(g_config_path, std::ifstream::binary);
    if (infile.good()) {
        reader.parse(infile, g_json_root, 0);
    }
    else {
        printf("open %s failed!\n", g_config_path);
    }

    Json::Value tmp;
    Json::Value default_value;
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (g_config_list[i].type == 0) {
            default_value = Json::Value(g_config_list[i].default_value);
            tmp = g_json_root.get(g_config_list[i].item_name, default_value);
            if (tmp.type() != Json::intValue) {
                tmp = default_value;
            }
        }
        else {
            tmp = g_json_root.get(g_config_list[i].item_name, Json::Value(g_config_list[i].default_string));
            if (tmp.type() != Json::stringValue) {
                tmp = Json::Value(g_config_list[i].default_string);
            }
        }
        g_json_root[g_config_list[i].item_name] = tmp;
    }

    //Json::Value xx = g_json_root.get("serial", Json::Value(114514));
    //g_json_root["serial"] = xx;
    std::cout << g_json_root.toStyledString();
    infile.close();
    return 0;
}

int config_reset()
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (g_config_list[i].type == 0) {
            g_json_root[g_config_list[i].item_name] = g_config_list[i].default_value;
        }
        else {
            g_json_root[g_config_list[i].item_name] = g_config_list[i].default_string;
        }
    }
    g_json_root["picture_list"].clear();
    return config_save();
}

int config_set(CONFIG_ITEM item, int value)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item) {
            g_json_root[g_config_list[i].item_name] = value;
            break;
        }
    }
    return config_save();
}

int config_set(CONFIG_ITEM item, std::string value)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item) {
            g_json_root[g_config_list[i].item_name] = value;
            break;
        }
    }
    return config_save();
}

int config_get(CONFIG_ITEM item)
{
#if 0
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item && g_config_list[i].type == 0) {
            return g_json_root[g_config_list[i].item_name].asInt();
        }
    }
#endif
    if (g_json_root[g_config_list[item].item_name].type() == Json::intValue) {
        return g_json_root[g_config_list[item].item_name].asInt();
    }
    printf("config get %d: %s failed\n", item, g_config_list[item].item_name);
    return -1;
}

int config_get_item_type(CONFIG_ITEM item)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item) {
            return g_config_list[item].type;
        }
    }
    return -1;
}
std::string config_get_item_name(CONFIG_ITEM item)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item) {
            return g_config_list[item].item_name;
        }
    }
    return "";
}


std::string config_get_string(CONFIG_ITEM item)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item && g_config_list[i].type == 1) {
            return g_json_root[g_config_list[i].item_name].asString();
        }
    }
    printf("config get string %d: %s failed\n", item, g_config_list[item].item_name);
    return "";
}

