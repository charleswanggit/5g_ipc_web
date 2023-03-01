/* ************************************************************************
 *    File Name:     web_server.cpp
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月28日 星期二 09时34分32秒
 *  Description:   
 ************************************************************************/
#include "web_server.h"
#include "mongoose.h"
#include "config.h"
#include "mtc_log.h"

#include <stdio.h>
#include <sys/time.h>
#include <thread>

static web_config_callback g_config_cb = nullptr;
static web_status_callback g_status_cb = nullptr;
static std::thread *g_thread = nullptr;
static int g_sig_stop = 0;
static const char *s_root_dir = "./webs";
static const char *s_listening_address = "http://0.0.0.0:8000";

#define ENUM_TYPE_CASE(x)           case x: return(#x)
static const char* mg_ev_enum2strings(int n)
{
    switch (n) {
        ENUM_TYPE_CASE(MG_EV_ERROR);       // Error                        char *error_message
        ENUM_TYPE_CASE(MG_EV_OPEN);        // Connection created           NULL
        ENUM_TYPE_CASE(MG_EV_POLL);        // mg_mgr_poll iteration        uint64_t *uptime_millis
        ENUM_TYPE_CASE(MG_EV_RESOLVE);     // Host name is resolved        NULL
        ENUM_TYPE_CASE(MG_EV_CONNECT);     // Connection established       NULL
        ENUM_TYPE_CASE(MG_EV_ACCEPT);      // Connection accepted          NULL
        ENUM_TYPE_CASE(MG_EV_TLS_HS);      // TLS handshake succeeded      NULL
        ENUM_TYPE_CASE(MG_EV_READ);        // Data received from socket    long *bytes_read
        ENUM_TYPE_CASE(MG_EV_WRITE);       // Data written to socket       long *bytes_written
        ENUM_TYPE_CASE(MG_EV_CLOSE);       // Connection closed            NULL
        ENUM_TYPE_CASE(MG_EV_HTTP_MSG);    // HTTP request/response        struct mg_http_message *
        ENUM_TYPE_CASE(MG_EV_HTTP_CHUNK);  // HTTP chunk (partial msg)     struct mg_http_message *
        ENUM_TYPE_CASE(MG_EV_WS_OPEN);     // Websocket handshake done     struct mg_http_message *
        ENUM_TYPE_CASE(MG_EV_WS_MSG);      // Websocket msg); text or bin   struct mg_ws_message *
        ENUM_TYPE_CASE(MG_EV_WS_CTL);      // Websocket control msg        struct mg_ws_message *
        ENUM_TYPE_CASE(MG_EV_MQTT_CMD);    // MQTT low-level command       struct mg_mqtt_message *
        ENUM_TYPE_CASE(MG_EV_MQTT_MSG);    // MQTT PUBLISH received        struct mg_mqtt_message *
        ENUM_TYPE_CASE(MG_EV_MQTT_OPEN);   // MQTT CONNACK received        int *connack_status_code
        ENUM_TYPE_CASE(MG_EV_SNTP_TIME);   // SNTP time received           uint64_t *epoch_millis
        ENUM_TYPE_CASE(MG_EV_USER);         // Starting ID for user events

    }
    return "Not recognized ev";
}

static const char* status_enum2strings(int n)
{
    switch (n) {
        ENUM_TYPE_CASE(STATUS_SIM);
        ENUM_TYPE_CASE(STATUS_IMEI);
        ENUM_TYPE_CASE(STATUS_IMSI);
        ENUM_TYPE_CASE(STATUS_REG);
        ENUM_TYPE_CASE(STATUS_CHANNEL);
        ENUM_TYPE_CASE(STATUS_RSRP);
        ENUM_TYPE_CASE(STATUS_SINR);
        ENUM_TYPE_CASE(STATUS_IP);
        ENUM_TYPE_CASE(STATUS_MAC);
        ENUM_TYPE_CASE(STATUS_GATEWAY);
        ENUM_TYPE_CASE(STATUS_DNS);
        ENUM_TYPE_CASE(STATUS_END);
    }
    return "Not recognized status";
}

// 获取程序运行时间
uint64_t utils_get_us()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t now = tv.tv_sec * 1000000LL + tv.tv_usec;
    static uint64_t timeorigin = 0;
    return ( now - timeorigin );
}


