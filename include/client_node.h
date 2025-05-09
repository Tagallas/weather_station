#ifndef OPC_UA_SERVER_H
#define OPC_UA_SERVER_H

#include "opc_ua/opc_ua_client.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t mutex2;

extern char time_table[DATA_ARRAY_SIZE][17];
extern double temperature_table[DATA_ARRAY_SIZE];
extern double wind_speed_table[DATA_ARRAY_SIZE];
extern int cloudiness_table[DATA_ARRAY_SIZE];

extern volatile int time_idx;
extern volatile int temperature_idx;
extern volatile int wind_speed_idx;
extern volatile int cloudiness_idx;

extern char max_time[17];
extern double temperature_sum;
extern double wind_speed_sum;
extern int cloudiness_sum;

int find_idx(int node_id);

static void dataChangeCallbackTime(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackTemp(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackWind(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackCloud(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);

void send_data(); 
void client_run();

#endif