struct snake {
	int width;
	int height;
	int score;
	int nfood;
	struct snakeblock *wall;
	struct snakeblock *snake;
	struct snakeblock *food;
};

struct snakeblock {
	int x;
	int y;
	struct snakeblock *next;
};

extern struct snakeblock *snake_up, *snake_down, *snake_left, *snake_right;

int snake(struct snake *sn, struct snakeblock *move);
void snake_free(struct snake *sn);
int snake_canmove(struct snake *sn, struct snakeblock *move);
int snake_new_blank(struct snake *sn, int w, int h, int nfood);

/* -------------------------------------------------------------------------- */

#include <stdlib.h>

static struct snakeblock snake_direction[] = {
	{ 0, -1, NULL },
	{ 0, +1, NULL },
	{ -1, 0, NULL },
	{ +1, 0, NULL }
};

struct snakeblock *snake_up = &snake_direction[0];
struct snakeblock *snake_down = &snake_direction[1];
struct snakeblock *snake_left = &snake_direction[2];
struct snakeblock *snake_right = &snake_direction[3];

static struct snakeblock *new(int x, int y, struct snakeblock *next);
static struct snakeblock *delete(struct snakeblock *lstp, struct snakeblock *q);
static long length(struct snakeblock *listp);
static struct snakeblock *tail(struct snakeblock *listp);
static struct snakeblock *find(int x, int y, struct snakeblock *listp);
static struct snakeblock *randomblock(struct snake *sn);
static void freeblocks(struct snakeblock *listp);

static struct snakeblock *direction(struct snake *sn)
{
	int i, x, y;

	for (i = 0; i < 4; i++) {
		x = snake_direction[i].x + sn->snake->next->x;
		y = snake_direction[i].y + sn->snake->next->y;
		x = (x < 0) ? sn->width - 1 : x % sn->width;
		y = (y < 0) ? sn->height - 1 : y % sn->height;
		if (sn->snake->x == x && sn->snake->y == y)
			return &snake_direction[i];
	}
	return &snake_direction[0];
}

int snake_canmove(struct snake *sn, struct snakeblock *move)
{
	struct snakeblock *d;

	d = direction(sn);
	if (move == NULL || move == d)
		return 1;
	if (move->x+d->x == 0 || move->y+d->y == 0)
		return 0;
	return 1;
}

static int move_snake(struct snake *sn, struct snakeblock *move)
{
	struct snakeblock *p;
	int x, y;

	x = sn->snake->x + move->x;
	y = sn->snake->y + move->y;
	x = (x < 0) ? sn->width - 1 : x % sn->width;
	y = (y < 0) ? sn->height - 1 : y % sn->height;
	if (find(x, y, sn->wall) || find(x, y, sn->snake))
		return -1;
	if ((p = find(x, y, sn->food)) != NULL)
		sn->score++;
	else
		p = tail(sn->snake);
	sn->food = delete(sn->food, p);
	sn->snake = delete(sn->snake, p);
	p->x = x;
	p->y = y;
	p->next = sn->snake;
	sn->snake = p;
	return 0;
}

static int add_food(struct snake *sn)
{
	struct snakeblock *p;

	p = randomblock(sn);
	if (p == NULL)
		return -1;
	p->next = sn->food;
	sn->food = p;
	return 0;
}

int snake(struct snake *sn, struct snakeblock *move)
{
	if (move == NULL || !snake_canmove(sn, move))
		move = direction(sn);
	if (move_snake(sn, move))
		return -1;
	if (length(sn->food) < sn->nfood) {
		if (add_food(sn) && sn->food == NULL)
			return +1;
	}
	return 0;
}

void snake_free(struct snake *sn)
{
	freeblocks(sn->wall);
	freeblocks(sn->snake);
	freeblocks(sn->food);
}

static int snake_new(struct snake *sn, int w, int h, int nfood,
                     struct snakeblock *wall, struct snakeblock *snake)
{
	int i;

	sn->width = w;
	sn->height = h;
	sn->score = 0;
	sn->nfood = nfood;
	sn->wall = wall;
	sn->snake = snake;
	sn->food = NULL;
	for (i = 0; i < nfood; i++)
		add_food(sn);
	return 0;
}

int snake_new_blank(struct snake *sn, int w, int h, int nfood)
{
	struct snakeblock *s, *n, *k;

	k = new(w/2 - 1, h/2, NULL);
	n = new(w/2 + 0, h/2, k);
	s = new(w/2 + 1, h/2, n);
	if (s == NULL || n == NULL || k == NULL
	 || snake_new(sn, w, h, nfood, NULL, s)) {
		free(s);
		free(n);
		free(k);
		return -1;
	}
	return 0;
}

struct snakeblock *delete(struct snakeblock *listp, struct snakeblock *q)
{
	struct snakeblock *p;

	if (listp == q)
		return listp->next;
	for (p = listp; p->next != NULL; p = p->next)
		if (p->next == q) {
			p->next = q->next;
			break;
		}
	return listp;
}

struct snakeblock *tail(struct snakeblock *listp)
{
	struct snakeblock *p;

	if (listp == NULL)
		return NULL;
	for (p = listp; p->next != NULL; p = p->next)
		;
	return p;
}

struct snakeblock *new(int x, int y, struct snakeblock *next)
{
	struct snakeblock *p;

	p = malloc(sizeof *p);
	if (p == NULL)
		return NULL;
	p->x = x;
	p->y = y;
	p->next = next;
	return p;
}

