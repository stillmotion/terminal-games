enum {
	INVADERS_WIDTH = 38,
	INVADERS_HEIGHT = 14
};

struct invadernode {
	int v;
	int x;
	int y;
	int size;
	struct invadernode *next;
};

struct invaders {
	long score;
	int lives;
	int easiness;
	int countdown;
	int dtn;
	struct invadernode *artifacts;
	struct invadernode *ship;
	struct invadernode *invader;
	struct invadernode *barrier;
	struct invadernode *special;
	struct invadernode *ishot;
	struct invadernode *sshot;
};

int invaders_init(struct invaders *in, int score, int lives);
int invaders(struct invaders *in, int move, int shoot);
void invaders_free(struct invaders *in);

/* -------------------------------------------------------------------------- */

#include <stdlib.h>

static struct invadernode *new(int v, int x, int y, int size,
                               struct invadernode *next);

int collision(struct invadernode *p, void *ptr);
int on_column(struct invadernode *p, void *ptr);
int on_row(struct invadernode *p, void *ptr);
struct invadernode *find(struct invadernode *list, void *ptr,
                         int found(struct invadernode *p, void *ptr));

static void freelist(struct invadernode *list);

static int length(struct invadernode *list);

static void delete(struct invadernode **listp, struct invadernode *target);

static void map(struct invadernode **p, void *q,
                int f(struct invadernode *, void *));

static int update_ishot(struct invadernode *p, void *data)
{
	struct invaders *in = data;
	struct invadernode *q;

	p->y++;
	q = find(in->sshot, p, collision);
	if (q != NULL && q->v == 0) {
		in->artifacts = new(0, q->x, q->y, q->size, in->artifacts);
		delete(&in->sshot, q);
		return -1;
	}
	q = find(in->ship, p, collision);
	if (q != NULL) {
		in->artifacts = new(0, q->x, q->y, q->size, in->artifacts);
		in->lives--;
		q->x = 0;
		return -1;
	}
	q = find(in->barrier, p, collision);
	if (q != NULL) {
		q->v--;
		if (q->v < 0)
			delete(&in->barrier, q);
		return -1;
	}
	return -(p->y >= INVADERS_HEIGHT);
}

static int update_sshot(struct invadernode *p, void *data)
{
	struct invaders *in = data;
	struct invadernode *q;

	p->y--;
	q = find(in->invader, p, collision);
	if (q != NULL) {
		in->artifacts = new(0, q->x, q->y, q->size, in->artifacts);
		in->score++;
		delete(&in->invader, q);
		if (length(in->invader) % 11 == 0)
			in->easiness -= 1 + rand()%2;
		if (length(in->invader) == 1)
			in->easiness /= 2;
		return -1;
	}
	q = find(in->barrier, p, collision);
	if (q != NULL) {
		if (--q->v < 0)
			delete(&in->barrier, q);
		return -1;
	}
	q = find(in->special, p, collision);
	if (q != NULL) {
		in->score += 5;
		in->artifacts = new(0, q->x, q->y, q->size, in->artifacts);
		delete(&in->special, q);
		return -1;
	}
	return -(p->y < 0);
}

static int update_special(struct invadernode *p, void *unused)
{
	p->x--;
	return -(p->x+p->size <= 0);
}

static int update_invader(struct invadernode *p, void *data)
{
	struct invaders *in = data;
	struct invadernode *q;

	if (in->dtn == 0)
		p->y++;
	else
		p->x += in->dtn;
	while ((q = find(in->barrier, p, collision)) != NULL)
		delete(&in->barrier, q);
	if (in->ishot != NULL)
		return 0;
	if (rand() / (double) RAND_MAX
	  >= length(p->next) / (double) length(in->invader))
		in->ishot = new(rand() % 2, p->x, p->y + 1, 1, NULL);
	return 0;
}

