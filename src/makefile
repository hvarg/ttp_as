FILES = ttp_as.c xhstt_parser.c structures.c
HEADERS = xhstt_parser.h structures.h
OUT = ttp_as

all: $(FILES) $(HEADERS)
	gcc -ggdb -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

wall: $(FILES) $(HEADERS)
	gcc -Wall -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

clean:
	rm $(OUT)
