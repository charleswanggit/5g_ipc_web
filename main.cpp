/* ************************************************************************
 *    File Name:     main.cpp
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月15日 星期三 10时47分13秒
 *  Description:   
 ************************************************************************/
#include "web_server.h"
#include "mtc_log.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

static int g_sig_stop = 0;
static void demo_handle_sig(int signo)
{
    g_sig_stop = 1;
}

static int cb(CONFIG_ITEM key, const char *value)
{
    mtc_print_info("%d, %s\n", key, value);
    return 0;
}

static int cb(STATUS_ITEM key, char *value, int size)
{
    snprintf(value, size, "ok: %d", key);
    mtc_print_info("%d, %s\n", key, value);
    return 0;
}


int main(int argc, char *argv[]) {

    mtc_print_info("%s ae build in %s %s\n", argv[0], __DATE__, __TIME__);
    /*signal处理, CTRL+C退出*/
    signal(SIGINT,  demo_handle_sig);
    signal(SIGQUIT, demo_handle_sig);
    signal(SIGKILL, demo_handle_sig);
    signal(SIGTERM, demo_handle_sig);

    web_init();
    web_set_config_cb(cb);
    web_set_status_cb(cb);
    while (!g_sig_stop) {
        sleep(1);
    }
    web_deinit();
    return 0;
}

