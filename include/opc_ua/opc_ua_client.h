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


void browse_node(UA_Client *client, UA_NodeId startNode);

static void handler_data_change(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value);

void read_value_string(UA_Client *client);
void read_value_array(UA_Client *client);

void write_array_to_node(UA_Client *client, UA_NodeId nodeId, float *data, size_t size);
void write_string_to_node(UA_Client *client, UA_NodeId nodeId, const char *text);

#endif  // OPC_UA_CLIENT_H