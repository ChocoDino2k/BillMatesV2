CFLAGS = -g -pthread -Wall -Werror
SOURCES = server.c json_util.c lex.yy.c y.tab.c
OBJECTS = $(SOURCES:.c=.o)
HFILES = myserver.h
EXEC = run
DATE = $(shell date +%Y%m%d-%H:%M:%S)
all: $(EXEC) git-commit

$(EXEC): $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^

$(OBJECTS): $(SOURCES)
	gcc $(CFLAGS) -c $^

lex.yy.c: y.tab.c json_lexer.l
	flex json_lexer.l

y.tab.c: json_yacc.y
	yacc -d json_yacc.y

clean:
	rm -f run *.o lex.yy.c y.tab.*

git-commit:
	git add .
	git commit -a -m 'Automatic commit $(DATE)' >> .local.git.out || echo

.PHONY: clean
.PHONY: all
.PHONY: $(EXEC)
.PHONY: git-commit
