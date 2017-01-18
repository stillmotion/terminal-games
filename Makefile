GAMES := invaders snake sokoban tetris
LDLIBS := -s -ltermbox

all: $(GAMES)

clean:
	rm -f $(GAMES)
