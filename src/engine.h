#include "tools.h"
#include <SDL.h>
#include <SDL_image.h>
#include <GL/gl.h>
#include <GL/glext.h>

//main
extern int lastmillis, totalmillis, curtime;
extern int scr_w, scr_h;
extern void quit(int code = 0);

//textures
struct texture
{
	const char *name;
	SDL_Surface *s;
	GLuint id;

	texture() : name(NULL), s(NULL), id(0) {}
	~texture()
	{
		delete[] name;
		if(s) SDL_FreeSurface(s);
		glDeleteTextures(1, &id);
	}
};

extern texture *notexture;

extern texture *loadtexture(const char *path, bool smooth = false);
extern void settexture(const char *path, bool smooth = false);
extern void settexture(texture *t);

//game
enum
{
	ES_ALIVE = 0,
	ES_DEAD,
	ES_INANIMATE
};
struct action;
struct entity
{
	int state;
	texture *tex;
	int x, y;

	entity(texture *tex) : state(ES_INANIMATE), tex(tex), x(0), y(0) {}
	virtual ~entity() {}

	virtual action *update()=0;
};

struct creature : entity
{
	//atm read, damage, turns per 100 iterations and max hp
	int strength, dexterity, endurance;
	int hp;
	const char *name;

	int entropy;

	action *update()
	{
		return NULL;
	}

	creature(const char *n, const char *tex, int str, int dex, int end) : entity(loadtexture(tex, true)), strength(str), dexterity(dex), endurance(end), name(n), entropy(0)
	{
		hp = endurance * 2;
		state = ES_ALIVE;
	}
	~creature() {}
};

struct monster : creature
{
	action *update();

	monster(const char *n, const char *tex, int str, int dex, int end) : creature(n, tex, str, dex, end) {}
};

enum
{
	DIR_SELF = 0,
	DIR_UP,
	DIR_URIGHT,
	DIR_RIGHT,
	DIR_DRIGHT,
	DIR_DOWN,
	DIR_DLEFT,
	DIR_LEFT,
	DIR_ULEFT,
	DIR_NUM
};

extern vector<entity *> ents;
extern vector<creature *> turnqueue;
extern int score;
extern creature *player;
extern action *interpolate;

enum
{
	A_NONE = 0,
	A_MOVE,
	A_ATTACK,
};

struct action
{
	creature *owner;
	int type;
	int startmillis;

	int duration()
	{
		switch(type)
		{
			case A_MOVE: return owner == player ? 200 : 0;
			case A_ATTACK: return 500;
		}
		return 0;
	}
	float progress() {
		return min(1.0f, (float) (lastmillis - startmillis) / (duration() ? duration() : 1));
	}

	union
	{
		struct { int dir; } move;
		struct { creature *other; } attack;
	};

	action(creature *owner) : owner(owner), startmillis(lastmillis) {}
	~action() {}
};

extern void newgame();
extern void addscore(int n);
extern void spawnmonsters();
extern void getdir(int dir, int &x, int &y);
extern void findfreetile(int &x, int &y);
extern bool gameinput(SDL_Event &event);
extern void update();

//render
extern void renderframe();

//render text
extern texture *font;
extern void drawstring(const char *str, int x, int y, int dx, int dy, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

//menu
extern void conoutf(const char *fmt, ...);
extern int mainmenu;
extern void drawmainmenu();
extern void drawpanel();
extern void drawconsole();
extern bool guiinput(SDL_Event &event);

//world
#define WORLD_H 40
#define WORLD_W 65

extern void generateworld();
extern bool emptytile(int x, int y, creature **collision = NULL);
extern void checktiles(creature *d);
extern void renderworld();
extern void renderfakeworld();