// Authenticated user.
// A user can be authenticated by:
//   - a name:pass pair
//   - a token
// When a user is shown a login screen, they enter a user:pass. If successful,
// the server returns the user info, which includes the token. From that point
// on, the client can use the token for authentication. Tokens could be
// refreshed/changed at server side, forcing clients to re-login.
struct user_info {
  const char *name, *pass, *token;
};

// Parse HTTP requests, return authenticated user or NULL
static struct user_info *getuser(struct mg_http_message *hm) {
  // In production, make passwords strong and tokens randomly generated
  // In this example, user list is kept in RAM. In production, it can
  // be backed by file, database, or some other method.
  static struct user_info users[] = {
      {"admin", "pass0", "admin_token"},
      {"user1", "pass1", "user1_token"},
      {"user2", "pass2", "user2_token"},
      {NULL, NULL, NULL},
  };
  char user[256], pass[256];
  struct user_info *u;
  mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
  if (user[0] != '\0' && pass[0] != '\0') {
    // Both user and password are set, search by user/password
    for (u = users; u->name != NULL; u++)
      if (strcmp(user, u->name) == 0 && strcmp(pass, u->pass) == 0) return u;
  } else if (user[0] == '\0') {
    // Only password is set, search by token
    for (u = users; u->name != NULL; u++)
      if (strcmp(pass, u->token) == 0) return u;
  }
  return NULL;
}

static void dir_fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    struct mg_http_serve_opts opts = {.root_dir = s_root_dir};   // Serve local dir
    //struct mg_http_serve_opts opts = {.root_dir = "./HTML_Study"};   // Serve local dir
    if (ev == MG_EV_HTTP_MSG) mg_http_serve_dir(c, (mg_http_message *)ev_data, &opts);
}

int web_apply_config()
{
    MG_INFO(("apply_config"));
    if (g_config_cb == nullptr) {
        return -1;
    }
    int value;
    CONFIG_ITEM cmd;
    std::string value_str;

    for (int i = 0; i < CONFIG_END; i++) {
        cmd = (CONFIG_ITEM)i;
        if (config_get_item_type(cmd) == 0) {
            value = config_get(cmd);
            if (value < 0) {
                continue;
            }
            if (cmd == CONFIG_RTSP_BITRATE) {
                char const *bit_rate_map[] = {"2Mbps", "4Mbps", "8Mbps", "16Mbps", "20Mbps"};
                value_str = bit_rate_map[value];
            }
            else if (cmd == CONFIG_RESOLUTION) {

                char const *resolution_map[] = {"480p", "720p", "1440p"};
                value_str = resolution_map[value];
            }
            else if (cmd == CONFIG_DAYNIGHT) {
                char const *color_map[] = {"auto", "color", "whiteblack"};
                value_str = color_map[value];
            }
            else if (cmd == CONFIG_WHITE_LED) {
                char const *whiteled_map[] = {"auto", "open", "close"};
                value_str = whiteled_map[value];
            }
            else if (cmd == CONFIG_SENSOR_HRZ_MIRROR ||
                     cmd == CONFIG_SENSOR_TILT_MIRROR ||
                     cmd == CONFIG_RECORD_ENABLE ||
                     cmd == CONFIG_SYNCTIME_ENABLE) {
                value_str = value?"close":"open";
            }
        }
        else {
            value_str = config_get_string(cmd);
        }
        g_config_cb(cmd, value_str.c_str());
        //printf("%s: %s\n", config_get_item_name(cmd).c_str(), value_str.c_str());
    }
    return 0;
}

static std::string get_status()
{
    if (g_status_cb == nullptr) {
        return "";
    }
    int ret;
    char value_str[128];
    STATUS_ITEM cmd;
    Json::Value json_root;
    for (int i = 0; i < STATUS_END; i++) {
        cmd = (STATUS_ITEM)i;
        ret = g_status_cb(cmd, value_str, sizeof(value_str));
        if (ret < 0) {
            mtc_print_error("get %s failed!\n", status_enum2strings(i));
        }
        json_root[status_enum2strings(i)] = value_str;
    }
    return json_root.toStyledString();
}

static void web_settings_fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    //struct mg_http_serve_opts opts = {.root_dir = s_root_dir};   // Serve local dir_fn
    if (ev == MG_EV_POLL) {
        return;
    }

    //MG_INFO(("ev: %d, %s\n", ev, mg_ev_enum2strings(ev)));
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        struct user_info *u = getuser(hm);
        //MG_INFO(("http: \n%s\n", hm->message.ptr));
