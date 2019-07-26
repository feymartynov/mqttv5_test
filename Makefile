all: mqtt5_test

mqtt5_test:
	gcc mqtt5_test.c -lpaho-mqtt3as -o mqtt5_test

clean:
	rm mqtt5_test