struct snakeblock *find(int x, int y, struct snakeblock *listp)
{
	struct snakeblock *p;

	for (p = listp; p != NULL; p = p->next)
		if (x == p->x && y == p->y)
			return p;
	return NULL;
}

struct snakeblock *randomblock(struct snake *sn)
{
	struct snakeblock tmp, save;
	long i, n;

	n = 0;
	for (i = 0; i < (long) sn->width * sn->height; i++) {
		tmp.x = i % sn->width;
		tmp.y = i / sn->width;
		if (find(tmp.x, tmp.y, sn->wall)
		 || find(tmp.x, tmp.y, sn->snake)
		 || find(tmp.x, tmp.y, sn->food))
			continue;
		if (rand() / (double) RAND_MAX <= 1.0/++n)
			save = tmp;
	}
	return (n == 0) ? NULL : new(save.x, save.y, NULL);
}

long length(struct snakeblock *listp)
{
	struct snakeblock *p;
	long n;

	n = 0;
	for (p = listp; p != NULL; p = p->next)
		n++;
	return n;
}

void freeblocks(struct snakeblock *listp)
{
	struct snakeblock *p;

	for (p = listp; p != NULL; p = listp) {
		listp = p->next;
		free(p);
	}
}

/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termbox.h>

static int playsnake(int width, int height, int nfood, int level);

int main(int argc, char *argv[])
{
	int width, height, nfood, level;
	int i;

	width = 32;
	height = 16;
	nfood = 1;
	level = 3;
	for (i = 1; i < argc; i++)
		if (strcmp(argv[i], "-f") == 0 && i+1 < argc)
			nfood = atoi(argv[++i]);
		else if (strcmp(argv[i], "-d") == 0 && i+1 < argc)
			sscanf(argv[++i], "%dx%d", &width, &height);
		else if (strcmp(argv[i], "-l") == 0 && i+1 < argc)
			level = atoi(argv[++i]);
		else {
			fprintf(stderr,
			        "usage: snake [-d wxh] [-f food] [-l level]\n");
			return EXIT_FAILURE;
		}
	return playsnake(width, height, nfood, level) ? EXIT_FAILURE : 0;
}

static int mainloop(struct snake *sn);

int playsnake(int width, int height, int nfood, int level)
{
	struct snake snake;
	int r;

	if (tb_init()) {
		fprintf(stderr, "snake: can't open display\n");
		return -1;
	}
	tb_select_input_mode(TB_INPUT_ALT);
	atexit(tb_shutdown);
	srand(time(NULL));
	do {
		if (snake_new_blank(&snake, width, height, nfood)) {
			fprintf(stderr, "snake: can't allocate memory\n");
			return EXIT_FAILURE;
		}
		while ((r = mainloop(&snake)) > 0)
			usleep(1000000 / (1 + level));
		snake_free(&snake);
	} while (r >= 0);
	return 0;
}

static int input(void **move, int *paused, struct snake *s)
{
	struct tb_event ev;
	int r;

	while ((r = tb_peek_event(&ev, 0)) > 0) {
		if (r != TB_EVENT_KEY)
			continue;
		if (ev.key == TB_KEY_CTRL_C)
			return -1;
		if (ev.ch == 'p')
			*paused = !*paused;
		if (ev.key == TB_KEY_ARROW_LEFT && snake_canmove(s, snake_left))
			*move = snake_left;
		if (ev.key == TB_KEY_ARROW_RIGHT && snake_canmove(s, snake_right))
			*move = snake_right;
		if (ev.key == TB_KEY_ARROW_UP && snake_canmove(s, snake_up))
			*move = snake_up;
		if (ev.key == TB_KEY_ARROW_DOWN && snake_canmove(s, snake_down))
			*move = snake_down;
	}
	return r;
}

static void draw(struct snake *sn, int max_score);

int mainloop(struct snake *sn)
{
	static int max_score, paused;
	static void *move;

	if (input(&move, &paused, sn))
		return -1;
	if (!paused) {
		if (snake(sn, move))
			return 0;
		if (sn->score > max_score)
			max_score = sn->score;
		move = NULL;
		draw(sn, max_score);
	}
	return 1;
}

static int vw, vh;

static void set(int x, int y, int c, int fg, int bg)
{
	int dx, dy;

	dx = tb_width()/2 - vw/2 + x;
	dy = tb_height()/2 - vh/2 + y;
	tb_change_cell(dx, dy, c, fg, bg);
}

static void drawlist(struct snakeblock *listp, int ch, int fg, int bg)
{
	struct snakeblock *p;

	for (p = listp; p != NULL; p = p->next) {
		set(p->x*2 + 0, p->y, ch, fg, bg);
		set(p->x*2 + 1, p->y, ch, fg, bg);
	}
}

void draw(struct snake *sn, int max_score)
{
	char score[256];
	int x, y;

	vw = sn->width * 2;
	vh = sn->height + 2;
	tb_clear();
	for (y = 0; y < vh-2; y++)
		for (x = 0; x < vw; x++)
			set(x, y, ' ', 0, TB_BLACK);
	sprintf(score, "Score: %d  Record: %d", sn->score, max_score);
	for (x = 0; x < vw && score[x] != '\0'; x++)
		set(x, vh - 1, score[x], 0, 0);
	drawlist(sn->wall, ' ', 0, TB_WHITE);
	drawlist(sn->snake, ' ', 0, TB_GREEN);
	drawlist(sn->food, ' ', 0, TB_YELLOW);
	tb_present();
}
