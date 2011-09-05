/* ----------------------------------- */
/* pip.c  ��С����ʽ                   */
/* ԭ����: dsyan   ��д��: fennet      */
/* ͼͼ by tiball.bbs@bbs.nhctc.edu.tw */
/* ----------------------------------- */

#include "pip.h"
struct chicken d;
time_t start_time;
time_t lasttime;

void temppress(const char *s){
	move(23,0);clrtoeol();
	prints("%s",s);igetkey();
    return;
}

/*��Ϸ����ʽ*/
//int p_pipple()
int
mod_default()
{
	int pipkey;
	char genbuf[200];

#ifdef MAPLE
	setutmpmode(CHICKEN);
	more("src/maple/pipgame/pip.welcome", true);
#else
	modify_user_mode(CHICKEN);
	refresh();
	move(1, 0);
	clrtobot();
#endif				// END MAPLE
	showtitle("������С��", BBS_FULL_NAME);
	srandom(time(0));
#ifdef MAPLE
	sprintf(genbuf, "home/%s/new_chicken", getCurrentUser()->userid);
#else
	sprintf(genbuf, "home/%c/%s/new_chicken", toupper(getCurrentUser()->userid[0]),
		getCurrentUser()->userid);
#endif				// END MAPLE

	pip_read_file();
	if (!dashf(genbuf)) {
		ansimore("game/pipgame/pip.welcome", false);
		pipkey = igetkey();
		if (pipkey == 'Q' || pipkey == 'q')
			return 0;
		if (d.death != 0 || !d.name[0]) {
			if (!pip_new_game())
				return 0;
		}
	} else {
		ansimore("game/pipgame/pip.welcome", false);
		pipkey = igetkey();
		if (pipkey == 'R' || pipkey == 'r')
			pip_read_backup();
		else if (pipkey == 'Q' || pipkey == 'q')
			return 0;
		if (d.death != 0 || !d.name[0]) {
			if (!pip_new_game())
				return 0;
		}
	}

	lasttime = time(0);
	start_time = time(0);
	pip_main_menu();
	d.bbtime += time(0) - start_time;
	pip_write_file();
	return 0;
}

/*ʱ���ʾ��*/
char *
dsyan_time(const time_t * t)
{
	struct tm *tp;
	static char ans[9];

	tp = localtime(t);
	sprintf(ans, "%02d/%02d/%02d", tp->tm_year % 100, tp->tm_mon + 1,
		tp->tm_mday);
	return ans;
}

