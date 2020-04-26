.PHONY: help targets build clean

CC = gcc
CFLAGS = -std=gnu90 -g -Wall -pedantic

GREEN=\033[0;32m
RED=\033[0;31m
CYAN=\033[0;36m
STD=\033[0m

default: help

targets: bin/main bin/Coda bin/Button bin/Led bin/S bin/Switc

bin/main: src/main.c bin/util.o
        $(CC) $(CFLAGS) -o bin/main src/main.c bin/util.o

bin/Coda: src/Coda.c bin/util.o
        $(CC) $(CFLAGS) -o bin/Coda src/Coda.c bin/util.o

bin/Button: src/Button.c bin/util.o
        $(CC) $(CFLAGS) -o bin/Button src/Button.c bin/util.o

bin/Led: src/Led.c bin/util.o
        $(CC) $(CFLAGS) -o bin/Led src/Led.c bin/util.o

bin/S: src/S.c bin/util.o
        $(CC) $(CFLAGS) -o bin/S src/S.c bin/util.o

bin/Switc: src/Switc.c bin/util.o
        $(CC) $(CFLAGS) -o bin/Switc src/Switc.c bin/util.o

bin/util.o: src/util.c
        $(CC) $(CFLAGS) -c -o bin/util.o src/util.c

help:
        @echo "$(RED)INFORMAZIONI SUL PROGETTO$(STD)"
        @echo "Gaia Faggin"
        @echo "Tobia Pavona"                                                                                                    @echo "Thomas Herbert"                                                                                                  @echo "Francesco Giacomazzi"
        @echo "\n Per iniziare: digita $(RED) make build(STD), poi $(RED)./run$(STD), oppure $(RED) make clean$(STD) pe$
build:
        @make clean
        @mkdir /tmp/ipc
        @mkdir /tmp/ipc/mqueues
        @mkdir bin
        @make targets
        @ln -s bin/main run
        @chmod +x run
        @echo "Compilazione completata. Usa ./run per avviare il main."

clean:
        @rm -f -r /tmp/ipc
        @rm -f -r bin
        @rm -f run
        @echo "Pulizia completata."
