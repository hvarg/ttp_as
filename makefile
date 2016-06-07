FILES = ttp_as.c
OUT = ttp_as

all: $(FILES)
	gcc -o $(OUT) `xml2-config --cflags` `xml2-config --libs` $(FILES)

clean:
	rm $(OUT)
