#include "engine.h"

int mainmenu = 1;
int mainindex = 0;
int panel = 0;
int panelindex = -1;

vector<const char*> lines;

void conoutf(const char *fmt, ...)
{
	if(lines.length() > 512)
		lines.remove(0, 256);

	va_list args;
	static char buf[512];

	va_start(args, fmt);
	vformatstring(buf, fmt, args, sizeof(buf));
	va_end(args);

	lines.add(newstring(buf));
	printf("%s\n", buf);
}

void drawbutton(const char *text, bool selected)
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);

	glBegin(GL_TRIANGLE_FAN);
	float shade = selected ? .75 : .5;
	glColor4f(shade, shade, shade, shade);

	glVertex2i(0, 0);
	glVertex2i(scr_w * .4, 0);
	glVertex2i(scr_w * .4, scr_h * .2);
	glVertex2i(0, scr_h * .2);

	glEnd();

	glBegin(GL_LINE_LOOP);
	shade = selected ? 1 : .75;
	glColor3f(shade, shade, shade);

	glVertex2i(0, 0);
	glVertex2i(scr_w * .4, 0);
	glVertex2i(scr_w * .4, scr_h * .2);
	glVertex2i(0, scr_h * .2);

	glEnd();
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glScalef(scr_h * .1 / 64, scr_h * .1 / 64, 1);
	drawstring(text, 10, 32, .4 * scr_w - 20, 0, shade, shade, shade);
	glPopMatrix();
}

struct highscore
{
	const char *player;
	int score;
};

highscore scores[10] = {
	{newstring("Anne"), 100},
	{newstring("Bob"), 90},
	{newstring("Bill"), 80},
	{newstring("Richard"), 70},
	{newstring("Sue"), 60},
	{newstring("Lill"), 50},
	{newstring("Robert"), 40},
	{newstring("Paula"), 30},
	{newstring("Jane"), 20},
	{newstring("Jake"), 10}
};

int comparehs(highscore *a, highscore *b)
{
	if(a->score < b->score) return 1;
	if(a->score > b->score) return -1;
	return 0;
}

void addhighscore(char *player, int result)
{
	highscore score = {newstring(player), result};
	if(comparehs(scores + 9, &score) == 1)
	{
		delete[] scores[9].player;
		scores[9] = score;
		qsort(scores, 10, sizeof(highscore), (int (*)(const void *,const void *)) comparehs);
	}
}

static vector<char> name;

void drawmainmenu()
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 0, 0, .5);
	glBegin(GL_TRIANGLE_FAN);

	glVertex2i(0, 0);
	glVertex2i(scr_w, 0);
	glVertex2i(scr_w, scr_h);
	glVertex2i(0, scr_h);

	glEnd();
	glEnable(GL_TEXTURE_2D);

	if(mainmenu == 4)
	{
		glPushMatrix();
		glTranslatef(scr_w * .2, scr_h * .2, 0);
		glScalef(.5, .5, .5);

		drawstring("You have perished in the labyrinth...\nWhat is your name hero?", 0, 0 , scr_w * 1.6, 128);

		name.add('_'); name.add('\0');
		drawstring(name.buf, 0, 192, scr_w * 1.6, 0);
		name.pop(); name.pop();

		defformatstring(str)("Score: %i", score);
		drawstring(str, 0, 280, scr_w * 1.6, 0);

		glPopMatrix();
	}
	else if(mainmenu == 3)
	{
		glPushMatrix();
		float scale = min(scr_w / 1024.f, scr_h / 768.f) * .9;
		glTranslatef( (scr_w - 1024 * scale) / 2, (scr_h - 768 * scale) / 2, 0);
		glScalef(scale, scale, 1);

		drawstring("Player", 128, 0, 512, 0);
		drawstring("Score", 704, 0, 256, 0);

		loopi(10)
		{
			static string str;
			formatstring(str)("%i.", i + 1);
			drawstring(str, 32, 80 + i * 68, 80, 0);

			drawstring(scores[i].player, 128, 80 + i * 68, 512, 0);
			formatstring(str)("%i", scores[i].score);
			drawstring(str, 704, 80 + 68 * i, 256, 0);

		}
		glPopMatrix();
	}
	else if(mainmenu == 2)
	{
		settexture("data/help.png", true);
		glColor4f(1, 1, 1, 1);
		glBegin(GL_TRIANGLE_FAN);

		glTexCoord2i(0,0); glVertex2i(scr_w * .5 - 256, scr_h * .5 - 256);
		glTexCoord2i(1,0); glVertex2i(scr_w * .5 + 256, scr_h * .5 - 256);
		glTexCoord2i(1,1); glVertex2i(scr_w * .5 + 256, scr_h * .5 + 256);
		glTexCoord2i(0,1); glVertex2i(scr_w * .5 - 256, scr_h * .5 + 256);

		glEnd();
	}
	else
	{
		glPushMatrix();

		glTranslatef(scr_w * 0.3, scr_h * .3, 0);
		drawbutton("New Game", mainindex == 0);

		glTranslatef(0, scr_h * .22, 0);
		drawbutton("High Score", mainindex == 1);


		glTranslatef(0, scr_h * .22, 0);
		drawbutton("Quit", mainindex == 2);

		glPopMatrix();
	}
}

