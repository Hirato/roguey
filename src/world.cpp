#include "engine.h"

enum
{
	T_SOLID = 1 << 0,
	T_TRAP = 1 << 1,
	T_EXIT = 1 << 2
};

struct tile
{
	texture *tex;
	int flags;

	tile(texture *tex, int flags) : tex(tex), flags(flags) {}
	~tile() {}
};

vector<tile *> tiles;

void initworld()
{
	#define LOAD(tex, flags) \
		tiles.add(new tile(loadtexture("data/" tex), flags))

	LOAD("solid.png", T_SOLID);
	LOAD("empty.png", 0);
	LOAD("exit.png", T_EXIT);

	#undef LOAD
}

int world[WORLD_W][WORLD_H];

bool emptytile(int x, int y, creature **collision)
{
	if(x < 0 || y < 0 || x >= WORLD_W || y >= WORLD_H) return false;
	if(tiles[world[x][y]]->flags & T_SOLID) return false;
	loopvi(ents)
	{
		creature *crit = dynamic_cast<creature *>(ents[i]);
		if(crit && crit->x == x && crit->y == y)
		{
			if(collision) collision[0] = crit;
			return false;
		}
	}
	return true;
}

void checktiles(creature *d)
{
	tile *t = tiles[world[d->x][d->y]];
	if(t->flags & T_TRAP)
	{
		//dodamage
	}
	if(d == player && t->flags & T_EXIT)
	{
		turnqueue.clear();
		ents.removeobject(player);
		ents.deletecontents();
		generateworld();
		spawnmonsters();
		findfreetile(player->x, player->y);
		ents.add(player);

		conoutf("You have decended deeper into the labyrinth.");
		addscore(20);
		player->hp = player->endurance * 2;
	}
}

void generateworld()
{
	if(!tiles.length()) initworld();
	loopi(WORLD_W) loopj(WORLD_H) world[i][j] = 0;

	int x = rnd(WORLD_W),
		y = rnd(WORLD_H);

	int i = 0;

	do
	{
		world[x][y] = rnd(tiles.length() - 2) + 1;

		vector<int> candidates;

		loopi(DIR_NUM)
		{
			int dx, dy;
			getdir(i, dx, dy);
			if((x + dx) < WORLD_W && (x + dx) >= 0
				&& (y + dy) < WORLD_H && (y + dy) >= 0
				&& world[x + dx][y + dy] == 0
			)
				candidates.add(i);
		}

		if(!candidates.length())
		{
			loopi(DIR_NUM)
			{
				int dx, dy;
				getdir(i, dx, dy);
				if((x + dx) < WORLD_W && (x + dx) >= 0
					&& (y + dy) < WORLD_H && (y + dy) >= 0
				)
					candidates.add(i);
			}
		}
		if(!candidates.length()) continue;
		int dx, dy;
		getdir(candidates[rnd(candidates.length())], dx, dy);
		x += dx; y += dy;
	} while(i++ < 700 || rnd(500));
	world[x][y] = tiles.length() - 1; //assumed the last one is exit
}

//used for the main menu's background
void renderfakeworld()
{
	if(!tiles.length()) initworld();

	int herow = cos(lastmillis / 8000.f) * 20 * WORLD_W + 32 * WORLD_W;
	int heroh = sin(lastmillis / 8000.f) * 20 * WORLD_H + 32 * WORLD_H;

	glPushMatrix();
	glTranslatef(
		-clamp<int>(0, WORLD_W * 64 - scr_w, herow - scr_w / 2 ),
		-clamp<int>(0, WORLD_H * 64 - scr_h, heroh - scr_h / 2 ),
		0
	);
	glColor3f(1, 1, 1);

	loopi(WORLD_W) loopj(WORLD_H)
	{
		settexture(tiles[world[i][j]]->tex);
		glBegin(GL_TRIANGLE_STRIP);

		glTexCoord2i(0, 0); glVertex2i(64 * i, 64 * j);
		glTexCoord2i(1, 0); glVertex2i(64 * i + 64, 64 * j);
		glTexCoord2i(0, 1); glVertex2i(64 * i, 64 * j + 64);
		glTexCoord2i(1, 1); glVertex2i(64 * i + 64, 64 * j + 64);

		glEnd();
	}
	loopvi(ents)
	{
		settexture(ents[i]->tex);

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2i(0, 0); glVertex2i(64 * ents[i]->x, 64 * ents[i]->y);
		glTexCoord2i(1, 0); glVertex2i(64 * ents[i]->x + 64, 64 * ents[i]->y);
		glTexCoord2i(0, 1); glVertex2i(64 * ents[i]->x, 64 * ents[i]->y + 64);
		glTexCoord2i(1, 1); glVertex2i(64 * ents[i]->x + 64, 64 * ents[i]->y + 64);
		glEnd();
	}

	glPopMatrix();
}

