#include "engine.h"

int totalmillis = 0,
    lastmillis = 0,
    curtime = 0;

void quit(int code)
{
	SDL_Quit();
	exit(code);
}

void fatal(const char *msg)
{
	printf("fatal error: %s\n", msg);
	quit(1);
}

SDL_Surface *screen = NULL;
int scr_w = 1024, scr_h = 768;
bool fullscreen = 0;

#ifdef WIN32
#define SDLFLAGS SDL_OPENGL
#else
#define SDLFLAGS SDL_OPENGL|SDL_RESIZABLE
#endif

void processinput()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				quit();
				break;
			case SDL_VIDEORESIZE:
			{
				#ifdef WIN32
				break;
				#endif

				int w = clamp(320, 2048, event.resize.w);
				int h = clamp(240, 1536, event.resize.h);
				SDL_Surface *s = SDL_SetVideoMode(w, h, 0, SDLFLAGS| (fullscreen ? SDL_FULLSCREEN : 0));
				if(!s) break;

				SDL_FreeSurface(screen);
				screen = s;
				scr_w = s->w;
				scr_h = s->h;
				glViewport(0, 0, scr_w, scr_h);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0, scr_w, scr_h, 0, -1, 1);
			}
			break;
			case SDL_KEYDOWN:
			//case SDL_KEYUP: // - not used at all
				switch(event.key.keysym.sym)
				{
					case SDLK_F4:
						if(event.key.keysym.mod & KMOD_ALT) {quit(); break;}
					default:
					if(!guiinput(event) && !gameinput(event)); //not a mistake
					break;
				}
				break;

		}
	}
}

int main(int argc, char *argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] != '-') continue;
		switch(argv[i][1])
		{
			case 'h':
				scr_h = strtol(argv[i] + 2, NULL, 0);
				break;
			case 'w':
				scr_w = strtol(argv[i] + 2, NULL, 0);
				break;
			case 'f':
				fullscreen = true;
				break;
		}
	}

	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO)< 0) fatal("failed to initialise SDL");
	SDL_WM_SetCaption("Roguey", NULL);
	screen = SDL_SetVideoMode(scr_w, scr_h, 0, SDLFLAGS | (fullscreen ? SDL_FULLSCREEN : 0));
	if(!screen) fatal("failed to create context");
	SDL_EnableKeyRepeat(0, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(0);

	notexture = loadtexture("data/notexture.png");
	if(!notexture) fatal("failed to load backup texture");
	font = loadtexture("data/font.png", true);
	if(font == notexture) fatal("failed to load font");

	generateworld();
	spawnmonsters();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, scr_w, scr_h, 0, -1, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	totalmillis = SDL_GetTicks();
	for(;;)
	{
		curtime = SDL_GetTicks() - totalmillis;
		if(curtime < 10)
		{
			SDL_Delay(10 - curtime);
			curtime = SDL_GetTicks() - totalmillis;
		}
		totalmillis += curtime;
		//manage curtime here
		lastmillis += curtime;

		glClear(GL_COLOR_BUFFER_BIT);
		processinput();
		if(mainmenu)
		{
			renderfakeworld();
			drawmainmenu();
		}
		else
		{
			update();
			renderframe();
		}

		SDL_GL_SwapBuffers();
	}

	return 1;
}
