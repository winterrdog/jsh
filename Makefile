jsh: src/jshell.c
	@${CC} -Ofast -std=c17 -o $@ $<

debug: src/jshell.c
	@${CC} -Wall -Wextra -pedantic -ggdb3 -Og -std=c17 -o $@ $<

clean: 
	@rm -rf jsh debug 