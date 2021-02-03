cc=gcc

odc: build/data.o build/main.o build/util.o build/graphics.o build/jsmn.o
	gcc -o $@ -lcurl -lncursesw -lm -lconfig $^ -I.


thefuck: build/data.o build/main.o build/util.o build/graphics.o build/jsmn.o
	gcc -o $@ -g -g0 -ggdb -lcurl -lncursesw -lm -lconfig $^ -I.

build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) -DNCURSES_WIDECHAR=1 $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	#$(PREFIX)$(CC) -DNCURSES_WIDECHAR=1 $(CFLAGS) $(CPPFLAGS) -o $@ -g -ggdb -c $<

clean:
	rm -rf build
	rm odc

run:
	make
	./odc
