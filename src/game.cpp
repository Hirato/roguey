#include "engine.h"

vector<entity *> ents;
vector<creature *> turnqueue;
action *interpolate = NULL;

creature *player = NULL;
int score = 0;

vector<monster *> critters;
void initmonsters()
{
	#define CRITTER(name, str, dex, end) \
		critters.add(new monster(name, "data/" name ".png", str, dex, end));

	CRITTER("newt", 3, 7, 4);
	CRITTER("rogue", 4, 5, 5);
	CRITTER("unicorn", 6, 11, 14);
	CRITTER("kobold", 5, 5, 4);
	CRITTER("native", 5, 4, 6);
	CRITTER("fairy", 2, 8, 4);
	CRITTER("cat", 3, 6, 5);
	CRITTER("dog", 4, 5, 6);
	CRITTER("tiger", 6, 9, 8);
	CRITTER("ninja", 8, 15, 8);
	CRITTER("rat", 2, 3, 2);
	CRITTER("bat", 2, 4, 2);
	CRITTER("wolf", 5, 8, 7);

	#undef CRITTER
}

void findfreetile(int &x, int &y)
{
	x = -1, y = -1;
	while(!emptytile(x, y))
	{
		x = rnd(WORLD_W);
		y = rnd(WORLD_H);
	}
}

void spawnmonsters()
{
	if(!critters.length()) initmonsters();
	loopi(30)
	{
		entity *ent;
		ent = new monster(*critters[rnd(critters.length())]);
		findfreetile(ent->x, ent->y);
		ents.add(ent);
	}
}

void newgame()
{
	ents.deletecontents();
	turnqueue.clear();
	delete interpolate;
	interpolate = NULL;

	player = new creature("you", "data/player.png", 5, 5, 5);
	score = 0;
	findfreetile(player->x, player->y);
	ents.add(player);

	spawnmonsters();
}

void addscore(int n)
{
	if(n <= 0) return;

	int lvl = (score + n) / 100 - score / 100;
	score += n;
	loopi(lvl)
	{
		switch(rnd(3))
		{
			case 0: player->strength++; break;
			case 1: player->dexterity++; break;
			case 2: player->endurance++; player->hp += 2; break;
		}
	}
}

void queueturns()
{
	while(player->entropy < 100)
	{
		loopvi(ents)
		{
			creature *crit = dynamic_cast<creature *>(ents[i]);
			if(!crit) continue;
			crit->entropy += crit->dexterity;
		}

		int tq = 0;
		do
		{
			tq = turnqueue.length();
			loopvi(ents)
			{
				creature *crit = dynamic_cast<creature *>(ents[i]);
				if(!crit) continue;
				if(crit != player && crit->entropy > 100)
				{
					crit->entropy -= 100;
					turnqueue.add(crit);
				}
			}
		} while(tq != turnqueue.length());
	}
	player->entropy -= 100;
}

bool move(int dir, creature *d, bool check = false, creature **collision = NULL)
{
	if(collision)
		collision[0] = NULL;
	int dx, dy;
	getdir(dir, dx, dy);

	if(!emptytile(d->x + dx, d->y + dy, collision)) return false;
	if(!check)
	{
		interpolate = new action(d);
		interpolate->type = A_MOVE;
		interpolate->move.dir = dir;
	}
	return true;
}

void attack(int dir, creature *d, creature *vic)
{
	interpolate = new action(d);
	interpolate->type = A_ATTACK;
	interpolate->attack.other = vic;
	conoutf(d == player ? "%s lunge at the %s" : "the %s lunges at %s", d->name, vic->name);
}

int pending = 0;

void act(int dir, creature *d)
{
	if(pending)
	{

	}
	else //default action
	{
		creature *col = NULL;
		if(!move(dir, d, false, &col) && dir != DIR_SELF)
		{
			if(dir != DIR_SELF && col && col != player)
			{
				attack(dir, d, col);
				queueturns();
			}
		}
		else
			queueturns();

	}
}