int invaders(struct invaders *in, int move, int shoot)
{
	int firstcol = 0;
	int lastcol = INVADERS_WIDTH - 1;
	int lastrow = INVADERS_HEIGHT - 1;

	freelist(in->artifacts);
	in->artifacts = NULL;
	if (shoot && (in->sshot == NULL || in->sshot->y < INVADERS_HEIGHT/2))
		in->sshot = new(0, in->ship->x + 1, in->ship->y, 1, in->sshot);
	if (move < 0 && in->ship->x > 0)
		in->ship->x--;
	if (move > 0 && in->ship->x < INVADERS_WIDTH - 1)
		in->ship->x++;
	map(&in->special, NULL, update_special);
	if (in->special == NULL && rand() % 300 == 0)
		in->special = new(0, INVADERS_WIDTH - 1, 0, 3, NULL);
	if (in->countdown <= 0) {
		in->countdown = in->easiness;
		if (in->dtn == 0)
			in->dtn = find(in->invader,&firstcol,on_column)? 1 : -1;
		else if ((in->dtn < 0 && find(in->invader,&firstcol,on_column))
		      || (in->dtn > 0 && find(in->invader,&lastcol,on_column)))
			in->dtn = 0;
		map(&in->invader, in, update_invader);
	}
	map(&in->ishot, in, update_ishot);
	map(&in->sshot, in, update_sshot);
	in->countdown--;
	if (find(in->invader, &lastrow, on_row))
		in->lives = 0;
	return (in->lives > 0 && in->invader != NULL);
}

void invaders_free(struct invaders *in)
{
	freelist(in->ship);
	freelist(in->sshot);
	freelist(in->invader);
	freelist(in->special);
	freelist(in->barrier);
	freelist(in->ishot);
}

static struct invadernode *init_invaders(void)
{
	struct invadernode *p, *lst;
	int i, x, y;

	lst = NULL;
	for (i = 0; i < 11 * 5; i++) {
		x = i % 11;
		y = i / 11;
		p = new((y + 1) / 2, x * 3, 1 + y, 2, lst);
		if (p == NULL) {
			freelist(lst);
			return NULL;
		}
		lst = p;
	}
	return lst;
}

static struct invadernode *init_barriers(void)
{
	struct invadernode *p, *lst;
	int i, j, x, y;

	lst = NULL;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 8; j++) {
			y = 11 + (j >= 3);
			x = 3 + ((j >= 3) ? j - 3 : 1 + j) + 10*i;
			if (j == 5)
				continue;
			p = new(1, x, y, 1, lst);
			if (p == NULL) {
				freelist(lst);
				return NULL;
			}
			lst = p;
		}
	return lst;
}

int invaders_init(struct invaders *in, int score, int lives)
{
	in->score = score;
	in->lives = lives;
	in->easiness = 16;
	in->countdown = 0;
	in->dtn = +1;
	in->artifacts = NULL;
	in->special = in->sshot = in->ishot = NULL;
	in->ship = new(0, 0, 13, 3, NULL);
	in->invader = init_invaders();
	in->barrier = init_barriers();
	if (in->ship == NULL || in->invader == NULL || in->barrier == NULL) {
		free(in->ship);
		freelist(in->invader);
		freelist(in->barrier);
		return -1;
	}
	return 0;
}

struct invadernode *new(int v, int x, int y, int size, struct invadernode *next)
{
	struct invadernode *p;

	p = malloc(sizeof *p);
	if (p == NULL)
		return NULL;
	p->v = v;
	p->x = x;
	p->y = y;
	p->size = size;
	p->next = next;
	return p;
}

int collision(struct invadernode *p, void *ptr)
{
	struct invadernode *q = ptr;
	int x;

	if (p->y != q->y)
		return 0;
	for (x = p->x; x < p->x+p->size; x++)
		if (x >= q->x && x < q->x+q->size)
			return 1;
	return 0;
}

int on_row(struct invadernode *p, void *ptr)
{
	int row = *(int *) ptr;

	return (p->y == row);
}

int on_column(struct invadernode *p, void *ptr)
{
	int column = *(int *) ptr;
	int x;

	for (x = p->x; x < p->x+p->size; x++)
		if (x == column)
			return 1;
	return 0;
}

struct invadernode *find(struct invadernode *list, void *ptr,
                         int found(struct invadernode *p, void *ptr))
{
	struct invadernode *p;

	for (p = list; p != NULL; p = p->next)
		if (found(p, ptr))
			return p;
	return NULL;
}

void map(struct invadernode **p, void *q, int f(struct invadernode *, void *))
{
	struct invadernode *tmp;

	while (*p != NULL)
		if (!f(*p, q))
			p = &(*p)->next;
		else {
			tmp = (*p)->next;
			free(*p);
			*p = tmp;
		}
}

void freelist(struct invadernode *list)
{
	struct invadernode *p, *next;

	for (p = list; p != NULL; p = next) {
		next = p->next;
		free(p);
	}
}

void delete(struct invadernode **listp, struct invadernode *target)
{
	struct invadernode *p;

	if (*listp == target)
		*listp = target->next;
	else for (p = *listp; p->next != NULL; p = p->next)
		if (p->next == target) {
			p->next = target->next;
			break;
		}
	free(target);
}

