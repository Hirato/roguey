#include "engine.h"

hashtable<const char *, texture> textures;

texture *notexture = NULL;

texture *texturedata(const char *path)
{
	texture *t = textures.access(path);
	if(t) return t;

	SDL_Surface *s = IMG_Load(path);

	if(s && s->format->BytesPerPixel != 3 && s->format->BytesPerPixel != 4)
	{
		SDL_FreeSurface(s);
		s = NULL;
	}
	if(!s)
	{
		printf("failed to load surface %s\n", path);
		return notexture;
	}

	const char *p = newstring(path);
	t = &textures.access(p, texture());
	t->name = p;
	t->s = s;

	return t;
}

void uploadtexture(texture *t, bool smooth)
{
	glGenTextures(1, &t->id);
	glBindTexture(GL_TEXTURE_2D, t->id);
	int bpp = t->s->format->BytesPerPixel;
	glTexImage2D(GL_TEXTURE_2D, 0, bpp, t->s->w, t->s->h, 0, (bpp == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, (unsigned char *)t->s->pixels);
	//we require pixel perfectness
	if(smooth)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

texture *loadtexture(const char *path, bool smooth)
{
	texture *t = textures.access(path);
	if(t) return t;

	t = texturedata(path);
	if(t != notexture)
		uploadtexture(t, smooth);
	return t;
}

void settexture(texture *t)
{
	glBindTexture(GL_TEXTURE_2D, t->id);
}

void settexture(const char *path, bool smooth)
{
	texture *t = loadtexture(path, smooth);
	glBindTexture(GL_TEXTURE_2D, t->id);
}