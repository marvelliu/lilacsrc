int add_room(struct room_struct *r)
{
	int i, j;

	for (i = 0; i < MAX_ROOM; i++)
		if (rooms[i].style == 1)
		{
			if (!strcmp(rooms[i].name, r->name))
				return -1;
			if (rooms[i].op!=-1 
				&& !strcmp(inrooms[i].peoples[rooms[i].op].id, CURRENTUSER->userid))
				return -1;
		}
	for (i = 0; i < MAX_ROOM; i++)
		if (rooms[i].style == -1)
		{
			memcpy(rooms + i, r, sizeof(struct room_struct));
			inrooms[i].status = INROOM_STOP;
			inrooms[i].killernum = 0;
			inrooms[i].msgi = 0;
			inrooms[i].policenum = 0;
			inrooms[i].w = 0;
			inrooms[i].gametype = 4;
			for (j = 0; j < MAX_MSG; j++)
				inrooms[i].msgs[j][0] = 0;
			for (j = 0; j < MAX_PEOPLE; j++)
				inrooms[i].peoples[j].style = -1;
			return 0;
		}
	return -1;
}

/*
int del_room(struct room_struct * r)
{
	int i, j;
	for(i=0;i<*roomst;i++)
	if(!strcmp(rooms[i].name, r->name))
	{
		rooms[i].name[0]=0;
		break;
	}
	return 0;
}
*/

void clear_room()
{
	int i;

	for (i = 0; i < MAX_ROOM; i++)
		if ((rooms[i].style != -1) && (rooms[i].numplayer + rooms[i].numspectator == 0))
			rooms[i].style = -1;
}

int can_see(struct room_struct *r)
{
	if (r->style == -1)
		return 0;
	if ( (r->level & CURRENTUSER->userlevel) != r->level)
		return 0;
	if (r->style != 1)
		return 0;
	if ( (r->flag & ROOM_SECRET) && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
		return 0;
	return 1;
}

int can_enter(struct room_struct *r)
{
	if (r->style == -1)
		return 0;
	if ( (r->level & CURRENTUSER->userlevel) != r->level)
		return 0;
	if (r->style != 1)
		return 0;
	if (r->flag & ROOM_LOCKED && !K_HAS_PERM(CURRENTUSER, PERM_SYSOP))
		return 0;
	return 1;
}

int room_count()
{
	int i, j = 0;

	for (i = 0; i < MAX_ROOM; i++)
		if (can_see(rooms + i))
			j++;
	return j;
}

int room_get(int w)
{
	int i, j = 0;

	for (i = 0; i < MAX_ROOM; i++) {
		if (can_see(rooms + i))
		{
			if (w == j)
				return i;
			j++;
		}
	}
	return -1;
}

int find_room(char *s)
{
	int i;
	struct room_struct *r2;

	for (i = 0; i < MAX_ROOM; i++)
	{
		r2 = rooms + i;
		if (!can_enter(r2))
			continue;
		if (!strcmp(r2->name, s))
			return i;
	}
	return -1;
}