void drawpanel()
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);

	glColor3f(0, .75, 1);
	glVertex2i(scr_w * .8 + 5, scr_h * .25);
	glVertex2i(scr_w  - 5, scr_h * .25);
	glVertex2i(scr_w - 5, scr_h - 5);
	glVertex2i(scr_w * .8 + 5, scr_h - 5);

	glEnd();
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(scr_w * .8, scr_h * .25, 0);
	glScalef(.5, .5, .5);
	string buf;

	switch(panel)
	{
		case 0:
			formatstring(buf)("Str:\t  %i", player->strength);
			drawstring(buf, 20, 10, scr_w * .4 - 40, 0);

			formatstring(buf)("Dex:\t  %i", player->dexterity);
			drawstring(buf, 20, 10 + 80, scr_w * .4 - 40, 0);

			formatstring(buf)("End:\t  %i", player->endurance);
			drawstring(buf, 20, 10 + 160, scr_w * .4 - 40, 0);

			formatstring(buf)("HP:\t  %i/%i", player->hp, player->endurance * 2);
			drawstring(buf, 20, 10 + 320, scr_w * .4 - 40, 0);

			formatstring(buf)("Dmg:  %i-%i", int(player->strength * .75), int(player->strength * 1.25));
			drawstring(buf, 20, 10 + 400, scr_w * .4 - 40, 0);

			formatstring(buf)("Score: %i", score);
			drawstring(buf, 20, 10 + 480, scr_w * .4 - 40, 0);

			if(turnqueue.length())
			{
				formatstring(buf)("Pending...\n%i moves", turnqueue.length());
				drawstring(buf, 20, 10 + 640, scr_w * .4, 128);
			}
			else
			{

			}
			break;
		case 1:
			drawstring("actions", scr_w * 0.8 + 10, scr_h * 0.5, scr_w * .2 - 20, scr_h * .2);
			break;
		case 2:
			drawstring("instructions", scr_w * 0.8 + 10, scr_h * 0.5, scr_w * .2 - 20, scr_h * .2);
			break;
	}
	glPopMatrix();
}

void drawconsole()
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);

	glColor3f(0, .75, 1);
	glVertex2i(5, 5);
	glVertex2i(scr_w - 5, 5);
	glVertex2i(scr_w - 5, scr_h * .25 - 5);
	glVertex2i(5, scr_h * .25 - 5);

	glEnd();
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(scr_w * .01 + 5, scr_h * .01 + 5, 0);
	glScalef(.5, .5, .5);

	int amnt = min<int>((scr_h * .23 - 10) / .5 / 64, lines.length());

	loopi(amnt)
	{
		const char *line;
		if(amnt < lines.length())
			line = lines[lines.length() - amnt + i];
		else
			line = lines[i];

		drawstring(line, 0, 64 * i, (scr_w * .99 - 10) / .5, 1);
	}

	glPopMatrix();
}

bool guiinput(SDL_Event &event)
{
	if(!mainmenu && !panel) return false;
	if(event.type == SDL_KEYUP) return true;

	int key = event.key.keysym.sym;

	if(mainmenu)
	{
		if(mainmenu == 4)
		{
			if(key >= SDLK_a && key <= SDLK_z && event.key.keysym.mod & KMOD_SHIFT)
				name.add('A' + key - SDLK_a);
			else if(key >= SDLK_a && key <= SDLK_z)
				name.add('a' + key - SDLK_a);
			else if(key == SDLK_BACKSPACE && name.length())
				name.pop();
			else if(key == SDLK_RETURN)
			{
				mainmenu = 3;
				name.add('\0');
				addhighscore(name.buf, score);
				name.pop();
			}
			return true;
		}
		if(mainmenu == 3)
		{
			mainmenu = 1;
			return true;
		}
		if(mainmenu == 2)
		{
			mainmenu = 0;
			generateworld();
			conoutf("You have been hired to curb the beasts within the Dungeons.");
			conoutf("So what are you waiting for, \"hero\", go smash some skulls in!");
			newgame();
			return true;
		}
		switch(key)
		{
			case SDLK_UP:
				mainindex = clamp(0, 2, --mainindex);
				break;
			case SDLK_DOWN:
				mainindex = clamp(0, 2, ++mainindex);
				break;
			case SDLK_RETURN:
			case SDLK_SPACE:
				switch(mainindex)
				{
					case 0:
						mainmenu = 2;
						break;
					case 1:
						mainmenu = 3;
						break;
					case 2:
						quit();
						break;
				}
				break;
		}
	}
	else if(panel)
	{

	}

	return true;
}