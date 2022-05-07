CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Waggregate-return -Wfloat-equal -Wwrite-strings
CFLAGS += -std=c18

initiate: initiate.o initiate_driver.o -lcurl

initiate.o: initiate.h initiate.c 
initiate.o: CPPFLAGS += -D_GNU_SOURCE

.PHONY: debug
debug: CFLAGS += -g
debug: initiate

.PHONY: clean
clean:
	$(RM) initiate *.o