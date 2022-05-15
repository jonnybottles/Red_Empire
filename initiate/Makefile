CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Waggregate-return -Wfloat-equal -Wwrite-strings
CFLAGS += -std=c18

initiate: host_utils.o curl_utils.o initiate.o initiate_driver.o -lcurl

initiate.o: initiate.h initiate.c
initiate.o: CPPFLAGS += -D_GNU_SOURCE

curl_utils.o: curl_utils.h curl_utils.c

host_utils.o: host_utils.h host_utils.c
host_utils.o: CPPFLAGS += -D_GNU_SOURCE

.PHONY: debug
debug: CFLAGS += -g
debug: initiate

.PHONY: clean
clean:
	$(RM) initiate *.o