//用户认证
#if 1
        if (u == NULL && mg_http_match_uri(hm, "/api/#")) {
            MG_INFO(("zsppp denied ##############################\n"));
            // All URIs starting with /api/ must be authenticated
            mg_http_reply(c, 403, "", "Denied\n");
            return;
        }
        else if (mg_http_match_uri(hm, "/api/data")) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{%Q:%Q,%Q:%Q}\n", "text", "Hello!", "data", "somedata");
            return;
        }
        else if (mg_http_match_uri(hm, "/api/login")) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{%Q:%Q,%Q:%Q}\n", "user", u->name, "token", u->token);
            return;
        }
#endif

        if (mg_http_match_uri(hm, "/api/device_info")) {
            //MG_INFO(("zsppp test device_info ##############################\n"));
            //static int count = 0;count++;
            //mg_http_reply(c, 200, "Content-Type: application/json\r\n", "TEST device_info %d\n", count);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", get_status().c_str());
        }

        else if (mg_http_match_uri(hm, "/api/settings")) {
            MG_INFO(("zsppp test settings ##############################\n"));
            struct mg_str *host;
            host = mg_http_get_header(hm, "Host");
            if (host) {
                MG_INFO(("zsppp host:%s\n", host->ptr));
                printf("host end\n");
            }
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", config_get_json().c_str());
            printf("\n%s\n", config_get_json().c_str());

        }
        else if (mg_http_match_uri(hm, "/api/submit")) {
            printf("body:\n%s\n", hm->body.ptr);
            int ret = 0;
            ret = config_set_json(hm->body.ptr);
            ret |= web_apply_config();
            if (ret == 0) {
                mg_http_reply(c, 200, "", "set OK!\n");
            }
            else {
                mg_http_reply(c, 403, "", "set error!\n");
            }
        }

        else if (mg_http_match_uri(hm, "/api/f1")) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%Q:%ld}\n",
                          "result", utils_get_us()/1000000);
        }
        else if (mg_http_match_uri(hm, "/api/sum")) {
            // Attempt to fetch a JSON array from the body, hm->body
            struct mg_str json = hm->body;
            double num1, num2;
            MG_INFO(("test /api/sum\n"));
            if (mg_json_get_num(json, "$[0]", &num1) &&
                mg_json_get_num(json, "$[1]", &num2)) {
                // Success! create a JSON response
                mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{%Q:%g}\n",
                              "result", num1 + num2);
                MG_INFO(("sum %lf + %lf = %lf\n", num1, num2, num1+num2));
            }
        }
        else if (mg_http_match_uri(hm, "/api/param")) {
        }

        else {
            //MG_INFO(("zsppp unknow uri\n"));
            struct mg_http_serve_opts opts = {.root_dir = s_root_dir};   // Serve local dir
            mg_http_serve_dir(c, (mg_http_message*)ev_data, &opts);
        }
    }
    else {
    }
}


static void run_loop()
{
    // 读取配置信息
    config_load();

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                        // Init manager
    //mg_http_listen(&mgr, s_listening_address, dir_fn, &mgr);  // Setup listener
    mg_http_listen(&mgr, s_listening_address, web_settings_fn, &mgr);  // Setup listener

    MG_INFO(("Mongoose version : v%s", MG_VERSION));
    MG_INFO(("Listening on     : %s", s_listening_address));
    MG_INFO(("Web root         : [%s]", s_root_dir));

    while (!g_sig_stop) mg_mgr_poll(&mgr, 1000);                         // Event loop
    mg_mgr_free(&mgr);                                        // Cleanup

}

int web_init()
{
    mtc_print_info("web init\n");
    if (g_thread != nullptr) {
        mtc_print_warning("is inited!\n");
        return -1;
    }
    g_thread = new std::thread(&run_loop);
    if (g_thread == nullptr) {
        mtc_print_error("thread create failed!\n");
        return -1;
    }

    return 0;
}

int web_deinit()
{
    mtc_print_info("web deinit\n");
    g_sig_stop = 1;
    if(g_thread)
    {
        //try { g_thread->join(); } catch (...) { }
        g_thread->join();
        delete g_thread;
        g_thread = nullptr;
    }
    return 0;
}

int web_set_config_cb(web_config_callback cb)
{
    g_config_cb = cb;
    return 0;
}

int web_set_status_cb(web_status_callback cb)
{
    g_status_cb = cb;
    return 0;
}
