/*---------------------------------------------------------------------------*/
/* ����ѡ��:��ʳ ��� ���� ��Ϣ                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

int
pip_basic_takeshower()
{				/*ϴ�� */
	int lucky;

	d.shit -= 20;
	if (d.shit < 0)
		d.shit = 0;
	d.hp -= rand() % 2 + 3;
	move(4, 0);
	lucky = rand() % 3;
	if (lucky == 0) {
		show_usual_pic(1);
		temppress("���Ǹɾ���С��  cccc....");
	} else if (lucky == 1) {
		show_usual_pic(7);
		temppress("��Ͱ �š���");
	} else {
		show_usual_pic(2);
		temppress("�Ұ�ϴ�� lalala....");
	}
	return 0;
}

int
pip_basic_takerest()
{				/*��Ϣ */
//     count_tired(5,20,"Y",100,0);
	d.tired -= rand() % 15;
	d.tired = d.tired > 0 ? d.tired : 0;

	if (d.hp > d.maxhp)
		d.hp = d.maxhp;
	d.shit += 1;
	move(4, 0);
	show_usual_pic(5);
	temppress("�ٰ�һ���Ҿ�����....");
	show_usual_pic(6);
	temppress("ιιι..������......");
	return 0;
}

int
pip_basic_kiss()
{				/*���� */
	if (rand() % 2 > 0) {
		d.happy += rand() % 3 + 4;
		d.satisfy += rand() % 2 + 1;
	} else {
		d.happy += rand() % 2 + 1;
		d.satisfy += rand() % 3 + 4;
	}
	count_tired(1, 2, "N", 100, 1);
	d.shit += rand() % 5 + 4;
	d.relation += rand() % 2;
	move(4, 0);
	show_usual_pic(3);
	if (d.shit < 60) {
		temppress("����! �һ��.....");
	} else {
		temppress("��̫��Ҳ�ǻ��������....");
	}
	return 0;
}

int
pip_basic_feed()
{				/* ��ʳ */
	time_t now;
	char buf[256];
	int pipkey;

	d.nodone = 1;

	do {
		if (d.death == 1 || d.death == 2 || d.death == 3)
			return 0;
		if (pip_mainmenu(1))
			return 0;
		move(b_lines - 2, 0);
		clrtoeol();
		move(b_lines - 2, 1);
		sprintf(buf, "%s����ʲ������?", d.name);
		prints(buf);
		now = time(0);
		move(b_lines, 0);
		clrtoeol();
		move(b_lines, 0);
		prints
		    ("\033[1;44;37m  ��ʳѡ��  \033[46m[1]�Է� [2]��ʳ [3]���� [4]��֥ [5]�˲� [6]ѩ�� [Q]������         \033[m");
		pip_time_change(now);
		pipkey = igetkey();
		pip_time_change(now);

		switch (pipkey) {
		case '1':
			if (d.food <= 0) {
				move(b_lines, 0);
				temppress("û��ʳ����..��ȥ��ɣ�");
				break;
			}
			move(4, 0);
			if ((d.bbtime / 60 / 30) < 3)
				show_feed_pic(0);
			else
				show_feed_pic(1);
			d.food--;
			d.hp += 50;
			if (d.hp >= d.maxhp) {
				d.hp = d.maxhp;
				d.weight += rand() % 2;
			}
			d.nodone = 0;
			temppress("ÿ��һ��ʳ���ָ�����50�!");
			break;

		case '2':
			if (d.cookie <= 0) {
				move(b_lines, 0);
				temppress("��ʳ�Թ���..��ȥ��ɣ�");
				break;
			}
			move(4, 0);
			d.cookie--;
			d.hp += 100;
			if (d.hp >= d.maxhp) {
				d.hp = d.maxhp;
				d.weight += (rand() % 2 + 2);
			} else {
				d.weight += (rand() % 2 + 1);
			}
			if (rand() % 2 > 0)
				show_feed_pic(2);
			else
				show_feed_pic(3);
			d.happy += (rand() % 3 + 4);
			d.satisfy += rand() % 3 + 2;
			d.nodone = 0;
			temppress("����ʳ�������...");
			break;

		case '3':
			if (d.bighp <= 0) {
				move(b_lines, 0);
				temppress("û�д�����Ү! �����..");
				break;
			}
			d.bighp--;
			d.hp += 600;
			d.tired -= 20;
			d.weight += rand() % 2;
			move(4, 0);
			show_feed_pic(4);
			d.nodone = 0;
			temppress("����..���������...");
			break;

		case '4':
			if (d.medicine <= 0) {
				move(b_lines, 0);
				temppress("û����֥��..��ȥ��ɣ�");
				break;
			}
			move(4, 0);
			show_feed_pic(1);
			d.medicine--;
			d.mp += 50;
			if (d.mp >= d.maxmp) {
				d.mp = d.maxmp;
			}
			d.nodone = 0;
			temppress("ÿ��һ����֥��ָ�����50�!");
			break;

		case '5':
			if (d.ginseng <= 0) {
				move(b_lines, 0);
				temppress("û��ǧ���˲�Ү! �����..");
				break;
			}
			d.ginseng--;
			d.mp += 500;
			d.tired -= 20;
			move(4, 0);
			show_feed_pic(1);
			d.nodone = 0;
			temppress("ǧ���˲�..���������...");
			break;

		case '6':
			if (d.snowgrass <= 0) {
				move(b_lines, 0);
				temppress("û����ɽѩ��Ү! �����..");
				break;
			}
			d.snowgrass--;
			d.mp = d.maxmp;
			d.hp = d.maxhp;
			d.tired -= 0;
			d.sick = 0;
			move(4, 0);
			show_feed_pic(1);
			d.nodone = 0;
			temppress("��ɽѩ��..���������...");
			break;

#ifdef MAPLE
		case Ctrl('R'):
			if (currutmp->msgs[0].last_pid) {
				show_last_call_in();
				my_write(currutmp->msgs[0].last_pid,
					 "ˮ�򶪻�ȥ��");
			}
			d.nodone = 0;
			break;
#endif				// END MAPLE
		}
	}
	while ((pipkey != 'Q') && (pipkey != 'q') && (pipkey != KEY_LEFT));

	return 0;
}
