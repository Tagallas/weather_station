#include "opc_ua_client.h"


void browse_node(UA_Client *client, UA_NodeId startNode) {
    printf("Browsing nodes under NodeId %d\n", startNode.identifier.numeric);

    // Create a Browse Request
    UA_BrowseRequest bReq;
    UA_BrowseRequest_init(&bReq);
    bReq.requestedMaxReferencesPerNode = 0; // No limit on results
    bReq.nodesToBrowse = UA_BrowseDescription_new();
    bReq.nodesToBrowseSize = 1;
    
    // Set the starting node (e.g., ObjectsFolder)
    bReq.nodesToBrowse[0].nodeId = startNode;
    bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; 

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


void read_value_string(UA_Client *client){
    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode status = UA_Client_readValueAttribute(client, UA_NODEID_NUMERIC(1, 2101), &value);
    if (status == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_STRING])) {
        printf("Value: %*s\n", *(UA_String*)value.data);
    }
    UA_Variant_clear(&value);
}


void read_value_array(UA_Client *client){
    UA_Variant value;
    UA_Variant_init(&value);
    UA_StatusCode status = UA_Client_readValueAttribute(client, UA_NODEID_NUMERIC(1, 2201), &value);
    if (status == UA_STATUSCODE_GOOD) {
        float *arr = (float *)value.data;
            printf("(73-element float array):\n");
            for (size_t i = 0; i < 73; i++) {
                printf("[%zu] = %f\n", i, arr[i]);
            }
            printf("\n");
    }
    else {
        printf("Couldn't read array value\n");
    }
    UA_Variant_clear(&value);
}


static void handler_data_change(UA_Client *client, UA_UInt32 subId, void *subContext,
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


void write_array_to_node(UA_Client *client, UA_NodeId nodeId, float *data, size_t size) {

    UA_Variant value;
    UA_Variant_init(&value);
    UA_Variant_setArray(&value, data, size, &UA_TYPES[UA_TYPES_FLOAT]);
    
    // send value to server
    UA_StatusCode status = UA_Client_writeValueAttribute(client, nodeId, &value);

    if (status != UA_STATUSCODE_GOOD) {
        printf("Write array error: %s\n", UA_StatusCode_name(status));
    }
}


void write_string_to_node(UA_Client *client, UA_NodeId nodeId, const char *text) {
    UA_Variant value;
    UA_Variant_init(&value);

    UA_String uaString = UA_STRING_ALLOC(text);
    UA_Variant_setScalar(&value, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    // send value to server
    UA_StatusCode status = UA_Client_writeValueAttribute(client, nodeId, &value);

    if (status != UA_STATUSCODE_GOOD) {
        printf("Write string error: %s\n", UA_StatusCode_name(status));
    }

    UA_String_clear(&uaString);
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
