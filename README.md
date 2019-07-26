# Paho 1.3.0 -> VerneMQ + mqtt-gateway with MQTT v5 test

1. Run `./run.sh` to build and start a docker container with the environment.
2. Inside the container compile the example with `make`.
3. Run the example with `./mqtt5_test`.
4. From another terminal on the localhost publish a message with mosquitto client:

```bash
mosquitto_pub \
  -h localhost \
  -p 1883 \
  -i "v1/service-agents/client-pub.usr.example.org" \
  -t "agents/alpha.mqtt5-test.svc.example.org/api/v1/in/client.svc.example.org" \
  -m '{"payload": "{\"key\": \"value\"}"}'
```

Expected log:

```
Connected
Subscribed
Message arrived to topic `agents/alpha.mqtt5-test.svc.example.org/api/v1/in/client.svc.example.org`: {"key":"value"}
Disconnected
Done
```
