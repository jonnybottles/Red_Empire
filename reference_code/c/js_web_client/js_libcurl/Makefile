CC=gcc
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Waggregate-return -Wfloat-equal -Wwrite-strings
CFLAGS += -std=c18

js_libcurl: js_libcurl.o -lcurl
js_libcurl.o: CPPFLAGS += -D_GNU_SOURCE

.PHONY: debug
debug: CFLAGS += -g
debug: js_libcurl


.PHONY: clean
clean:
	$(RM) js_ *.o