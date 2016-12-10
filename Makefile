GAMES := snake
LDLIBS := -s -ltermbox

all: $(GAMES)

clean:
	rm -f $(GAMES)
