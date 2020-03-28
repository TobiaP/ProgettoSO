.PHONY: help build clean

CC = gcc
CFLAGS = -std=gnu90 -g Wall -pedantic

GREEN=\033[0;32m
RED=\033[0;31m
CYAN=\033[0;36m
NC=\033[0m

default: help

help:
  @echo "$(GREEN)
  @echo
  @echo
  @echo
  @echo
  
