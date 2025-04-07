#ifndef OPC_UA_CLIENT_H
#define OPC_UA_CLIENT_H

#include <open62541/client.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/client_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>
#include <stdio.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "../global.h"

static const int nodeIDs[DATA_ARRAY_SIZE] = {
    2101, 2201, 2301, 2401, 2501, 2601, 2701, 2801, 2901, 2111, 2211, 2311,
    2411, 2511, 2611, 2711, 2811, 2911, 2121, 2221, 2321, 2421, 2521, 2621,
    2721, 2821, 2921, 2131, 2231, 2331, 2431, 2531, 2631, 2731, 2831, 2931,
    2141, 2241, 2341, 2441, 2541, 2641, 2741, 2841, 2941, 2151, 2251, 2351,
    2451, 2551, 2651, 2751, 2851, 2951, 2161, 2261, 2361, 2461, 2561, 2661,
    2761, 2861, 2961, 2171, 2271, 2371, 2471, 2571, 2671};

extern ReceivedWeatherData data_table[DATA_ARRAY_SIZE];
  
void browse_node(UA_Client *client, UA_NodeId startNode);

void read_value_array(UA_Client *client);
UA_String read_value_string(UA_Client *client, int node_id);
int read_value_int32(UA_Client *client, int node_id);
double read_value_double(UA_Client *client, int node_id);
ReceivedWeatherData read_weather_data(UA_Client *client, int node_id);

void write_to_array_node(UA_Client *client, int node_id, float *data, size_t size);
void write_to_string_node(UA_Client *client, int node_id, const char *text);
void write_to_double_node(UA_Client *client, int node_id, double new_value);
void write_to_int32_node(UA_Client *client, int node_id, int new_value);

static void handler_data_change(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);

void add_subscription(UA_Client *client, int node_id, UA_Client_DataChangeNotificationCallback callback);
UA_Client *create_and_start_opc_ua_client(char *server_url);

#endif  // OPC_UA_CLIENT_H