#include "opc_ua/opc_ua_client.h"
#include "global.h"


// receive update from string node
static void dataChangeCallback(UA_Client *client, UA_UInt32 subId, void *subContext,
    UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_STRING])) {
        UA_String *str = (UA_String *)value->value.data;
        printf("Received update: %.*s\n", (int)str->length, str->data);
        // TODO: odbierz dane
        pthread_mutex_unlock(&lock[0]);

        sleep(60);
        pthread_mutex_lock(&lock[0]);
    }
}


static void clientRun() {
    client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, "opc.tcp://192.168.192.26:4840");
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
                                                  monRequest, NULL, dataChangeCallback, NULL);
    }

    float tab[73] = {0};

    while (true) {
        //UA_Client_run_iterate(client, 1000);
        // read_valueString(client);
        // read_valueArray(client);
        // writeStringToNode(client, UA_NODEID_NUMERIC(1, 2101), "jeden");
        // writeArrayToNode(client, UA_NODEID_NUMERIC(1, 2201), tab, 73);
        sleep(5);
    }

    UA_Client_delete(client);
}

 int main(void) {
     printf("OPC UA Client started...\n");
     clientRun();
     return 0;
}
