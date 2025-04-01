#ifndef OPC_UA_SERVER_H
#define OPC_UA_SERVER_H

#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/types_generated.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../geo_loc/geo_loc_sections/sections.h"
#define DATA_ARRAY_SIZE 69

void add_object_node(UA_Server *server, char* name, int nodeID, UA_NodeId parent_nodeID);
void add_string_node(UA_Server *server, char* name, int nodeID, int parent_nodeID);
void add_float_array_node(UA_Server *server, char* name, int nodeID, int parent_nodeID);
void add_double_node(UA_Server *server, char *name, int nodeID, int parent_nodeID);
void add_int32_node(UA_Server *server, char *name, int nodeID,  int parent_nodeID);
  
void update_variable(UA_Server *server);

void add_weather_object(UA_Server *server, char *name, int nodeID, int parent_nodeID);
void add_weather_object_for_every_section(UA_Server *server, GeoLoc array[], int parent_nodeID);
void add_average_weather_object(UA_Server *server, GeoLoc array[], int parent_nodeID);
void create_and_start_opc_ua_server(const char *server_url, GeoLoc array[]);

#endif  // OPC_UA_SERVER_H