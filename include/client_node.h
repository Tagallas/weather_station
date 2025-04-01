#ifndef OPC_UA_SERVER_H
#define OPC_UA_SERVER_H

#include "opc_ua/opc_ua_client.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t mutex2;

extern UA_String time_table[DATA_ARRAY_SIZE];
extern double temperature_table[DATA_ARRAY_SIZE];
extern double wind_speed_table[DATA_ARRAY_SIZE];
extern int cloudiness[DATA_ARRAY_SIZE];

extern volatile int time_idx;
extern volatile int temperature_idx;
extern volatile int wind_speed_idx;
extern volatile int cloudiness_idx;

static void dataChangeCallbackTime(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackTemp(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackWind(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);
static void dataChangeCallbackCloud(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);

void clientRun();

#endif