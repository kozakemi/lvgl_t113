#ifndef CHECK_WIFI_H
#define CHECK_WIFI_H
int check_wifi_state(char * interface) {
    FILE *fp;
    char operstate[10];

    // 构建路径
    char path[50];
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", interface);

    // 打开文件读取操作状态
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Failed to open operstate file");
        return 1;
    }

    // 读取状态
    if (fgets(operstate, sizeof(operstate), fp) != NULL) {
        if (strncmp(operstate, "up", 2) == 0) {
            printf("WiFi interface %s is up\n", interface);
            fclose(fp);
            return 0;
        } else {
            printf("WiFi interface %s is down\n", interface);
            fclose(fp);
            return 1;
        }
    } else {
        printf("Failed to read WiFi state\n");
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 1;
}
#endif