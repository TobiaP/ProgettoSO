.PHONY: help targets build clean 

CC = gcc
CFLAGS = -std=gnu90 -g -Wall -pedantic

GREEN=\033[0;32m
RED=\033[0;31m
CYAN=\033[0;36m
STD=\033[0m

default: help

targets: bin/main

bin/main: src/main.c bin/util.o 
	$(CC) $(CFLAGS) -o bin/main src/main.c bin/util.o 
		
bin/button: src/button.c bin/util.o 
	$(CC) $(CFLAGS) -o bin/button src/button.c bin/util.o 
	
bin/led: src/led.c bin/util.o
	$(CC) $(CFLAGS) -o bin/led src/led.c bin/util.o

bin/s: src/s.c bin/util.o
	$(CC) $(CFLAGS) -o bin/s src/s.c bin/util.o

bin/switc: src/switc.c bin/util.o
	$(CC) $(CFLAGS) -o bin/switc src/switc.c bin/util.o

bin/util.o: src/util.c
	$(CC) $(CFLAGS) -c -o bin/util.o src/util.c

bin/coda.o: src/coda.c
	$(CC) $(CFLAGS) -c -o bin/coda src/coda.c bin/util.o

help:
	@echo "$(RED)INFORMAZIONI SUL PROGETTO$(STD)"
	@echo "Gaia Faggin"
	@echo "Tobia Pavona"
    	@echo "Thomas Herbert"
    	@echo "Francesco Giacomazzi"
	@echo "\n Per iniziare: digita $(RED) make build$(STD), poi $(RED)./run$(STD), oppure $(RED) make clean$(STD) per rimuovere i file temporanei."

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
	@rm -f - r /tmp/ipc
	@rm -f -r bin
	@rm -f run
	@echo "Pulizia completata."
