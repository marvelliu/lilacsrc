/*---------------------------------------------------------------------------*/
/* ����ѡ��:ɢ�� ���� �˶� Լ�� ��ȭ                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

int
pip_play_stroll()
{				/*ɢ�� */
	int lucky;

	count_tired(3, 3, "Y", 100, 0);
	lucky = rand() % 7;
	if (lucky == 2) {
		d.happy += rand() % 3 + rand() % 3 + 9;
		d.satisfy += rand() % 3 + rand() % 3 + 3;
		d.shit += rand() % 3 + 3;
		d.hp -= (rand() % 3 + 5);
		move(4, 0);
		if (rand() % 2 > 0)
			show_play_pic(1);
		else
			show_play_pic(2);
		temppress("����������  ���.... ^_^");
	} else if (lucky == 3) {
		d.money += 100;
		d.happy += rand() % 3 + 6;
		d.satisfy += rand() % 3 + 4;
		d.shit += rand() % 3 + 3;
		d.hp -= (rand() % 3 + 4);
		move(4, 0);
		show_play_pic(3);
		temppress("����100Ԫ��..ҮҮҮ....");
	}

	else if (lucky == 4) {
		if (rand() % 2 > 0) {
			d.happy -= (rand() % 2 + 5);
			move(4, 0);
			d.hp -= (rand() % 3 + 3);
			show_play_pic(4);
			if (d.money >= 50) {
				d.money -= 50;
				temppress("����50Ԫ��..������....");
			} else {
				d.money = 0;
				d.hp -= (rand() % 3 + 3);
				temppress("Ǯ�������..������....");
			}
			d.shit += rand() % 3 + 2;
		} else {
			d.happy += rand() % 3 + 5;
			move(4, 0);
			show_play_pic(5);
			if (d.money >= 50) {
				d.money -= 50;
				d.hp -= (rand() % 3 + 3);
				temppress("����50Ԫ��..�����������....");
			} else {
				d.money = 0;
				d.hp -= (rand() % 3 + 3);
				temppress("Ǯ����͵�ù����..:p");
			}
			d.shit += rand() % 3 + 2;
		}
	} else if (lucky == 5) {
		d.happy += rand() % 3 + 6;
		d.satisfy += rand() % 3 + 5;
		d.shit += 2;
		move(4, 0);
		if (rand() % 2 > 0)
			show_play_pic(6);
		else
			show_play_pic(7);
		temppress("�ð�฼������˵.....");
	} else if (lucky == 6) {
		d.happy -= (rand() % 3 + 10);
		d.shit += (rand() % 3 + 20);
		move(4, 0);
		show_play_pic(9);
		temppress("���ǵ��  ����ȥ�򰮹���ȯ");
	} else {
		d.happy += rand() % 3 + 3;
		d.satisfy += rand() % 2 + 1;
		d.shit += rand() % 3 + 2;
		d.hp -= (rand() % 3 + 2);
		move(4, 0);
		show_play_pic(8);
		temppress("û���ر���·�����.....");
	}
	return 0;
}

int
pip_play_sport()
{				/*�˶� */
	count_tired(3, 8, "Y", 100, 1);
	d.weight -= (rand() % 3 + 2);
	d.satisfy += rand() % 2 + 3;
	if (d.satisfy > 100)
		d.satisfy = 100;
	d.shit += rand() % 5 + 10;
	d.hp -= (rand() % 2 + 8);
	d.maxhp += rand() % 2;
	d.speed += (2 + rand() % 3);
	move(4, 0);
	show_play_pic(10);
	temppress("�˶��ô������...");
	return 0;
}

int
pip_play_date()
{				/*Լ�� */
	if (d.money < 150) {
		temppress("��Ǯ��������! Լ���ܵû���ǮǮ");
	} else {
		count_tired(3, 6, "Y", 100, 1);
		d.happy += rand() % 5 + 12;
		d.shit += rand() % 3 + 5;
		d.hp -= rand() % 4 + 8;
		d.satisfy += rand() % 5 + 7;
		d.character += rand() % 3 + 1;
		d.money = d.money - 150;
		move(4, 0);
		show_play_pic(11);
		temppress("Լ��ȥ  ����");
	}
	return 0;
}

