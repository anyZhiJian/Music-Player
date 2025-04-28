#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    exwifi_mode_ap,
    exwifi_mode_sta,
    exwifi_mode_ap_sta
} exwifi_mode_t;

void exwifi_init(const char *ssid, const char *password, exwifi_mode_t mode);
void exwifi_scan(void);

#ifdef __cplusplus
}
#endif