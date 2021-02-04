cc=gcc

odc: build/data.o build/main.o build/api.o build/graphics.o build/jsmn.o
	gcc -o $@ -lcurl -lncursesw -lm -lconfig $^ -I.


build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) -DNCURSES_WIDECHAR=1 $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -rf build
	rm odc

run:
	make
	./odc
