void send_msg(int u, char *msg)
{
	int i, j, k, f;
	int maxi;
	char buf[200], buf2[200], buf3[80];

	if (strlen(msg)>199)
		msg[199]=0;

	strcpy(buf, msg);

	for (i = 0; i <= 6; i++)
	{
		buf3[0] = '%';
		buf3[1] = i + 48;
		buf3[2] = 0;
		while (strstr(buf, buf3) != NULL)
		{
			strcpy(buf2, buf);
			k = strstr(buf, buf3) - buf;
			buf2[k] = 0;
			k += 2;
			sprintf(buf, "%s\x1b[3%dm%s", buf2, (i > 0) ? i : 7, buf2 + k);
		}
	}
	for (i = 0; i <= 6; i++)
	{
		buf3[0] = '#';
		buf3[1] = i + 48;
		buf3[2] = 0;
		while (strstr(buf, buf3) != NULL)
		{
			strcpy(buf2, buf);
			k = strstr(buf, buf3) - buf;
			buf2[k] = 0;
			k += 2;
			sprintf(buf, "%s\x1b[3%d;1m%s", buf2, (i > 0) ? i : 7, buf2 + k);
		}
	}

	while (strchr(buf, '\n') != NULL)
	{
		i = strchr(buf, '\n') - buf;
		buf[i] = 0;
		send_msg(u, buf);
		strcpy(buf2, buf + i + 1);
		strcpy(buf, buf2);
	}
 
	j = 0;
	f = 0;
	for (i = 0; i < strlen(buf); i++)
	{
		if (buf[i] == '\x1b')
			f = 1;
		if (f)
		{
			if (isalpha(buf[i]))
				f = 0;
			continue;
		}
		if (j >= 54 || i>=96)
			break;
		j++;
		if (buf[i]&0x80)
		{
			i++;
			j++;
		}
	}
	maxi=i;
	if (maxi < strlen(buf) && maxi != 0)
	{
		buf2[0]=' ';
		strcpy(buf2+1,buf+maxi);
		buf[maxi] = 0;
	}
	else
		buf2[0]=0;
		   
	j = MAX_MSG;
	if (RINFO.msgs[(MAX_MSG - 1 + RINFO.msgi) % MAX_MSG][0] == 0)
		for (i = 0; i < MAX_MSG; i++)
			if (RINFO.msgs[(i + RINFO.msgi) % MAX_MSG][0] == 0)
			{
				j = (i + RINFO.msgi) % MAX_MSG;
				break;
			}
	if (j == MAX_MSG)
	{
		strcpy(RINFO.msgs[RINFO.msgi], buf);
		if (u == -1)
			RINFO.msgpid[RINFO.msgi] = -1;
		else
			RINFO.msgpid[RINFO.msgi] = PINFO(u).pid;
		RINFO.msgi = (RINFO.msgi + 1) % MAX_MSG;
	}
	else
	{
		strcpy(RINFO.msgs[j], buf);
		if (u == -1)
			RINFO.msgpid[j] = u;
		else
			RINFO.msgpid[j] = PINFO(u).pid;
	}
	
#ifdef AUTOPOST
	if (u==-1)
	{
		struct flock ldata;
		int fd;
		fd = open(ap_tmpname(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd>=0)
		{
			ldata.l_type = F_WRLCK;
			ldata.l_whence = 0;
			ldata.l_len = 0;
			ldata.l_start = 0;
			if (fcntl(fd, F_SETLKW, &ldata) != -1)
			{
				lseek(fd, 0, SEEK_END);
				write(fd, buf, strlen(buf));
				write(fd,"\n",1);
				ldata.l_type = F_UNLCK;
				fcntl(fd, F_SETLKW, &ldata);
			}
			close(fd);			
		}
	}
#endif	// AUTOPOST

	if (buf2[0])
		send_msg(u,buf2);
}

void send_msg2 (long people_type, char * msg)
{
	int i;
	for(i=0;i<MAX_PLAYER;i++)
		if (PINFO(i).style!=-1	&& (PINFO(i).flag & PEOPLE_ALIVE) && (PINFO(i).flag & people_type))
		{
			send_msg(i,msg);
			kill_msg(i);
		}
}


