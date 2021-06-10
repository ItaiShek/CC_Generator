# create an executable for CC_Generator

CC_Generator: main.c
	gcc -g -o CC_Generator main.c -lm

.PHONY: clean

clean:
	rm -f CC_Generator