int
pip_play_outing()
{				/*���� */
	int lucky;
	char buf[256];

	if (d.money < 250) {
		temppress("��Ǯ��������! �����ܵû���ǮǮ");
	} else {
		d.weight += rand() % 2 + 1;
		d.money -= 250;
		count_tired(10, 45, "N", 100, 0);
		d.hp -= rand() % 10 + 20;
		if (d.hp >= d.maxhp)
			d.hp = d.maxhp;
		d.happy += rand() % 10 + 12;
		d.character += rand() % 5 + 5;
		d.satisfy += rand() % 10 + 10;
		lucky = rand() % 4;
		if (lucky == 0) {
			d.maxmp += rand() % 3;
			d.art += rand() % 2;
			show_play_pic(12);
			if (rand() % 2 > 0)
				temppress
				    ("������һ�ɵ����ĸо�  ������....");
			else
				temppress("��ˮ ���� ����ö���.....");
		} else if (lucky == 1) {
			d.art += rand() % 3;
			d.maxmp += rand() % 2;
			show_play_pic(13);
			if (rand() % 2 > 0)
				temppress
				    ("��ɽ��ˮ������  �γ�һ�������Ļ�..");
			else
				temppress("���ſ���  ȫ��ƣ����������..");
		} else if (lucky == 2) {
			d.love += rand() % 3;
			show_play_pic(14);
			if (rand() % 2 > 0)
				temppress("��  ̫����û��ˮ����...");
			else
				temppress("��˵���Ǻ�����  ��˵��?");
		} else if (lucky == 3) {
			d.maxhp += rand() % 3;
			show_play_pic(15);
			if (rand() % 2 > 0)
				temppress
				    ("�����Ƿ����ҹ��ĺ�̲��....����..");
			else
				temppress
				    ("��ˬ�ĺ���ӭ��Ϯ��  ��ϲ�����ָо���....");
		}
		if ((rand() % 301 + rand() % 200) % 100 == 12) {
			lucky = 0;
			clear();
			sprintf(buf,
				"\033[1;41m  �ǿ�ս���� �� %-10s                                                    \033[0m",
				d.name);
			show_play_pic(0);
			move(17, 10);
			prints("\033[1;36m�װ��� \033[1;33m%s ��\033[0m", d.name);
			move(18, 10);
			prints
			    ("\033[1;37m����������Ŭ���������Լ�������  ��������ʮ�ֵĸ����..\033[m");
			move(19, 10);
			prints
			    ("\033[1;36mС��ʹ�Ҿ������㽱�͹�������  ͵͵�ذ�����һ��....^_^\033[0m");
			move(20, 10);
			lucky = rand() % 7;
			if (lucky == 6) {
				prints
				    ("\033[1;33m�ҽ�����ĸ�������ȫ�������ٷ�֮���......\033[0m");
				d.maxhp = d.maxhp * 105 / 100;
				d.hp = d.maxhp;
				d.maxmp = d.maxmp * 105 / 100;
				d.mp = d.maxmp;
				d.attack = d.attack * 105 / 100;
				d.resist = d.resist * 105 / 100;
				d.speed = d.speed * 105 / 100;
				d.character = d.character * 105 / 100;
				d.love = d.love * 105 / 100;
				d.wisdom = d.wisdom * 105 / 100;
				d.art = d.art * 105 / 100;
				d.brave = d.brave * 105 / 100;
				d.homework = d.homework * 105 / 100;
			}

			else if (lucky <= 5 && lucky >= 4) {
				prints
				    ("\033[1;33m�ҽ������ս������ȫ�������ٷ�֮ʮ�.......\033[0m");
				d.attack = d.attack * 110 / 100;
				d.resist = d.resist * 110 / 100;
				d.speed = d.speed * 110 / 100;
				d.brave = d.brave * 110 / 100;
			}

			else if (lucky <= 3 && lucky >= 2) {
				prints
				    ("\033[1;33m�ҽ������ħ��������������ȫ�������ٷ�֮ʮ�.......\033[0m");
				d.maxhp = d.maxhp * 110 / 100;
				d.hp = d.maxhp;
				d.maxmp = d.maxmp * 110 / 100;
				d.mp = d.maxmp;
			} else if (lucky <= 1 && lucky >= 0) {
				prints
				    ("\033[1;33m�ҽ�����ĸ�������ȫ�������ٷ�֮��ʮ�....\033[0m");
				d.character = d.character * 110 / 100;
				d.love = d.love * 110 / 100;
				d.wisdom = d.wisdom * 110 / 100;
				d.art = d.art * 110 / 100;
				d.homework = d.homework * 110 / 100;
			}

			temppress("����������...");
		}
	}
	return 0;
}

int
pip_play_kite()
{				/*���� */
	count_tired(4, 4, "Y", 100, 0);
	d.weight += (rand() % 2 + 2);
	d.satisfy += rand() % 3 + 12;
	if (d.satisfy > 100)
		d.satisfy = 100;
	d.happy += rand() % 5 + 10;
	d.shit += rand() % 5 + 6;
	d.hp -= (rand() % 2 + 7);
	d.affect += rand() % 4 + 6;
	move(4, 0);
	show_play_pic(16);
	temppress("�ŷ����������...");
	return 0;
}

