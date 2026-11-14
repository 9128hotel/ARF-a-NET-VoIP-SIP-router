FILE_OUTPUT := main # if you change this remember to change the "remove main" github action
LIBS := 
FLAGS := -Wall -Wextra -fsanitize=address -fstack-protector-strong

build:
	gcc src/* $(FLAGS) $(LIBS) -o $(FILE_OUTPUT)

run: build
	./$(FILE_OUTPUT)

rm-main:
	git filter-branch --index-filter 'git rm -rf --cached --ignore-unmatch $(FILE_OUTPUT)' HEAD
