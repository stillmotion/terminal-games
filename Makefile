GAMES := snake tetris
LDLIBS := -s -ltermbox

all: $(GAMES)

clean:
	rm -f $(GAMES)