bool gameinput(SDL_Event &event)
{
	if(!player) return false;
	switch(event.key.keysym.sym)
	{
		case SDLK_q:
			if(KMOD_CTRL & event.key.keysym.mod) {quit(); return true;}
		case SDLK_a:
			if(KMOD_CTRL & event.key.keysym.mod)
			{
				mainmenu = 4;
				turnqueue.clear();
				return true;
			}
		default: break;
	}

	if(turnqueue.length() || interpolate) return false;

	switch(event.key.keysym.sym)
	{
		case SDLK_q:
			act(DIR_ULEFT, player); return true;
		case SDLK_w:
			act(DIR_UP, player); return true;
		case SDLK_e:
			act(DIR_URIGHT, player); return true;
		case SDLK_a:
			act(DIR_LEFT, player); return true;
		case SDLK_s:
			act(DIR_SELF, player); return true;
		case SDLK_d:
			act(DIR_RIGHT, player); return true;
		case SDLK_z:
			act(DIR_DLEFT, player); return true;
		case SDLK_x:
			act(DIR_DOWN, player); return true;
		case SDLK_c:
			act(DIR_DRIGHT, player); return true;
		/*case SDLK_SPACE: // the only possible action would be seppuku; broken at present
			extern int panel;
			panel = 1;
			return true;
		*/
		default: break;
	}
	return false;
}

void update()
{
	while(interpolate || turnqueue.length())
	{
		if(interpolate && lastmillis < interpolate->startmillis + interpolate->duration())
			return;

		if(!interpolate && turnqueue.length())
		{
			interpolate = turnqueue.remove(0)->update();
			continue;
		}

		if(!interpolate) continue;

		switch(interpolate->type)
		{
			case A_MOVE:
			{
				int dx, dy;
				getdir(interpolate->move.dir, dx, dy);
				interpolate->owner->x += dx;
				interpolate->owner->y += dy;

				checktiles(interpolate->owner);
				break;
			}
			case A_ATTACK:
			{
				int dmg = (75 + rnd(51)) / 100.0f * interpolate->owner->strength;
				bool hit = rnd(2 + max(0, interpolate->owner->dexterity - interpolate->attack.other->dexterity)) != 0;

				if(interpolate->owner == player)
				{
					if(hit)
						conoutf("you hit the %s for %i damage!", interpolate->attack.other->name, dmg);
					else
						conoutf("you missed!");
				}
				else if(hit)
					conoutf("%s hits %s%s for %i damage",
						interpolate->owner->name,
						interpolate->attack.other == player ? "" : "the ",
						interpolate->attack.other->name,
						dmg
					);
				else
					conoutf("the %s misses", interpolate->owner->name);

				if(hit && (interpolate->attack.other->hp -= dmg) <= 0)
				{
					interpolate->attack.other->state = ES_DEAD;
					conoutf(interpolate->attack.other == player ? "%s have been killed by %s%s" : "the %s has been killed by %s%s",
						interpolate->attack.other->name,
						interpolate->owner == player ? "" : "the ",
						interpolate->owner->name
					);

					if(interpolate->attack.other == player)
					{
						mainmenu = 4;
						break;
					}
					if(interpolate->owner == player)
					{
						addscore(10);
					}

					ents.removeobject(interpolate->attack.other);
					turnqueue.removeobject(interpolate->attack.other);
					delete interpolate->attack.other;
				}

				break;
			}
		}
		delete interpolate;
		interpolate = NULL;
	}
}

action *monster::update()
{
	if(state != ES_ALIVE) return NULL;

	action *act = new action(this);
	//temp dumb AI
	int dir = rnd(DIR_NUM);
	creature *collision = NULL;
	if(move(dir, this, true, &collision))
	{
		act->type = A_MOVE;
		act->move.dir = dir;
	}
	else if(collision && collision != this)
	{
		act->type = A_ATTACK;
		act->attack.other = collision;
	}
	else
	{
		delete act;
		act = NULL;
	}
	//end temp
	return act;
}

void getdir(int dir, int &x, int &y)
{
	//up is - down is +; left and right is as expected
	//the view port is flipped to avoid rendering textures upside down
	switch(dir)
	{
		case DIR_UP:
			x = 0; y = -1; break;
		case DIR_URIGHT:
			x = 1; y = -1; break;
		case DIR_RIGHT:
			x = 1; y = 0; break;
		case DIR_DRIGHT:
			x = y = 1; break;
		case DIR_DOWN:
			x = 0; y = 1; break;
		case DIR_DLEFT:
			x = -1; y = 1; break;
		case DIR_LEFT:
			x = -1; y = 0; break;
		case DIR_ULEFT:
			x = y = -1; break;
		case DIR_SELF:
		default:
			x = y = 0; break;
	}
}