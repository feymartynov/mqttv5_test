docker build -t mqtt5_test .

docker run \
  --rm \
  -it \
  -v $(pwd):/opt/mqtt5_test \
  -p 1883:1883 \
  -p 8080:8080 \
  mqtt5_test \
  /bin/bash -c "/opt/vernemq/bin/vernemq start && cd /opt/mqtt5_test && /bin/bash"
