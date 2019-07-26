#include "mqtt5_test.h"

int main(int argc, char **argv) {
  Context context;
  context.halt = false;
  void *ctx = &context;

  // Create client
  MQTTAsync_createOptions create_options = MQTTAsync_createOptions_initializer;
  create_options.MQTTVersion = MQTTVERSION_5;

  int create_rc = MQTTAsync_createWithOptions(
    &context.client,
    BROKER_URL,
    CLIENT_ID,
    MQTTCLIENT_PERSISTENCE_NONE,
    NULL,
    &create_options
  );

  if (create_rc != MQTTASYNC_SUCCESS) goto error;

  // Set callbacks
  int set_callbacks_rc = MQTTAsync_setCallbacks(
    context.client,
    ctx,
    on_connection_lost,
    on_message_arrived,
    on_delivery_complete
  );

  if (set_callbacks_rc != MQTTASYNC_SUCCESS) goto error;

  // Connect
  MQTTAsync_connectOptions connect_options = MQTTAsync_connectOptions_initializer5;
  connect_options.context = ctx;
  connect_options.MQTTVersion = MQTTVERSION_5;
  connect_options.keepAliveInterval = KEEP_ALIVE_INTERVAL;
  connect_options.automaticReconnect = 0;
  connect_options.cleanstart = 1;
  connect_options.onSuccess5 = on_connect_success;
  connect_options.onFailure5 = on_connect_failure;

  int connect_rc = MQTTAsync_connect(context.client, &connect_options);
  if (connect_rc != MQTTASYNC_SUCCESS) goto error;

  // The flow has been gone async. Wait for halt in the main thread.
  while (!context.halt) sleep(1);
  printf("Done\n");

  MQTTAsync_destroy(&context.client);
  return 0;

error:
  MQTTAsync_destroy(&context.client);
  return 1;
}

void on_connect_success(void *ctx, MQTTAsync_successData5 *response) {
  printf("Connected\n");

  // Subscribe
  MQTTAsync_responseOptions subscribe_options = MQTTAsync_responseOptions_initializer;
	subscribe_options.context = ctx;
  subscribe_options.onSuccess5 = on_subscribe_success;
  subscribe_options.onFailure5 = on_subscribe_failure;

  MQTTAsync_subscribe(((Context*) ctx)->client, SUBSCRIBE_TOPIC, SUBSCRIBE_QOS, &subscribe_options);
}

void on_connect_failure(void *ctx, MQTTAsync_failureData5 *response) {
  printf("Failed to connect\n");
  ((Context*) ctx)->halt = true;
}

void on_subscribe_success(void *ctx, MQTTAsync_successData5 *response) {
  printf("Subscribed\n");
}

void on_subscribe_failure(void *ctx, MQTTAsync_failureData5 *response) {
  printf("Subscribe failed\n");
  ((Context*) ctx)->halt = true;
}

int on_message_arrived(void *ctx, char *topic, int topic_len, MQTTAsync_message *message) {
  printf("Message arrived to topic `%s`: %s\n", topic, (char*) message->payload);

  // Disconnect
  MQTTAsync_disconnectOptions disconnect_options = MQTTAsync_disconnectOptions_initializer;
  disconnect_options.context = ctx;
  disconnect_options.timeout = DISCONNECT_TIMEOUT;
  disconnect_options.onSuccess5 = on_disconnect_success;
  disconnect_options.onFailure5 = on_disconnect_failure;

  MQTTAsync_disconnect(((Context*) ctx)->client, &disconnect_options);
}

void on_disconnect_success(void *ctx, MQTTAsync_successData5 *response) {
  printf("Disconnected\n");
  ((Context*) ctx)->halt = true;
}

void on_disconnect_failure(void *ctx, MQTTAsync_failureData5 *response) {
  printf("Failed to disconnect\n");
  ((Context*) ctx)->halt = true;
}

void on_connection_lost(void *ctx, char *cause) {
  printf("Connection lost: %s\n", cause);
  ((Context*) ctx)->halt = true;
}

void on_delivery_complete(void *ctx, MQTTAsync_token token) {
}

