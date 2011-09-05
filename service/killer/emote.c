void emote_callback (int me, char *msg, char * buf)
{
	int i,k;
	char buf2[220], buf3[220];
	i = 2;
	while (msg[i] != ' ' && i < strlen(msg))
			i++;
	strcpy(buf, msg + 2);
	buf[i - 2] = 0;
	while (msg[i] == ' ' && i < strlen(msg))
			i++;
	buf2[0] = 0;
	buf3[0] = 0;
	if (msg[i - 1] == ' ' && i < strlen(msg))
	{
		k = i;
		while (msg[k] != ' ' && k < strlen(msg))
			k++;
		strcpy(buf2, msg + i);
		buf2[k - i] = 0;
		i = k;
		while (msg[i] == ' ' && i < strlen(msg))
			i++;
		if (msg[i - 1] == ' ' && i < strlen(msg))
		{
			k = i;
			while (msg[k] != ' ' && k < strlen(msg))
				k++;
			strcpy(buf3, msg + i);
			buf3[k - i] = 0;
		}
	}
	k = 1;
	for (i = 0;; i++)
	{
		if (!party_data[i].verb)
				break;
		if (!strcasecmp(party_data[i].verb, buf))
		{
				k = 0;
				sprintf(buf, "%s \x1b[1m%s\x1b[m %s", party_data[i].part1_msg, buf2[0] ? buf2 : "´ó¼Ò", party_data[i].part2_msg);
				break;
		}
	}
	if (k)
		for (i = 0;; i++)
		{
			if (!speak_data[i].verb)
					break;
			if (!strcasecmp(speak_data[i].verb, buf))
			{
					k = 0;
					sprintf(buf, "%s: %s", speak_data[i].part1_msg, buf2);
					break;
			}
		}
		if (k)
			for (i = 0;; i++)
			{
				if (!condition_data[i].verb)
					break;
				if (!strcasecmp(condition_data[i].verb, buf)) 
				{
					k = 0;
					sprintf(buf, "%s", condition_data[i].part1_msg);
					break;
				}
			}
		if (k)
			sprintf(buf, "%s", msg);
		else
		{
			strcpy(buf2, buf);
			sprintf(buf, "\x1b[1m%d %s\x1b[m %s", me + 1, PINFO(me).nick, buf2);
		}
} 