int
pip_play_KTV()
    /*KTV*/
{
	if (d.money < 250) {
		temppress("��Ǯ��������! �����ܵû���ǮǮ");
	} else {
		count_tired(10, 10, "Y", 100, 0);
		d.satisfy += rand() % 2 + 20;
		if (d.satisfy > 100)
			d.satisfy = 100;
		d.happy += rand() % 3 + 20;
		d.shit += rand() % 5 + 6;
		d.money -= 250;
		d.hp += (rand() % 2 + 6);
		d.art += rand() % 4 + 3;
		move(4, 0);
		show_play_pic(17);
		temppress("��˵��  ��Ҫ��...");
	}
	return 0;
}

int
pip_play_guess()
{				/* ��ȭ��ʽ */
	int com;
	int pipkey;
	struct tm *qtime;
	time_t now;

	time(&now);
	qtime = localtime(&now);
	d.satisfy += (rand() % 3 + 2);
	count_tired(2, 2, "Y", 100, 1);
	d.shit += rand() % 3 + 2;
	do {
		if (d.death == 1 || d.death == 2 || d.death == 3)
			return 0;
		if (pip_mainmenu(0))
			return 0;
		move(b_lines - 2, 0);
		clrtoeol();
		move(b_lines, 0);
		clrtoeol();
		move(b_lines, 0);
		prints
		    ("\033[1;44;37m  ��ȭѡ��  \033[46m[1]�ҳ����� [2]�ҳ�ʯͷ [3]�ҳ����� [4]��ȭ��¼ [Q]������         \033[m");
		move(b_lines - 1, 0);
		clrtoeol();
		pipkey = igetkey();
		switch (pipkey) {
#ifdef MAPLE
		case Ctrl('R'):
			if (currutmp->msgs[0].last_pid) {
				show_last_call_in();
				my_write(currutmp->msgs[0].last_pid,
					 "ˮ�򶪻�ȥ��");
			}
			break;
#endif				// END MAPLE
		case '4':
			situ();
			break;
		}
	}
	while ((pipkey != '1') && (pipkey != '2') && (pipkey != '3')
	       && (pipkey != 'q') && (pipkey != 'Q'));

	com = rand() % 3;
	move(18, 0);
	clrtobot();
	switch (com) {
	case 0:
		outs("С��������\n");
		break;
	case 1:
		outs("С����ʯͷ\n");
		break;
	case 2:
		outs("С������\n");
		break;
	}

	move(17, 0);

	switch (pipkey) {
	case '1':
		outs("��  ������\n");
		if (com == 0)
			tie();
		else if (com == 1)
			lose();
		else if (com == 2)
			win();
		break;
	case '2':
		outs("�㡡��ʯͷ\n");
		if (com == 0)
			win();
		else if (com == 1)
			tie();
		else if (com == 2)
			lose();
		break;
	case '3':
		outs("�㡡����\n");
		if (com == 0)
			lose();
		else if (com == 1)
			win();
		else if (com == 2)
			tie();
		break;
	case 'q':
		break;
	}
    return 0;
}

int
win()
{
	d.winn++;
	d.hp -= rand() % 2 + 3;
	move(4, 0);
	show_guess_pic(2);
	move(b_lines, 0);
	temppress("С������....~>_<~");
	return 0;
}

int
tie()
{
	d.hp -= rand() % 2 + 3;
	d.happy += rand() % 3 + 5;
	move(4, 0);
	show_guess_pic(3);
	move(b_lines, 0);
	temppress("ƽ��........-_-");
	return 0;
}

int
lose()
{
	d.losee++;
	d.happy += rand() % 3 + 5;
	d.hp -= rand() % 2 + 3;
	move(4, 0);
	show_guess_pic(1);
	move(b_lines, 0);
	temppress("С��Ӯ��....*^_^*");
	return 0;
}

int
situ()
{
	clrchyiuan(19, 21);
	move(19, 0);
	prints("��:\033[44m %dʤ %d��\033[m                     \n", d.winn, d.losee);
	move(20, 0);
	prints("��:\033[44m %dʤ %d��\033[m                     \n", d.losee, d.winn);

	if (d.winn >= d.losee) {
		move(b_lines, 0);
		temppress("��..ӮС��Ҳû�����");
	} else {
		move(b_lines, 0);
		temppress("����..������˼�....��...");
	}
	return 0;
}

