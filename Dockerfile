FROM netologygroup/vernemq-buster:1.7.1 as vernemq
FROM netologygroup/mqtt-gateway:v0.11.1 as mqtt-gateway-plugin
FROM debian:buster

# Build tools & dependencies
RUN apt-get update && apt-get install -y build-essential git libssl-dev libtinfo5

# Paho
RUN PAHO_MQTT_BUILD_DIR=$(mktemp -d) \
  && cd "${PAHO_MQTT_BUILD_DIR}" \
  && git clone "https://github.com/eclipse/paho.mqtt.c.git" . \
  && git checkout "v1.3.0" \
  && make \
  && make install \
  && rm -rf "${PAHO_BUILD_DIR}"

# VerneMQ with mqtt-gateway
COPY --from=vernemq "/vernemq" "/opt/vernemq"
COPY --from=vernemq /vernemq/etc/vm.args /opt/vernemq/etc/vm.args

RUN ln -s /opt/vernemq/etc /etc/vernemq \
    && ln -s /opt/vernemq/data /var/lib/vernemq \
    && ln -s /opt/vernemq/log /var/log/vernemq

COPY --from=mqtt-gateway-plugin "/app" "/opt/mqttgw"

ENV APP_AUTHN_ENABLED "0"
ENV APP_AUTHZ_ENABLED "0"

RUN set -xe \
    && VERNEMQ_ENV='/opt/vernemq/lib/env.sh' \
    && perl -pi -e 's/(RUNNER_USER=).*/${1}root\n/s' "${VERNEMQ_ENV}" \
    && VERNEMQ_CONF='/etc/vernemq/vernemq.conf' \
    && perl -pi -e 's/(listener.tcp.default = ).*/${1}0.0.0.0:1883\nlistener.ws.default = 0.0.0.0:8080/g' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/(plugins.vmq_passwd = ).*/${1}off/s' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/(plugins.vmq_acl = ).*/${1}off/s' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/## (listener.tcp.allowed_protocol_versions = ).*/${1}3,4,5\n/s' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/## (listener.ssl.allowed_protocol_versions = ).*/${1}3,4,5\n/s' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/## (listener.ws.allowed_protocol_versions = ).*/${1}3,4,5\n/s' "${VERNEMQ_CONF}" \
    && perl -pi -e 's/## (listener.wss.allowed_protocol_versions = ).*/${1}3,4,5\n/s' "${VERNEMQ_CONF}" \
    && printf "\nplugins.mqttgw = on\nplugins.mqttgw.path = /opt/mqttgw\n" >> "${VERNEMQ_CONF}"
