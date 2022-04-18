CC=gcc
LIBS=-lcurl -lncursesw -lm -lconfig -lcjson
CFLAGS=-Wall -std=c99 -pedantic -g -DMOVE_IO_CLOSE
VALGND_FLAGS=--tool=memcheck --leak-check=full --track-origins=yes 
#--show-leak-kinds=all
LD_LIBRARY_PATH=/lib:/usr/lib:/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH

odc: build/data.o build/main.o build/api.o build/graphics.o
	$(CC) -o $@ $^ $(LIBS)

build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(CC) -DNCURSES_WIDECHAR=1 $(CFLAGS) -o $@ -c $<

mem: odc
	valgrind $(VALGND_FLAGS) ./odc

clean:
	rm -rf build
	rm odc

run:
	make
	./odc
