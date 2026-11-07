LIBS := 
FLAGS := -Wall -Wextra -fsanitize=address -fstack-protector-strong

build:
	gcc src/* $(FLAGS) $(LIBS) -o main

run: build
	./main
