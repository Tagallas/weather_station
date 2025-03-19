#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <stdio.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>

static void dataChangeCallback(UA_Client *client, UA_UInt32 subId, void *subContext,
                               UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {
        UA_String *str = (UA_String *)value->value.data;
        printf("Received update: %.*s\n", (int)str->length, str->data);
    }
}

static void clientRun() {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if (status != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        printf("Failed to connect to server\n");
        return;
    }

    UA_NodeId nodeId = UA_NODEID_STRING(1, "customVariable");
    UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request, NULL, NULL, NULL);

    if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
        UA_MonitoredItemCreateRequest monRequest =
            UA_MonitoredItemCreateRequest_default(nodeId);
        UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId, UA_TIMESTAMPSTORETURN_BOTH,
                                                  monRequest, NULL, dataChangeCallback, NULL);
    }

    while (true) {
        UA_Client_run_iterate(client, 1000);
    }

    UA_Client_delete(client);
}

int main(void) {
    printf("OPC UA Client started...\n");
    clientRun();
    return 0;
}