int length(struct invadernode *list)
{
	struct invadernode *p;
	int n;

	n = 0;
	for (p = list; p != NULL; p = p->next)
		n++;
	return n;
}

/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termbox.h>

long play(long hiscore);

int main(void)
{
	long hiscore, r;

	srand(time(NULL));
	if (tb_init()) {
		fprintf(stderr, "invaders: can't open display\n");
		return EXIT_FAILURE;
	}
	atexit(tb_shutdown);
	hiscore = 0;
	while ((r = play(hiscore)) >= 0)
		if (r > hiscore)
			hiscore = r;
	return 0;
}

int input(int *move, int *shoot, int *pause)
{
	struct tb_event ev;
	int r;

	*shoot = *move = 0;
	while ((r = tb_peek_event(&ev, 0)) > 0) {
		if (r != TB_EVENT_KEY)
			continue;
		if (ev.key == TB_KEY_CTRL_C)
			return -1;
		if (ev.key == TB_KEY_SPACE)
			*shoot = 1;
		if (ev.key == TB_KEY_ARROW_LEFT)
			*move = -1;
		if (ev.key == TB_KEY_ARROW_RIGHT)
			*move = +1;
		if (ev.ch == 'p')
			*pause = !*pause;
	}
	return 0;
}

void draw(struct invaders *in, long hiscore);

long game(struct invaders *in, long *hiscore)
{
	int move, shoot, pause;

	move = shoot = pause = 0;
	do {
		if (in->score > *hiscore)
			*hiscore = in->score;
		draw(in, *hiscore);
		usleep(1000000 / 7);
		if (input(&move, &shoot, &pause))
			return -1;
	} while (pause || invaders(in, move, shoot));
	if (in->score > *hiscore)
		*hiscore = in->score;
	return in->score;
}

long play(long hiscore)
{
	struct invaders in;
	long score = 0;
	int lives = 3;

	do {
		if (invaders_init(&in, score, lives))
			return -1;
		score = game(&in, &hiscore);
		lives = in.lives;
		invaders_free(&in);
		if (score < 0)
			return -1;
	} while (lives > 0);
	return score;
}

static void set(int x, int y, int c, int fg, int bg)
{
	int dx, dy;

	dx = tb_width()/2 - INVADERS_WIDTH/2 + x;
	dy = tb_height()/2 - (INVADERS_HEIGHT + 2)/2 + (y + 2);
	tb_change_cell(dx, dy, c, fg, bg);
}

static void drawgame(struct invaders *in)
{
	struct invadernode *p;
	int i;

	for (p = in->ship; p != NULL; p = p->next)
		for (i = 0; i < p->size; i++)
			set(p->x + i, p->y, "=^"[i == p->size/2], TB_RED, 0);
	for (p = in->invader; p != NULL; p = p->next)
		for (i = 0; i < p->size; i++)
			set(p->x + i, p->y, ":o@"[p->v], 0, 0);
	for (p = in->barrier; p != NULL; p = p->next)
		for (i = 0; i < p->size; i++)
			set(p->x + i, p->y, "# "[p->v], TB_WHITE, TB_RED);
	for (p = in->special; p != NULL; p = p->next) {
		set(p->x, p->y, '<', TB_RED, 0);
		for (i = 1; i < p->size-1; i++)
			set(p->x + i, p->y, '=', TB_RED, 0);
		set(p->x + p->size - 1, p->y, '>', TB_RED, 0);
	}
	for (p = in->ishot; p != NULL; p = p->next)
		set(p->x, p->y, "!$"[p->v], 0, 0);
	for (p = in->sshot; p != NULL; p = p->next)
		set(p->x, p->y, '|', 0, 0);
	for (p = in->artifacts; p != NULL; p = p->next)
		for (i = 0; i < p->size; i++)
			set(p->x + i, p->y, 'X', TB_RED, 0);
}

void draw(struct invaders *in, long hiscore)
{
	char s[8192];
	int i;

	tb_clear();
	sprintf(s, " SCORE        HI-SCORE       LIVES    ");
	for (i = 0; s[i] != '\0'; i++)
		set(i, -2, s[i], 0, 0);
	sprintf(s, " %06ld        %06ld     %.*s", in->score, hiscore,
	        in->lives * 4, "=^= =^= =^=");
	for (i = 0; s[i] != '\0'; i++)
		set(i, -1, s[i], 0, 0);
	drawgame(in);
	tb_present();
}