/*����Ϸ���趨*/
int
pip_new_game()
{
	char buf[256];
	time_t now;
	char *pipsex[3] = { "��", "��", "��" };
	struct tm *ptime;

	ptime = localtime(&now);

	if (d.death == 1 && !(!d.name[0])) {
		clear();
		showtitle("����ս����", BBS_FULL_NAME);
		move(4, 6);
		prints("��ӭ���� \033[1;5;33m�ǿ�����Ƽ��о�Ժ\033[0m");
		move(6, 6);
		prints("�����ǵ�����ʾ  ��ǰ��������С���  ���Ǳ���������...");
		move(8, 6);
		if (d.liveagain < 4) {
			prints("���ǿ��԰����С������  ������Ҫ����һ�����");
#ifdef MAPLE
			getdata(10, 6, "��Ҫ��������������? [y/N]:", buf, 2, 1,
				0);
#else
			getdata(10,6,"��Ҫ��������������? [y/N]:",buf,2,DOECHO,NULL,true);
#endif				// END MAPLE
			if (buf[0] == 'y' || buf[0] == 'Y') {
				pip_live_again();
			}
		} else if (d.liveagain >= 4) {
			prints("�����㸴������̫�����  С�����϶��ǿ����ۼ�");
			move(10, 6);
			prints("�����Ҳ������������ĵط���  ����....");
			temppress("����������....��....");
		}
	}
	if (d.death != 0 || !d.name[0]) {
		clear();
		showtitle("����ս����", BBS_FULL_NAME);
		/*
		 * С������
		 */
		strcpy(buf, "����֮��");
#ifdef MAPLE
		getdata(2, 3, "��С��ȡ�����������ְ�(�벻Ҫ�пո�):", buf, 11,
			1, 0);
#else
		getdata(2,3,"��С��ȡ�����������ְ�(�벻Ҫ�пո�):",buf,11,DOECHO,NULL,false);
#endif				// END MAPLE
		if (!buf[0])
			return 0;
		strcpy(d.name, buf);
		/*
		 * 1:�� 2:ĸ 
		 */
#ifdef MAPLE
		getdata(4, 3, "[Boy]С������ or [Girl]Сĸ���� [b/G]", buf, 2,
			1, 0);
#else
		getdata(4,3,"[Boy]С������ or [Girl]Сĸ���� [b/G]",buf,2,DOECHO,NULL,true);
#endif				// END MAPLE
		if (buf[0] == 'b' || buf[0] == 'B') {
			d.sex = 1;
		} else {
			d.sex = 2;
		}
		move(6, 3);
		prints("�ǿ�ս��������Ϸ�ֽ�ֳ������淨");
		move(7, 3);
		prints("ѡ�н�ֻ���С��20��ʱ������Ϸ������֪С�������ķ�չ");
		move(8, 3);
		prints("ѡû�н����һֱ����С�������Ž�����Ϸ....");
		/*
		 * 1:��Ҫ��δ�� 4:Ҫ��δ�� 
		 */
#ifdef MAPLE
		getdata(9, 3, "��ϣ��С����Ϸ�Ƿ�Ҫ��20����? [Y/n]", buf, 2,
			1, 0);
#else
		getdata(9,3,"��ϣ��С����Ϸ�Ƿ�Ҫ��20����? [Y/n]",buf,2,DOECHO,NULL,true);
#endif				// END MAPLE
		if (buf[0] == 'n' || buf[0] == 'N') {
			d.wantend = 1;
		} else {
			d.wantend = 4;
		}
		/*
		 * ��ͷ����
		 */
		show_basic_pic(0);
		temppress("С����춵����ˣ���úð���....");

		/*
		 * ��ͷ�趨
		 */
		now = time(0);
		strcpy(d.birth, dsyan_time(&now));
		d.bbtime = 0;

		/*
		 * ��������
		 */
		d.year = ptime->tm_year % 100;
		d.month = ptime->tm_mon + 1;
		d.day = ptime->tm_mday;
		d.death = d.nodone = d.relation = 0;
		d.liveagain = d.dataB = d.dataC = d.dataD = d.dataE = 0;

		/*
		 * �������
		 */
		d.hp = rand() % 15 + 20;
		d.maxhp = rand() % 20 + 20;
		if (d.hp > d.maxhp)
			d.hp = d.maxhp;
		d.weight = rand() % 10 + 50;
		d.tired = d.sick = d.shit = d.wrist = 0;
		d.bodyA = d.bodyB = d.bodyC = d.bodyD = d.bodyE = 0;

		/*
		 * ���۲���
		 */
		d.social = d.family = d.hexp = d.mexp = 0;
		d.tmpA = d.tmpB = d.tmpC = d.tmpD = d.tmpE = 0;

		/*
		 * ս������
		 */
		d.mp = d.maxmp = d.attack = d.resist = d.speed = d.hskill =
		    d.mskill = d.mresist = 0;
		d.magicmode = d.fightB = d.fightC = d.fightD = d.fightE = 0;

		/*
		 * ��������
		 */
		d.weaponhead = d.weaponrhand = d.weaponlhand = d.weaponbody =
		    d.weaponfoot = 0;
		d.weaponA = d.weaponB = d.weaponC = d.weaponD = d.weaponE = 0;

		/*
		 * ��������
		 */
		d.toman = d.character = d.love = d.wisdom = d.art = d.etchics =
		    0;
		d.brave = d.homework = d.charm = d.manners = d.speech =
		    d.cookskill = 0;
		d.learnA = d.learnB = d.learnC = d.learnD = d.learnE = 0;

		/*
		 * ״̬��ֵ
		 */
		d.happy = rand() % 10 + 20;
		d.satisfy = rand() % 10 + 20;
		d.fallinlove = d.belief = d.offense = d.affect = 0;
		d.stateA = d.stateB = d.stateC = d.stateD = d.stateE = 0;

		/*
		 * ʳ�����:ʳ�� ��ʳ ҩƷ ����
		 */
		d.food = 10;
		d.medicine = d.cookie = d.bighp = 2;
		d.ginseng = d.snowgrass = d.eatC = d.eatD = d.eatE = 0;

		/*
		 * ��Ʒ����:�� ���
		 */
		d.book = d.playtool = 0;
		d.money = 1500;
		d.thingA = d.thingB = d.thingC = d.thingD = d.thingE = 0;

		/*
		 * ��ȭ����:Ӯ ��
		 */
		d.winn = d.losee = 0;

		/*
		 * �μ�����
		 */
		d.royalA = d.royalB = d.royalC = d.royalD = d.royalE = 0;
		d.royalF = d.royalG = d.royalH = d.royalI = d.royalJ = 0;
		d.seeroyalJ = 1;
		d.seeA = d.seeB = d.seeC = d.seeD = d.seeE;
		/*
		 * ������鰮��
		 */
		d.lover = 0;
		/*
		 * 0:û�� 1:ħ�� 2:���� 3:A 4:B 5:C 6:D 7:E 
		 */
		d.classA = d.classB = d.classC = d.classD = d.classE = 0;
		d.classF = d.classG = d.classH = d.classI = d.classJ = 0;
		d.classK = d.classL = d.classM = d.classN = d.classO = 0;

		d.workA = d.workB = d.workC = d.workD = d.workE = 0;
		d.workF = d.workG = d.workH = d.workI = d.workJ = 0;
		d.workK = d.workL = d.workM = d.workN = d.workO = 0;
		d.workP = d.workQ = d.workR = d.workS = d.workT = 0;
		d.workU = d.workV = d.workW = d.workX = d.workY = d.workZ = 0;
		/*
		 * ������¼
		 */
		now = time(0);
		sprintf(buf, "\033[1;36m%s %-11s����һֻ�� [%s] �� %s С�� \033[0m\n",
			Cdate(now), getCurrentUser()->userid, d.name, pipsex[d.sex]);
		pip_log_record(buf);
	}
	pip_write_file();
	return 1;
}

