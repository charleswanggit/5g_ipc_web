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
};

static const config_t g_config_list[] =
{
    /*
    {1, ENUM_TYPE_AND_STRING(CONFIG_SERIAL_NUMBER)},
    {115200, ENUM_TYPE_AND_STRING(CONFIG_BAUD_RATE)},

    {0x77, ENUM_TYPE_AND_STRING(CONFIG_PICTURE_SIZE)},
    {0x36, ENUM_TYPE_AND_STRING(CONFIG_PICTURE_COMPRESS)},
    {50, ENUM_TYPE_AND_STRING(CONFIG_PICTURE_BRIGHT)},

    {45, ENUM_TYPE_AND_STRING(CONFIG_GY_DEGREE_1)},
    {0, ENUM_TYPE_AND_STRING(CONFIG_GY_DEGREE_2)},
    {0, ENUM_TYPE_AND_STRING(CONFIG_GY_DIRECTION)},

    {1, ENUM_TYPE_AND_STRING(CONFIG_GY_PICTURE_COUNT)},
    {0, ENUM_TYPE_AND_STRING(CONFIG_RESET_FLAG)},
    {0, ENUM_TYPE_AND_STRING(CONFIG_REBOOT_FLAG)},
    */

    {2, 6, ENUM_TYPE_AND_STRING(CONFIG_BIT_RATE)}, //Mbps
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_RESOLUTION)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_HORIZONTAL_IMAGE)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_VERTICAL_IMAGE)},
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_COLOR_MODE)},
    {0, 3, ENUM_TYPE_AND_STRING(CONFIG_WHITE_LIGHT)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_RECORD)},
    {0, 2, ENUM_TYPE_AND_STRING(CONFIG_NTP)},
};

//static const char g_config_path[] = "/etc/config/sc1a03_config.json";
static const char g_config_path[] = "./config.json";
static Json::Value g_json_root;

static int config_save()
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
        if (root[*iter].type() != Json::intValue) {
            printf("json type error! %s\n", (*iter).c_str());
            continue;
        }
        tmp = g_json_root.get((*iter).c_str(), Json::Value::nullSingleton());
        if (tmp != Json::Value::nullSingleton()) {

            for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
                if (g_config_list[i].item_name == (*iter) &&
                    0 <= root[*iter].asInt() &&
                    root[*iter].asInt() < g_config_list[i].range_value) {
                    printf("%s : %d\n", (*iter).c_str(), root[*iter].asInt());       // 打印键名
                    g_json_root[(*iter).c_str()] = root[*iter].asInt();

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
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        tmp = g_json_root.get(g_config_list[i].item_name, Json::Value(g_config_list[i].default_value));
        g_json_root[g_config_list[i].item_name] = tmp;
        //std::cout << g_config_list[i].item_name << " : " << g_json_root[g_config_list[i].item_name].type() << std::endl;
        //std::cout << g_config_list[i].item_name << " " << g_config_list[i].default_value << std::endl;
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
        g_json_root[g_config_list[i].item_name] = g_config_list[i].default_value;
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

int config_get(CONFIG_ITEM item)
{
    for (int i = 0; i < (int)(sizeof(g_config_list) / sizeof(g_config_list[0])); i++) {
        if (item == g_config_list[i].item) {
            return g_json_root[g_config_list[i].item_name].asInt();
        }
    }
    printf("config get %d failed\n", item);
    return -1;
}

