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


void browseNode(UA_Client *client, UA_NodeId startNode) {
    printf("Browsing nodes under NodeId %d\n", startNode.identifier.numeric);

    // Create a Browse Request
    UA_BrowseRequest bReq;
    UA_BrowseRequest_init(&bReq);
    bReq.requestedMaxReferencesPerNode = 0; // No limit on results
    bReq.nodesToBrowse = UA_BrowseDescription_new();
    bReq.nodesToBrowseSize = 1;
    
    // Set the starting node (e.g., ObjectsFolder)
    bReq.nodesToBrowse[0].nodeId = startNode;
    bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; // Get all info (NodeId, BrowseName, DisplayName, etc.)

    // Send browse request
    UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);

    if (bResp.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        for (size_t i = 0; i < bResp.resultsSize; i++) {
            for (size_t j = 0; j < bResp.results[i].referencesSize; j++) {
                UA_ReferenceDescription *ref = &bResp.results[i].references[j];

                printf("Found Node: %.*s (NodeId: %d)\n",
                       (int)ref->browseName.name.length, ref->browseName.name.data,
                       ref->nodeId.nodeId.identifier.numeric);
            }
        }
    } else {
        printf("Browse failed with status code: %x\n", bResp.responseHeader.serviceResult);
    }

    // Cleanup
    UA_BrowseResponse_clear(&bResp);
    UA_BrowseRequest_clear(&bReq);
}


// read node value
void read_value(UA_Client *client){
    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode status = UA_Client_readValueAttribute(client, UA_NODEID_NUMERIC(1, 1002), &value);
    if (status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])) {
        printf("Value: %d\n", *(UA_Int32*)value.data);
    }
    UA_Variant_clear(&value);
}


// zmiana wartości, subscribe na danym nodzie
static void dataChangeCallback(UA_Client *client, UA_UInt32 subId, void *subContext,
                               UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {
        UA_String *str = (UA_String *)value->value.data;
        printf("Received update: %.*s\n", (int)str->length, str->data);
    }
}
static void handler_DataChange(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {

    if (value->hasValue) {
        size_t arrayLength = value->value.arrayLength;
        if (arrayLength == 73 && value->value.type == &UA_TYPES[UA_TYPES_FLOAT]) {
            float *arr = (float *)value->value.data;
            printf("Received update (73-element float array):\n");
            for (size_t i = 0; i < arrayLength; i++) {
                printf("[%zu] = %f\n", i, arr[i]);
            }
            printf("\n");
        } else {
            printf("Received update, but data is not a 73-element float array\n");
        }
    } else {
        printf("No valid data received\n");
    }
}
// w main:
// UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
// UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, NULL); 

// if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
//     printf("Subskrypcja utworzona!\n");
// }

// UA_MonitoredItemCreateRequest monRequest =
//     UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(1, 2102)); // dodaje monitorowany węzeł: Temperature Machine1
// UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId,
//                                           UA_TIMESTAMPSTORETURN_BOTH, monRequest,
//                                           NULL, handler_DataChange, NULL); // handler_DataChange - obsługuje zmiane wartości
// while (1) {
//     UA_Client_run_iterate(client, 1000); - odpytuje co 1s server czy zmiana wartości
// }
// UA_Client_runAsync(client, 1000); - odpytuje co 1s i działa w tle


static void clientRun() {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, "opc.tcp://192.168.192.185:4845");
    if (status != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        printf("Failed to connect to server\n");
        return;
    }

    /// subscription
    UA_NodeId nodeId = UA_NODEID_NUMERIC(1, 2201);
    UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, NULL);

    if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        UA_MonitoredItemCreateRequest monRequest =
            UA_MonitoredItemCreateRequest_default(nodeId);
        UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH,
                                                  monRequest, NULL, handler_DataChange, NULL);
    }

    while (true) {
        UA_Client_run_iterate(client, 1000);
    }


    while (true) {
        // browseNode(client, UA_NODEID_NUMERIC(1, 2001));
        sleep(10000);
    }    

    UA_Client_delete(client);
}


int main(void) {
    printf("OPC UA Client started...\n");
    clientRun();
    return 0;
}