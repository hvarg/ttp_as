FILES = ttp_as.c xhstt_parser.c
HEADERS = xhstt_parser.h
OUT = ttp_as

all: $(FILES) $(HEADERS)
	gcc -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

clean:
	rm $(OUT)
