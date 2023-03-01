/* ************************************************************************
 *    File Name:     web_server.h
 *       Author:     zhongshupeng
 *         mail:     media_zsp@qq.com
 *      Company:     shinra
 * Created Time:     2023年02月28日 星期二 10时59分44秒
 *  Description:   
 ************************************************************************/
#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#include "web_enum.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*web_config_callback)(CONFIG_ITEM key, const char *value);
typedef int (*web_status_callback)(STATUS_ITEM key, char *value, int size);

int web_init();
int web_set_config_cb(web_config_callback cb);
int web_set_status_cb(web_status_callback cb);
int web_apply_config();
int web_deinit();

#ifdef __cplusplus
}
#endif

#endif /* end of __WEB_SERVER_H */

