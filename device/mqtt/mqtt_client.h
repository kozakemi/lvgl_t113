#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

bool mqtt_is_connected();
int start_mqtt_client();
void stop_mqtt_client();
#endif