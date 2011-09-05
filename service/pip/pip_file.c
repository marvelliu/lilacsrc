
/*---------------------------------------------------------------------------*/
/*С�������Ķ�д��ʽ							     */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

/*��Ϸд�����뵵��*/
void pip_write_file(void){
	FILE *ff;
	char buf[200];

#ifdef MAPLE
	sprintf(buf, "home/%s/new_chicken", getCurrentUser()->userid);
#else
	sprintf(buf, "home/%c/%s/new_chicken", toupper(getCurrentUser()->userid[0]),
		getCurrentUser()->userid);
#endif				// END MAPLE

	if ((ff = fopen(buf, "w"))!=NULL) {
		fwrite(&d, sizeof (d), 1, ff);
		fclose(ff);
	}
}

/*��Ϸ�����ϳ�����*/
void pip_read_file(void){
	FILE *fs;
	char buf[200];

#ifdef MAPLE
	sprintf(buf, "home/%s/new_chicken", getCurrentUser()->userid);
#else
	sprintf(buf, "home/%c/%s/new_chicken", toupper(getCurrentUser()->userid[0]),
		getCurrentUser()->userid);
#endif				// END MAPLE
	if ((fs = fopen(buf, "r"))!=NULL) {
		fread(&d, sizeof (d), 1, fs);
		fclose(fs);
	}
}

/*��¼��pip.log��*/
void pip_log_record(char *msg){
	FILE *fs;
	fs = fopen("game/pipgame/pip.log", "a+");
	if (fs == NULL) return;
	fprintf(fs, "%s", msg);
	fclose(fs);
}

/*С�����ȴ���*/
int
pip_write_backup()
{
	char *files[4] = { "û��", "����һ", "���ȶ�", "������" };
	char buf[200], buf1[200];
	char ans[3];
	int num = 0;
	int pipkey;

	show_system_pic(21);
	pip_write_file();
	do {
		move(b_lines - 2, 0);
		clrtoeol();
		move(b_lines - 1, 0);
		clrtoeol();
		move(b_lines - 1, 1);
		prints
		    ("���� [1]����һ [2]���ȶ� [3]������ [Q]���� [1/2/3/Q]��");
		pipkey = igetkey();

		if (pipkey == '1')
			num = 1;
		else if (pipkey == '2')
			num = 2;
		else if (pipkey == '3')
			num = 3;
		else
			num = 0;

	}
	while (pipkey != 'Q' && pipkey != 'q' && num != 1 && num != 2
	       && num != 3);
	if (pipkey == 'q' || pipkey == 'Q') {
		temppress("����������Ϸ����");
		return 0;
	}
	move(b_lines - 2, 1);
	prints("���浵���Ḳ�Ǵ洢��� [%s] ��С���ĵ���ร��뿼�����...",
	       files[num]);
	sprintf(buf1, "ȷ��Ҫ����� [%s] ������ [y/N]:", files[num]);
#ifdef MAPLE
	getdata(b_lines - 1, 1, buf1, ans, 2, 1, 0);
#else
	getdata(b_lines-1,1,buf1,ans,2,DOECHO,NULL,true);
#endif				// END MAPLE
	if (ans[0] != 'y' && ans[0] != 'Y') {
		temppress("�������浵��");
		return 0;
	}

	move(b_lines - 1, 0);
	clrtobot();
	sprintf(buf1, "���� [%s] ���������", files[num]);
	temppress(buf1);
#ifdef MAPLE
	sprintf(buf, "/bin/cp home/%s/new_chicken home/%s/new_chicken.bak%d",
		getCurrentUser()->userid, getCurrentUser()->userid, num);
#else
	sprintf(buf,
		"/bin/cp home/%c/%s/new_chicken home/%c/%s/new_chicken.bak%d",
		toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid,
		toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid, num);
#endif				// END MAPLE
	system(buf);
	return 0;
}

