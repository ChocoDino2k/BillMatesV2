CFLAGS = -Wall -Werror -g -pthread
SOURCES = server.c
OBJECTS = $(SOURCES:.c=.o)
HFILES = myserver.h
EXEC = run
DATE = $(shell date +%Y%m%d-%H:%M:%S)
all: clean $(EXEC) git-commit

$(EXEC): $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^

$(OBJECTS): $(SOURCES)
	gcc $(CFLAGS) -c $^

clean:
	rm -f run *.o

git-commit:
	git add *.c *.h Makefile >> .local.git.out || echo
	git commit -a -m 'Automatic commit $(DATE)' >> .local.git.out || echo

.PHONY: clean
.PHONY: all
.PHONY: $(EXEC)
.PHONY: git-commit