/*С��������ʽ*/
int pipdie(const char *msg,int mode){
	char genbuf[200];
	time_t now;

	clear();
	showtitle("������С��", BBS_FULL_NAME);
	if (mode == 1) {
		show_die_pic(1);
		temppress("����������С����");
		clear();
		showtitle("������С��", BBS_FULL_NAME);
		show_die_pic(2);
		move(14, 20);
		prints("������С��\033[1;31m%s\033[m", msg);
		temppress("�ǿհ�����....");
	} else if (mode == 2) {
		show_die_pic(3);
		temppress("������..�ұ�������.....");
	} else if (mode == 3) {
		show_die_pic(0);
		temppress("��Ϸ������..");
	}

	now = time(0);
	sprintf(genbuf, "\033[1;31m%s %-11s��С�� [%s] %s\033[m\n", Cdate(now),
		getCurrentUser()->userid, d.name, msg);
	pip_log_record(genbuf);
	pip_write_file();
    return 0;
}

/*pro:���� base:���� mode:���� mul:��Ȩ100=1 cal:�Ӽ�*/
int
count_tired(prob, base, mode, mul, cal)
int prob, base;
char *mode;
int mul;
int cal;
{
	int tiredvary = 0;
	int tm;

	/*
	 * time_t now;
	 */
	tm = (time(0) - start_time + d.bbtime) / 60 / 30;
	if (mode == "Y") {
		if (tm >= 0 && tm <= 3) {
			if (cal == 1)
				tiredvary =
				    (rand() % prob + base) * d.maxhp / (d.hp +
									0.8 *
									d.hp) *
				    120 / 100;
			else if (cal == 0)
				tiredvary = (rand() % prob + base) * 4 / 3;
		} else if (tm >= 4 && tm <= 7) {
			if (cal == 1)
				tiredvary =
				    (rand() % prob + base) * d.maxhp / (d.hp +
									0.8 *
									d.hp);
			else if (cal == 0)
				tiredvary = (rand() % prob + base) * 3 / 2;
		} else if (tm >= 8 && tm <= 10) {
			if (cal == 1)
				tiredvary =
				    (rand() % prob + base) * d.maxhp / (d.hp +
									0.8 *
									d.hp) *
				    110 / 100;
			else if (cal == 0)
				tiredvary = (rand() % prob + base) * 5 / 4;
		} else if (tm >= 11) {
			if (cal == 1)
				tiredvary =
				    (rand() % prob + base) * d.maxhp / (d.hp +
									0.8 *
									d.hp) *
				    150 / 100;
			else if (cal == 0)
				tiredvary = (rand() % prob + base) * 1;
		}
	} else if (mode == "N") {
		tiredvary = rand() % prob + base;
	}

	if (cal == 1) {
		d.tired += (tiredvary * mul / 100);
		if (d.tired > 100)
			d.tired = 100;
	} else if (cal == 0) {
		d.tired -= (tiredvary * mul / 100);
		if (d.tired <= 0) {
			d.tired = 0;
		}
	}
	tiredvary = 0;
	return 0;
}