int
pip_read_backup()
{
	char buf[200], buf1[200], buf2[200];
	char *files[4] = { "û��", "����һ", "���ȶ�", "������" };
	char ans[3];
	int pipkey;
	int num = 0;
	int ok = 0;
	FILE *fs;

	show_system_pic(22);
	do {
		move(b_lines - 2, 0);
		clrtoeol();
		move(b_lines - 1, 0);
		clrtoeol();
		move(b_lines - 1, 1);
		prints
		    ("��ȡ [1]����һ [2]���ȶ� [3]������ [Q]���� [1/2/3/Q]��");
		pipkey = igetkey();

		if (pipkey == '1')
			num = 1;
		else if (pipkey == '2')
			num = 2;
		else if (pipkey == '3')
			num = 3;
		else
			num = 0;

		if (num > 0) {
#ifdef MAPLE
			sprintf(buf, "home/%s/new_chicken.bak%d", getCurrentUser()->userid,
				num);
#else
			sprintf(buf, "home/%c/%s/new_chicken.bak%d",
				toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid, num);
#endif				// END MAPLE
			if ((fs = fopen(buf, "r")) == NULL) {
				sprintf(buf, "���� [%s] ������", files[num]);
				temppress(buf);
				ok = 0;
			} else {

				move(b_lines - 2, 1);
				prints
				    ("��ȡ�������Ḳ�������������С���ĵ���ร��뿼�����...");
				sprintf(buf,
					"ȷ��Ҫ��ȡ�� [%s] ������ [y/N]:",
					files[num]);
#ifdef MAPLE
				getdata(b_lines - 1, 1, buf, ans, 2, 1, 0);
#else
				getdata(b_lines-1,1,buf,ans,2,DOECHO,NULL,true);
#endif				// END MAPLE
				if (ans[0] != 'y' && ans[0] != 'Y') {
					temppress("�����پ���һ��...");
				} else
					ok = 1;
			}
		}
	}
	while (pipkey != 'Q' && pipkey != 'q' && ok != 1);
	if (pipkey == 'q' || pipkey == 'Q') {
		temppress("������ԭ������Ϸ");
		return 0;
	}

	move(b_lines - 1, 0);
	clrtobot();
	sprintf(buf, "��ȡ [%s] ���������", files[num]);
	temppress(buf);

#ifdef MAPLE
	sprintf(buf1, "/bin/touch home/%s/new_chicken.bak%d", getCurrentUser()->userid,
		num);
	sprintf(buf2, "/bin/cp home/%s/new_chicken.bak%d home/%s/new_chicken",
		getCurrentUser()->userid, num, getCurrentUser()->userid);
#else
	sprintf(buf1, "/bin/touch home/%c/%s/new_chicken.bak%d",
		toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid, num);
	sprintf(buf2,
		"/bin/cp home/%c/%s/new_chicken.bak%d home/%c/%s/new_chicken",
		toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid, num,
		toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid);
#endif				// END MAPLE
	system(buf1);
	system(buf2);
	pip_read_file();
	return 0;
}

int
pip_live_again()
{
	char genbuf[80];
	time_t now;
	int tm;

	tm = (d.bbtime) / 60 / 30;

	clear();
	showtitle("С������������", BBS_FULL_NAME);

	now = time(0);
	sprintf(genbuf, "\033[1;33m%s %-11s��С�� [%s����] �����ˣ�\033[m\n",
		Cdate(now), getCurrentUser()->userid, d.name);
	pip_log_record(genbuf);

	/*
	 * �����ϵ��趨
	 */
	d.death = 0;
	d.maxhp = d.maxhp * 3 / 4 + 1;
	d.hp = d.maxhp / 2 + 1;
	d.tired = 20;
	d.shit = 20;
	d.sick = 20;
	d.wrist = d.wrist * 3 / 4;
	d.weight = 45 + 10 * tm;

	/*
	 * Ǯ�������֮һ
	 */
	d.money = d.money / 5;

	/*
	 * ս��������һ��
	 */
	d.attack = d.attack * 3 / 4;
	d.resist = d.resist * 3 / 4;
	d.maxmp = d.maxmp * 3 / 4;
	d.mp = d.maxmp / 2;

	/*
	 * ��Ĳ�����
	 */
	d.happy = 0;
	d.satisfy = 0;

	/*
	 * ���ۼ���
	 */
	d.social = d.social * 3 / 4;
	d.family = d.family * 3 / 4;
	d.hexp = d.hexp * 3 / 4;
	d.mexp = d.mexp * 3 / 4;

	/*
	 * ���������
	 */
	d.weaponhead = 0;
	d.weaponrhand = 0;
	d.weaponlhand = 0;
	d.weaponbody = 0;
	d.weaponfoot = 0;

	/*
	 * ʳ��ʣһ��
	 */
	d.food = d.food / 2;
	d.medicine = d.medicine / 2;
	d.bighp = d.bighp / 2;
	d.cookie = d.cookie / 2;

	d.liveagain += 1;

	temppress("С�������ؽ��У�");
	temppress("С�����ʻָ��У�");
	temppress("С�����������У�");
	temppress("�����������С���ָ����ޣ�");
	pip_write_file();
	return 0;
}
