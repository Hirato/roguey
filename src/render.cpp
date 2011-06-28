#include "engine.h"

void renderframe()
{
	renderworld();

	drawpanel();
	drawconsole();

	glPushMatrix();
	glTranslatef(scr_w * .99 - 96, scr_h * .99 - 24, 0);
	glScalef(.35, .35, .35);

	glPopMatrix();
}
