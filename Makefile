TARGET: testhttp_raw

CC	= cc
LFLAGS	= -Wall

testhttp_raw.o cookies.o http.o input.o tcp.o utils.o err.o: cookies.h http.h input.h tcp.h utils.h err.h

testhttp_raw: testhttp_raw.o cookies.o http.o input.o tcp.o utils.o err.o
	$(CC) $(LFLAGS) $^ -o $@

.PHONY: clean TARGET
clean:
	rm -f testhttp_raw *.o *~ *.bak
