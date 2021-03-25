CC=gcc
LIBS=-lcurl -lncursesw -lm -lconfig -lcjson
CFLAGS=-Wall -std=c99 -pedantic -g -DMOVE_IO_CLOSE

odc: build/data.o build/main.o build/api.o build/graphics.o 
	$(CC) -o $@ $(LIBS) $^ -I.


build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(CC) -DNCURSES_WIDECHAR=1 $(CFLAGS) -o $@ -c $<

clean:
	rm -rf build
	rm odc

run:
	make
	./odc
