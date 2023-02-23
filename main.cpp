/* ************************************************************************
 *    File Name:     main.cpp
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月15日 星期三 10时47分13秒
 *  Description:   
 ************************************************************************/
#include "mongoose.h"
#include "config.h"

#include <stdio.h>
#include <sys/time.h>

#define ENUM_TYPE_CASE(x)           case x: return(#x)
static const char* mg_ev_enum2strings(int ev)
{
    switch (ev) {
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

// 获取程序运行时间
uint64_t utils_get_us()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t now = tv.tv_sec * 1000000LL + tv.tv_usec;
    static uint64_t timeorigin = 0;
    return ( now - timeorigin );
}


//static int s_debug_level = MG_LL_INFO;
static const char *s_root_dir = ".";
static const char *s_listening_address = "http://0.0.0.0:8000";

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

static int apply_config()
{
    MG_INFO(("apply_config"));
    int value;

    int bit_rate_map[] = {2, 4, 8, 12, 16, 20}; //Mbps
    value = config_get(CONFIG_BIT_RATE);
    if (0 < value && value < (int)(sizeof(bit_rate_map) / sizeof(bit_rate_map[0]))) {
        printf("CONFIG_BIT_RATE: %d Mbps\n", bit_rate_map[value]);
    }

    int resolution_map[] = {720, 1080, 1440};
    value = config_get(CONFIG_RESOLUTION);
    if (0 < value && value < (int)(sizeof(resolution_map) / sizeof(resolution_map[0]))) {
        printf("CONFIG_RESOLUTION: %dp\n", resolution_map[value]);
    }

    // 0:open 1:close
    value = config_get(CONFIG_HORIZONTAL_IMAGE);
    // 0:open 1:close
    value = config_get(CONFIG_VERTICAL_IMAGE);
    value = config_get(CONFIG_COLOR_MODE);
    value = config_get(CONFIG_WHITE_LIGHT);
    // 0:open 1:close
    value = config_get(CONFIG_RECORD);
    // 0:open 1:close
    value = config_get(CONFIG_NTP);
    return 0;
}

static void web_settings_fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    //struct mg_http_serve_opts opts = {.root_dir = s_root_dir};   // Serve local dir_fn
    if (ev == MG_EV_POLL) {
        return;
    }

    MG_INFO(("ev: %d, %s\n", ev, mg_ev_enum2strings(ev)));
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        struct user_info *u = getuser(hm);
        MG_INFO(("http: \n%s\n", hm->message.ptr));
        if (mg_http_match_uri(hm, "/api/fn3")) {
            MG_INFO(("zsppp test fn3 ##############################\n"));
            struct mg_str *host;
            host = mg_http_get_header(hm, "Host");
            if (host) {
                MG_INFO(("zsppp host:%s\n", host->ptr));
                printf("host end\n");
            }
            //mg_http_reply(c, 301, "Location: /web_root\r\n", "test /web_root\n");
            //mg_http_reply(c, 200, "Content-Type: text/plain;charset=UTF-8\r\n", "/web_root\n");
            //mg_http_reply(c, 200, "Content-Type: application/json\r\n ", "{%Q:\"%s\"}\n",
            //              "url", "web_root");
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", config_get_json().c_str());
            printf("\n%s\n", config_get_json().c_str());


            //struct mg_http_serve_opts opts = {.root_dir = "./web_root"};   // Serve local dir
            //mg_http_serve_dir(c, ev_data, &opts);
        }
        else if (mg_http_match_uri(hm, "/api/submit")) {
            printf("body:\n%s\n", hm->body.ptr);
            int ret = 0;
            ret = config_set_json(hm->body.ptr);
            ret |= apply_config();
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

//用户认证
#if 1
        else if (u == NULL && mg_http_match_uri(hm, "/api/#")) {
            MG_INFO(("zsppp denied ##############################\n"));
            // All URIs starting with /api/ must be authenticated
            mg_http_reply(c, 403, "", "Denied\n");
            return;
        }
#endif
        else if (mg_http_match_uri(hm, "/api/data")) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{%Q:%Q,%Q:%Q}\n", "text", "Hello!", "data", "somedata");
        }
        else if (mg_http_match_uri(hm, "/api/login")) {
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{%Q:%Q,%Q:%Q}\n", "user", u->name, "token", u->token);
        }


        else {
            MG_INFO(("zsppp unknow uri\n"));
            struct mg_http_serve_opts opts = {.root_dir = s_root_dir};   // Serve local dir
            mg_http_serve_dir(c, (mg_http_message*)ev_data, &opts);
        }
    }
    else {
    }
}


int main(int argc, char *argv[]) {
    config_load();

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                        // Init manager
    //mg_http_listen(&mgr, s_listening_address, dir_fn, &mgr);  // Setup listener
    mg_http_listen(&mgr, s_listening_address, web_settings_fn, &mgr);  // Setup listener

    MG_INFO(("Mongoose version : v%s", MG_VERSION));
    MG_INFO(("Listening on     : %s", s_listening_address));
    MG_INFO(("Web root         : [%s]", s_root_dir));

    for (;;) mg_mgr_poll(&mgr, 1000);                         // Event loop
    mg_mgr_free(&mgr);                                        // Cleanup
    return 0;
}