void renderworld()
{
	int herow = 0;
	int heroh = 0;
	if(interpolate && interpolate->owner == player)
	{
		if(interpolate->type == A_MOVE)
		{
			getdir(interpolate->move.dir, herow, heroh);
			herow *= 32 * interpolate->progress(); heroh *= 32 * interpolate->progress();
		}
		herow += interpolate->owner->x * 32;
		heroh += interpolate->owner->y * 32;
	}
	else
	{
		herow = player->x * 32;
		heroh = player->y * 32;
	}

	if(!tiles.length()) initworld();
	glPushMatrix();
	glEnable(GL_SCISSOR_TEST);

	int woffset = 0, hoffset = 0;
	if(scr_w * .8 < WORLD_W * 32)
		woffset = clamp<int>(0, WORLD_W * 32 - scr_w * .8, 16 + herow - scr_w * .4);
	if(scr_h * .75 < WORLD_H * 32)
		hoffset = clamp<int>(0, WORLD_H * 32 - scr_h * .75, 16 + heroh - scr_h * .375);

	glTranslatef(-woffset, scr_h * .25 - hoffset, 0);
	glScissor(0, 0, scr_w * .8, scr_h * .75);
	glColor3f(1, 1, 1);

	loopi(WORLD_W) loopj(WORLD_H)
	{
		settexture(tiles[world[i][j]]->tex);
		glBegin(GL_TRIANGLE_FAN);

		glTexCoord2i(0, 0); glVertex2i(32 * i, 32 * j);
		glTexCoord2i(1, 0); glVertex2i(32 * i + 32, 32 * j);
		glTexCoord2i(1, 1); glVertex2i(32 * i + 32, 32 * j + 32);
		glTexCoord2i(0, 1); glVertex2i(32 * i, 32 * j + 32);

		glEnd();
	}

	loopvi(ents)
	{
		settexture(ents[i]->tex);
		glBegin(GL_TRIANGLE_FAN);

		int posx = 0, posy = 0;
		if(interpolate && ents[i] == interpolate->owner && interpolate->type == A_MOVE)
		{
			getdir(interpolate->move.dir, posx, posy);
			posx *= 32 * interpolate->progress();
			posy *= 32 * interpolate->progress();
		}
		posx += ents[i]->x * 32;
		posy += ents[i]->y * 32;

		glTexCoord2i(0, 0); glVertex2i(posx, posy);
		glTexCoord2i(1, 0); glVertex2i(posx + 32, posy);
		glTexCoord2i(1, 1); glVertex2i(posx + 32, posy + 32);
		glTexCoord2i(0, 1); glVertex2i(posx, posy + 32);

		glEnd();
	}

	settexture("data/halo.png");
	int posx, posy;
	if(!interpolate || interpolate->owner == player)
	{
		glColor3f(0, 1, 0);
		posx = herow;
		posy = heroh;

	}
	else
	{
		glColor3f(1, 0, 0);
		posx = interpolate->owner->x * 32;
		posy = interpolate->owner->y * 32;
	}

	glTranslatef(posx + 16, posy + 16, 0);
	glPushMatrix();
	glRotatef(lastmillis / 10.f, 0, 0, -1);

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2i(0, 0); glVertex2i(-64, -64);
	glTexCoord2i(1, 0); glVertex2i(64, -64);
	glTexCoord2i(1, 1); glVertex2i(64, 64);
	glTexCoord2i(0, 1); glVertex2i(-64,64);
	glEnd();

	glPopMatrix();

	glDisable(GL_SCISSOR_TEST);
	glPopMatrix();
}