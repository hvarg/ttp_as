FILES = ttp_as.c xhstt_parser.c structures.c
HEADERS = xhstt_parser.h structures.h
OUT = ttp_as
DEBUG = -Wall -ggdb
VERB = -DVERBOSE=1

all: $(FILES) $(HEADERS)
	gcc -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

debug: $(FILES) $(HEADERS)
	gcc $(DEBUG) -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

verbose: $(FILES) $(HEADERS)
	gcc $(VERB) -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

clean:
	rm $(OUT)
