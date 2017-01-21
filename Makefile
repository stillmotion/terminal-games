GAMES := invaders snake sokoban sudoku tetris
LDLIBS := -s -ltermbox

all: $(GAMES)

clean:
	rm -f $(GAMES)
