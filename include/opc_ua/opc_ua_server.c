#include "opc_ua_server.h"


static void update_variable(UA_Server *server) {
    static int counter = 0;
    CustomData data;
    data.intValue = counter;
    snprintf(data.stringValue, sizeof(data.stringValue), "Message %d", counter);
    counter++;

    UA_Variant value;
    UA_Variant_init(&value);
    UA_String uaString = UA_STRING(data.stringValue);
    UA_Variant_setScalar(&value, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId nodeId = UA_NODEID_NUMERIC(1, 2101);
    UA_Server_writeValue(server, nodeId, value);
}


static void add_object_node(UA_Server *server, char* name, int nodeID, UA_NodeId parent_nodeID) {
    UA_NodeId myObject;
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);

    UA_Server_addObjectNode(server, 
        UA_NODEID_NUMERIC(1, nodeID),  
        parent_nodeID, 
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), 
        UA_QUALIFIEDNAME(1, name), // name
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE), // type
        oAttr, NULL, &myObject);
}


static void add_string_node(UA_Server *server, char* name, int nodeID, int parent_nodeID){

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE; 

    UA_String initialString = UA_STRING("01/01/2000 00:00");
    UA_Variant_setScalar(&attr.value, &initialString, &UA_TYPES[UA_TYPES_STRING]);

    UA_NodeId nodeId = UA_NODEID_NUMERIC(1, nodeID); // node ID
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, parent_nodeID); // parent ID
    UA_Server_addVariableNode(server, nodeId, parentNodeId, 
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), // relasion
        UA_QUALIFIEDNAME(1, name),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), // variable type
        attr, NULL, NULL);
}


static void add_float_array_node(UA_Server *server, char* name, int nodeID, int parent_nodeID) {

    float values[73] = {0}; 
    for (int i = 0; i<73; i++){
        values[i] = i;
    }

    // setting up attributes
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.dataType = UA_TYPES[UA_TYPES_FLOAT].typeId; 
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE; 

    attr.valueRank = 1; // array size
    attr.arrayDimensionsSize = 1;
    UA_UInt32 arrayDims = 73;
    attr.arrayDimensions = &arrayDims;

    UA_Variant_setArrayCopy(&attr.value, values, 73, &UA_TYPES[UA_TYPES_FLOAT]);

    // adding node
    UA_Server_addVariableNode(server,
        UA_NODEID_NUMERIC(1, nodeID),
        UA_NODEID_NUMERIC(1, parent_nodeID),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, name),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        attr, NULL, NULL);
}


void add_double_node(UA_Server *server, char *name, int nodeID, int parent_nodeID) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    double initialValue = 0.0;

    UA_Variant_setScalar(&attr.value, &initialValue, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_Server_addVariableNode(server, 
        UA_NODEID_NUMERIC(1, nodeID), 
        UA_NODEID_NUMERIC(1, parent_nodeID),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT), 
        UA_QUALIFIEDNAME(1, name),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), 
        attr, NULL, NULL);
}


void add_int32_node(UA_Server *server, char *name, int nodeID, int parent_nodeID) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    int initialValue = 0;

    UA_Variant_setScalar(&attr.value, &initialValue, &UA_TYPES[UA_TYPES_INT32]);

    UA_NodeId nodeId = UA_NODEID_NUMERIC(1, nodeID);
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(1, parent_nodeID);
    UA_Server_addVariableNode(server, 
        nodeId, 
        parentNodeId, 
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, name),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), 
        attr, NULL, NULL);
}


void create_and_start_opc_ua_server(char *server_url, GeoLoc array[]) {
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
  
    UA_String customServerUrls[1];
    customServerUrls[0] = UA_STRING(server_url);
    size_t customServerUrlsSize = 1;
  
    config->serverUrls = customServerUrls;
    config->serverUrlsSize = customServerUrlsSize;
    config->applicationDescription.applicationName =
        UA_LOCALIZEDTEXT_ALLOC("en", "Example for Medium");
  
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID(OBJECTSFOLDER));
    add_object(server, "Weather station", 1001, parentNodeId);
    add_object(server, "Weather data", 2001, UA_NODEID_NUMERIC(1, 1001));
    add_object(server, "Average data", 3001, UA_NODEID_NUMERIC(1, 1001));
  
    add_weather_object_for_every_section(server, array, 2001);
    add_average_weather_object(server, array, 3001);
    // TODO: Add nodes for other tasks
  
    printf("OPC UA Server started...\n");
    UA_Server_runUntilInterrupt(server);
    UA_Server_delete(server);
  }


void add_weather_object_for_every_section(UA_Server *server, GeoLoc array[],
    int parent_nodeID) {
    int baseID = 2101;
    int offset = 100;
    int j = 0;
    for (int i = 0; i < DATA_ARRAY_SIZE; i++) {
        char *locationName = array[i].cities;
        int nodeID = baseID;
        if (j != 0) {
            nodeID = nodeID + j * offset;
        }
        j++;
        if (j == 9) {
            j = 0;
            baseID = baseID + 10;
        }
        add_weather_object(server, locationName, nodeID, parent_nodeID);
    }
}

void add_average_weather_object(UA_Server *server, GeoLoc array[], int parent_nodeID){
    
    add_weather_object_for_every_section(server, array, parent_nodeID);

}

void add_weather_object(UA_Server *server, char *name, int nodeID,
    int parent_nodeID) {
    add_object(server, name, nodeID, UA_NODEID_NUMERIC(1, parent_nodeID));

    add_string_node(server, "Date", nodeID + 1, nodeID);
    add_double_node(server, "Temperature", nodeID + 2, nodeID);
    add_double_node(server, "WindSpeed", nodeID + 3, nodeID);
    add_int32_node(server, "Cloudiness", nodeID + 4, nodeID);
}