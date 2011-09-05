#include "ctrltnight.c"
#include "ctrltday.c"
#include "ctrltcheck.c"
#include "ctrltdefence.c"
void ctrl_t_callback(int selected)
{
	char buf[200];
	switch(RINFO.status)
	{
		case INROOM_STOP:
			break;
		case INROOM_NIGHT:
			ctrl_t_night();
			break;
		case INROOM_DAY:
			ctrl_t_day();
			break;
		case INROOM_CHECK:
			ctrl_t_check(selected);
			break;
		case INROOM_DEFENCE:
			ctrl_t_defence();
			break;
		case INROOM_VOTE:
			if ( RINFO.voted[mypos]!=-1
					&& PINFO(mypos).vote==-1 )
			{
				sprintf (buf, "\x1b[32;1m你投票给 %d %s\33[m", RINFO.voted[mypos]+1,PINFO(RINFO.voted[mypos]).nick);
				send_msg (mypos, buf);
				PINFO(mypos).vote=RINFO.voted[mypos];
				vote_result();
				kill_msg(-1);
			}
			break;
		case INROOM_DARK:
			if (RINFO.victim==mypos)
			{
				sprintf (buf, "\x1b[35;1m%d %s\x1b[m: \33[31;1mOver.\33[m", mypos + 1, PINFO(mypos).nick);
				send_msg (-1, buf);
				sprintf (buf, "那么，请\33[35;1m %d %s \33[m朋友以旁观者的身份，继续游戏...", mypos + 1, PINFO(mypos).nick);
				send_msg (-1, buf);
				PINFO(mypos).flag&=~PEOPLE_ALIVE;
				goto_night();
				kill_msg(-1);
			}
			break;
	}
} 


