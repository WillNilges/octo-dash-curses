cc=gcc

octo: build/main.o build/util.o
	gcc -o $@ -lcurl $^ -I.

build/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
