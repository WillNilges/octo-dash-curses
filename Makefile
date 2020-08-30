cc=gcc

octodashcurses: build/main.o build/util.o build/jsmn.o
	gcc -o $@ -lcurl -lncurses $^ -I.

build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -rf build
