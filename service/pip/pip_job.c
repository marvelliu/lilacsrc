/*---------------------------------------------------------------------------*/
/* ��ѡ��:���� �๤ �ҽ� ��̯                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

int
pip_job_workA()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ͥ�������˽��� + N , ɨ��ϴ�� + N , ��⿼��� + N  ��*/
/*  ��        ���͸��׵Ĺ�ϵ + N , ƣ�� + 1 , ���� - 2      ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ����ͥ������ ��    �� - RND (ƣ��) >=   5 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	d.maxhp += rand() % 2;
	d.shit += rand() % 3 + 5;
	count_tired(3, 7, "Y", 100, 1);
	d.hp -= (rand() % 2 + 4);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	d.affect -= 7 + rand() % 7;
	if (d.affect <= 0)
		d.affect = 0;
	show_job_pic(11);
	if (class >= 75) {
		d.cookskill += rand() % 2 + 7;
		d.homework += rand() % 2 + 7;
		d.family += rand() % 3 + 4;
		d.relation += rand() % 3 + 4;
		workmoney = 80 + (d.cookskill * 2 + d.homework + d.family) / 40;
		temppress("���ºܳɹ��..��һ��Ǯ����..");
	} else if (class < 75 && class >= 50) {
		d.cookskill += rand() % 2 + 5;
		d.homework += rand() % 2 + 5;
		d.family += rand() % 3 + 3;
		d.relation += rand() % 3 + 3;
		workmoney = 60 + (d.cookskill * 2 + d.homework + d.family) / 45;
		temppress("���»���˳�����..����..");
	} else if (class < 50 && class >= 25) {
		d.cookskill += rand() % 3 + 3;
		d.homework += rand() % 3 + 3;
		d.family += rand() % 3 + 2;
		d.relation += rand() % 3 + 2;
		workmoney = 40 + (d.cookskill * 2 + d.homework + d.family) / 50;
		temppress("��������ͨͨ��..���Ը��õ�..����..");
	} else if (class < 25) {
		d.cookskill += rand() % 3 + 1;
		d.homework += rand() % 3 + 1;
		d.family += rand() % 3 + 1;
		d.relation += rand() % 3 + 1;
		workmoney = 20 + (d.cookskill * 2 + d.homework + d.family) / 60;
		temppress("���º�����..����������..");
	}
	d.money += workmoney;
	d.workA += 1;
	return 0;
}

int
pip_job_workB()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������Ժ  ��ĸ�� + N , ���� + 1 , ���� - 1 , ƣ�� + 3   ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ������Ժ  ���� ��    �� - RND (ƣ��) >=  20 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	d.maxhp += rand() % 2 + 1;
	d.shit += rand() % 3 + 5;
	d.affect += rand() % 3 + 4;

	count_tired(3, 9, "Y", 100, 1);
	d.hp -= (rand() % 3 + 6);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	d.charm -= rand() % 3 + 4;
	if (d.charm <= 0)
		d.charm = 0;
	show_job_pic(21);
	if (class >= 90) {
		d.love += rand() % 2 + 7;
		d.toman += rand() % 2 + 2;
		workmoney = 150 + (d.love + d.toman) / 50;
		temppress("����ķ�ܳɹ��..�´������..");
	} else if (class < 90 && class >= 75) {
		d.love += rand() % 2 + 5;
		d.toman += rand() % 2 + 2;
		workmoney = 120 + (d.love + d.toman) / 50;
		temppress("��ķ�����Ĳ����..����..");
	} else if (class < 75 && class >= 50) {
		d.love += rand() % 2 + 3;
		d.toman += rand() % 2 + 1;
		workmoney = 100 + (d.love + d.toman) / 50;
		temppress("С���Ѻ�Ƥ�..����..");
	} else if (class < 50) {
		d.love += rand() % 2 + 1;
		d.toman += rand() % 2 + 1;
		workmoney = 80 + (d.love + d.toman) / 50;
		temppress("������..���ֲ�סС����Ү...");
	}
	d.money += workmoney;
	d.workB += 1;
	return 0;
}

int
pip_job_workC()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ���ù�    ��ɨ��ϴ�� + N , ս������ - N , ƣ�� + 2      ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ���ù�    ���� ��    �� - RND (ƣ��) >=  30 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	d.maxhp += rand() % 2 + 2;
	d.shit += rand() % 3 + 5;
	count_tired(5, 12, "Y", 100, 1);
	d.hp -= (rand() % 4 + 8);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	show_job_pic(31);
	if (class >= 95) {
		d.homework += rand() % 2 + 7;
		d.family += rand() % 2 + 4;
		d.hskill -= rand() % 2 + 7;
		if (d.hskill < 0)
			d.hskill = 0;
		workmoney = 250 + (d.cookskill * 2 + d.homework * 2) / 40;
		temppress("�ù���ҵ��������..ϣ�����ٹ���...");
	} else if (class < 95 && class >= 80) {
		d.homework += rand() % 2 + 5;
		d.family += rand() % 2 + 3;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		workmoney = 200 + (d.cookskill * 2 + d.homework * 2) / 50;
		temppress("�ùݻ���˳�����..����..");
	} else if (class < 80 && class >= 60) {
		d.homework += rand() % 2 + 3;
		d.family += rand() % 2 + 3;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		workmoney = 150 + (d.cookskill * 2 + d.homework * 2) / 50;
		temppress("����ͨͨ��..���Ը��õ�..����..");
	} else if (class < 60) {
		d.homework += rand() % 2 + 1;
		d.family += rand() % 2 + 1;
		d.hskill -= rand() % 2 + 1;
		if (d.hskill < 0)
			d.hskill = 0;
		workmoney = 100 + (d.cookskill * 2 + d.homework * 2) / 50;
		temppress("���������..������������..");
	}
	d.money += workmoney;
	d.workC += 1;
	return 0;
}

int
pip_job_workD()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ��ũ��    ������ + 1 , ���� + 1 , ���� - 1 , ƣ�� + 3   ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ��ũ��    ���� ��    �� - RND (ƣ��) >=  30 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	d.maxhp += rand() % 3 + 2;
	d.wrist += rand() % 2 + 2;
	d.shit += rand() % 5 + 10;
	count_tired(5, 15, "Y", 100, 1);
	d.hp -= (rand() % 4 + 10);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	d.character -= rand() % 3 + 4;
	if (d.character < 0)
		d.character = 0;
	show_job_pic(41);
	if (class >= 95) {
		workmoney = 250 + (d.wrist * 2 + d.hp * 2) / 80;
		temppress("ţ�򳤵ĺú��..ϣ����������æ...");
	} else if (class < 95 && class >= 80) {
		workmoney = 210 + (d.wrist * 2 + d.hp * 2) / 80;
		temppress("�Ǻ�..�������..:)");
	} else if (class < 80 && class >= 60) {
		workmoney = 160 + (d.wrist * 2 + d.hp * 2) / 80;
		temppress("����ͨͨ��..���Ը��õ�..");
	} else if (class < 60) {
		workmoney = 120 + (d.wrist * 2 + d.hp * 2) / 80;
		temppress("�㲻̫�ʺ�ũ���Ĺ���  -_-...");
	}
	d.money += workmoney;
	d.workD += 1;
	return 0;
}

int
pip_job_workE()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������    ������ + N , ս������ - N , ƣ�� + 2          ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ������    ���� ��⿼��� - RND (ƣ��) >=  50 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = d.cookskill - d.tired;
	d.maxhp += rand() % 2 + 1;
	d.shit += rand() % 4 + 12;
	count_tired(5, 9, "Y", 100, 1);
	d.hp -= (rand() % 4 + 8);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	show_job_pic(51);
	if (class >= 80) {
		d.homework += rand() % 2 + 1;
		d.family += rand() % 2 + 1;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		d.cookskill += rand() % 2 + 6;
		workmoney =
		    250 + (d.cookskill * 2 + d.homework * 2 +
			   d.family * 2) / 80;
		temppress("���˶�˵̫�ó���..����һ�̰�...");
	} else if (class < 80 && class >= 60) {
		d.homework += rand() % 2 + 1;
		d.family += rand() % 2 + 1;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		d.cookskill += rand() % 2 + 4;
		workmoney =
		    200 + (d.cookskill * 2 + d.homework * 2 +
			   d.family * 2) / 80;
		temppress("��Ļ�������..:)");
	} else if (class < 60 && class >= 30) {
		d.homework += rand() % 2 + 1;
		d.family += rand() % 2 + 1;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		d.cookskill += rand() % 2 + 2;
		workmoney =
		    150 + (d.cookskill * 2 + d.homework * 2 +
			   d.family * 2) / 80;
		temppress("����ͨͨ��..���Ը��õ�..");
	} else if (class < 30) {
		d.homework += rand() % 2 + 1;
		d.family += rand() % 2 + 1;
		d.hskill -= rand() % 2 + 5;
		if (d.hskill < 0)
			d.hskill = 0;
		d.cookskill += rand() % 2 + 1;
		workmoney =
		    100 + (d.cookskill * 2 + d.homework * 2 +
			   d.family * 2) / 80;
		temppress("��ĳ��մ���ǿ�...");
	}
	d.money += workmoney;
	d.workE += 1;
	return 0;
}

int
pip_job_workF()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������    ������ + 2 , ���� + 1 , ���� - 2 , ƣ�� + 1   ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	count_tired(5, 7, "Y", 100, 1);
	d.love += rand() % 3 + 4;
	d.belief += rand() % 4 + 7;
	d.etchics += rand() % 3 + 7;
	d.shit += rand() % 3 + 3;
	d.hp -= rand() % 3 + 5;
	d.offense -= rand() % 4 + 7;
	if (d.offense < 0)
		d.offense = 0;
	show_job_pic(61);
	if (class >= 75) {
		workmoney = 100 + (d.belief + d.etchics - d.offense) / 20;
		temppress("Ǯ���� �������������� �����һ��...");
	} else if (class < 75 && class >= 50) {
		workmoney = 75 + (d.belief + d.etchics - d.offense) / 20;
		temppress("лл������İ�æ..:)");
	} else if (class < 50 && class >= 25) {
		workmoney = 50 + (d.belief + d.etchics - d.offense) / 20;
		temppress("����ĺ��а�����..�����е�С�۵�����...");
	} else if (class < 25) {
		workmoney = 25 + (d.belief + d.etchics - d.offense) / 20;
		temppress("�����ײ���..��Ҳ���ܴ���....:(");
	}
	d.money += workmoney;
	d.workF += 1;
	return 0;
}

int
pip_job_workG()
{
/* �����������ࡪ��������������������������������������������*/
/* ����̯    ������ + 2 , ���� + 1 , ƣ�� + 3 ,̸�� +1     ��*/
/* �����������ࡪ��������������������������������������������*/
	long workmoney;

	workmoney = 0;
	workmoney = 200 + (d.charm * 3 + d.speech * 2 + d.toman) / 50;
	count_tired(3, 12, "Y", 100, 1);
	d.shit += rand() % 3 + 8;
	d.speed += rand() % 2;
	d.weight -= rand() % 2;
	d.happy -= (rand() % 3 + 7);
	d.satisfy -= rand() % 3 + 5;
	d.hp -= (rand() % 6 + 6);
	d.charm += rand() % 2 + 3;
	d.speech += rand() % 2 + 3;
	d.toman += rand() % 2 + 3;
	move(4, 0);
	show_job_pic(71);
	temppress("�ڵ�̯Ҫ�㾯����..:p");
	d.money += workmoney;
	d.workG += 1;
	return 0;
}

int
pip_job_workH()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ľ��  ������ + 2 , ���� - 2 , ƣ�� + 4              ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ����ľ��  ���� ��    �� - RND (ƣ��) >=  80 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 1) {	/*һ����� */
		temppress("С��̫С��,һ���Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = d.wrist - d.tired;
	d.maxhp += rand() % 2 + 3;
	d.shit += rand() % 7 + 15;
	d.wrist += rand() % 3 + 4;
	count_tired(5, 15, "Y", 100, 1);
	d.hp -= (rand() % 4 + 10);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	d.character -= rand() % 3 + 7;
	if (d.character < 0)
		d.character = 0;
	show_job_pic(81);
	if (class >= 70) {
		workmoney = 350 + d.wrist / 20 + d.maxhp / 80;
		temppress("�������ܺ��..:)");
	} else if (class < 70 && class >= 50) {
		workmoney = 300 + d.wrist / 20 + d.maxhp / 80;
		temppress("���˲������.....:)");
	} else if (class < 50 && class >= 20) {
		workmoney = 250 + d.wrist / 20 + d.maxhp / 80;
		temppress("����ͨͨ��..���Ը��õ�..");
	} else if (class < 20) {
		workmoney = 200 + d.wrist / 20 + d.maxhp / 80;
		temppress("����ǿ�..����������....");
	}
	d.money += workmoney;
	d.workH += 1;
	return 0;
}

int
pip_job_workI()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������Ժ  ������ + 1 , ���� - 1 , ƣ�� + 3              ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ������Ժ  ���� �������� - RND (ƣ��) >=  40 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 1) {	/*һ����� */
		temppress("С��̫С��,һ���Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = d.art - d.tired;
	d.maxhp += rand() % 2;
	d.affect += rand() % 2 + 3;
	count_tired(3, 11, "Y", 100, 1);
	d.shit += rand() % 4 + 8;
	d.hp -= (rand() % 4 + 10);
	d.happy -= (rand() % 3 + 4);
	d.satisfy -= rand() % 3 + 4;
	d.wrist -= rand() % +3;
	if (d.wrist < 0)
		d.wrist = 0;
	/*
	 * show_job_pic(4);
	 */
	if (class >= 80) {
		workmoney = 400 + d.art / 10 + d.affect / 20;
		temppress("���˶���ϲ�������������..:)");
	} else if (class < 80 && class >= 60) {
		workmoney = 360 + d.art / 10 + d.affect / 20;
		temppress("���Ĳ����..�������...:)");
	} else if (class < 60 && class >= 40) {
		workmoney = 320 + d.art / 10 + d.affect / 20;
		temppress("��������..�ټ���һ��..");
	} else if (class < 40) {
		workmoney = 250 + d.art / 10 + d.affect / 20;
		temppress("����ǿ�..�Ժ�������....");
	}
	d.money += workmoney;
	d.workI += 1;
	return 0;
}

int
pip_job_workJ()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ��������  ������ + 1 , ���� - 1 , ĸ�� - 1 , ƣ�� + 3   ��*/
/*  ��        ��ս������ + N                                ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ��������  ���� ��    �� - RND (ƣ��) >=  80 ��          ��*/
/*  ��        ���� ��    �� - RND (ƣ��) >=  40 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	float class1;
	long workmoney;

	/*
	 * �������ϲ���
	 */
	if ((d.bbtime / 60 / 30) < 2) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	class1 = d.wisdom - d.tired;
	count_tired(5, 15, "Y", 100, 1);
	d.shit += rand() % 4 + 13;
	d.weight -= (rand() % 2 + 1);
	d.maxhp += rand() % 2 + 3;
	d.speed += rand() % 2 + 3;
	d.hp -= (rand() % 6 + 8);
	d.character -= rand() % 3 + 4;
	d.happy -= rand() % 5 + 8;
	d.satisfy -= rand() % 5 + 6;
	d.love -= rand() % 3 + 4;
	if (d.character < 0)
		d.character = 0;
	if (d.love < 0)
		d.love = 0;
	move(4, 0);
	show_job_pic(101);
	if (class >= 80 && class1 >= 80) {
		d.hskill += rand() % 2 + 7;
		workmoney = 300 + d.maxhp / 50 + d.hskill / 20;
		temppress("��������������..");
	} else if ((class < 75 && class >= 50) && class1 >= 60) {
		d.hskill += rand() % 2 + 5;
		workmoney = 270 + d.maxhp / 45 + d.hskill / 20;
		temppress("�ջ񻹲����..���Ա���һ����..:)");
	} else if ((class < 50 && class >= 25) && class1 >= 40) {
		d.hskill += rand() % 2 + 3;
		workmoney = 240 + d.maxhp / 40 + d.hskill / 20;
		temppress("������ǿ����  �ټ����..");
	} else if ((class < 25 && class >= 0) && class1 >= 20) {
		d.hskill += rand() % 2 + 1;
		workmoney = 210 + d.maxhp / 30 + d.hskill / 20;
		temppress("�����������������Ľ��....");
	} else if (class < 0) {
		d.hskill += rand() % 2;
		workmoney = 190 + d.hskill / 20;
		temppress("Ҫ�������������ǻ���....");
	}
	d.money = d.money + workmoney;
	d.workJ += 1;
	return 0;
}

int
pip_job_workK()
{
/* �����������ࡪ��������������������������������������������*/
/* ������    ������ + 2 , ���� - 1 , ƣ�� + 3              ��*/
/* �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	/*
	 * �������ϲ���
	 */
	if ((d.bbtime / 60 / 30) < 2) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	count_tired(5, 15, "Y", 100, 1);
	d.shit += rand() % 4 + 16;
	d.weight -= (rand() % 2 + 2);
	d.maxhp += rand() % 2 + 1;
	d.speed += rand() % 2 + 2;
	d.hp -= (rand() % 6 + 10);
	d.charm -= rand() % 3 + 6;
	d.happy -= (rand() % 5 + 10);
	d.satisfy -= rand() % 5 + 6;
	if (d.charm < 0)
		d.charm = 0;
	move(4, 0);
	show_job_pic(111);
	if (class >= 75) {
		workmoney = 250 + d.maxhp / 50;
		temppress("���̺�����  лл����..");
	} else if (class < 75 && class >= 50) {
		workmoney = 220 + d.maxhp / 45;
		temppress("�����г�˳��  ��������..");
	} else if (class < 50 && class >= 25) {
		workmoney = 200 + d.maxhp / 40;
		temppress("���̲�ǿ����  �ټ����..");
	} else if (class < 25 && class >= 0) {
		workmoney = 180 + d.maxhp / 30;
		temppress("��  ����ǿ����ǿ....");
	} else if (class < 0) {
		workmoney = 160;
		temppress("�´�������һ��..ƣ�Ͷȵ�һ������....");
	}

	d.money = d.money + workmoney;
	d.workK += 1;
	return 0;
}

int
pip_job_workL()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ��Ĺ԰    ����ħ���� + N , ���� + 1 , ���� - 1          ��*/
/*  ��        ��ƣ�� + 2                                    ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	float class1;
	long workmoney;

	/*
	 * �������
	 */
	if ((d.bbtime / 60 / 30) < 3) {
		temppress("С�����ڻ�̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	class1 = d.belief - d.tired;
	d.shit += rand() % 5 + 8;
	d.maxmp += rand() % 2;
	d.affect += rand() % 2 + 2;
	d.brave += rand() % 2 + 2;
	count_tired(5, 12, "Y", 100, 1);
	d.hp -= (rand() % 3 + 7);
	d.happy -= (rand() % 4 + 6);
	d.satisfy -= rand() % 3 + 5;
	d.charm -= rand() % 3 + 6;
	if (d.charm < 0)
		d.charm = 0;
	show_job_pic(121);
	if (class >= 75 && class1 >= 75) {
		d.mresist += rand() % 2 + 7;
		workmoney = 200 + (d.affect + d.brave) / 40;
		temppress("��Ĺ�ɹ��  ������Ǯ");
	} else if ((class < 75 && class >= 50) && class1 >= 50) {
		d.mresist += rand() % 2 + 5;
		workmoney = 150 + (d.affect + d.brave) / 50;
		temppress("��Ĺ����ɹ��..л��..");
	} else if ((class < 50 && class >= 25) && class1 >= 25) {
		d.mresist += rand() % 2 + 3;
		workmoney = 120 + (d.affect + d.brave) / 60;
		temppress("��Ĺ�����ǿ�����..����..");
	} else {
		d.mresist += rand() % 2 + 1;
		workmoney = 80 + (d.affect + d.brave) / 70;
		temppress("��Ҳ������˵ɶ��..���ټ���..");
	}
	if (rand() % 10 == 5) {
		temppress("���ǵ��  ����������ħ..");
		pip_fight_bad(12);
	}
	d.money += workmoney;
	d.workL += 1;
	return 0;
}

int
pip_job_workM()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ͥ��ʦ������ + 1 , ĸ�� + N , ���� - 1 , ƣ�� + 7   ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 4) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	workmoney = 50 + d.wisdom / 20 + d.character / 20;
	count_tired(5, 10, "Y", 100, 1);
	d.shit += rand() % 3 + 8;
	d.character += rand() % 2;
	d.wisdom += rand() % 2;
	d.happy -= (rand() % 3 + 6);
	d.satisfy -= rand() % 3 + 5;
	d.hp -= (rand() % 3 + 8);
	d.money = d.money + workmoney;
	move(4, 0);
	show_job_pic(131);
	temppress("�ҽ����� ��ȻǮ����һ����");
	d.workM += 1;
	return 0;
}

int
pip_job_workN()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ���Ƶ�    ����⿼��� + N , ̸������ + N , ���� - 2      ��*/
/*  ��        ��ƣ�� + 5                                    ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ���Ƶ�    ���� ��    �� - RND (ƣ��) >=  60 ��          ��*/
/*  ��        ���� ��    �� - RND (ƣ��) >=  50 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	float class1;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 5) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = (d.hp * 100 / d.maxhp) - d.tired;
	class1 = d.charm - d.tired;
	d.shit += rand() % 5 + 5;
	count_tired(5, 14, "Y", 100, 1);
	d.hp -= (rand() % 3 + 5);
	d.social -= rand() % 5 + 6;
	d.happy -= (rand() % 4 + 6);
	d.satisfy -= rand() % 3 + 5;
	d.wisdom -= rand() % 3 + 4;
	if (d.wisdom < 0)
		d.wisdom = 0;
	/*
	 * show_job_pic(6);
	 */
	if (class >= 75 && class1 >= 75) {
		d.cookskill += rand() % 2 + 7;
		d.speech += rand() % 2 + 5;
		workmoney = 500 + (d.charm) / 5;
		temppress("��ܺ��  :)");
	} else if ((class < 75 && class >= 50) && class1 >= 50) {
		d.cookskill += rand() % 2 + 5;
		d.speech += rand() % 2 + 5;
		workmoney = 400 + (d.charm) / 5;
		temppress("���ܻ�ӭ��Ү....");
	} else if ((class < 50 && class >= 25) && class1 >= 25) {
		d.cookskill += rand() % 2 + 4;
		d.speech += rand() % 2 + 3;
		workmoney = 300 + (d.charm) / 5;
		temppress("��ƽ����..��������...");
	} else {
		d.cookskill += rand() % 2 + 2;
		d.speech += rand() % 2 + 2;
		workmoney = 200 + (d.charm) / 5;
		temppress("�������������..�����....");
	}
	d.money += workmoney;
	d.workN += 1;
	return 0;
}

int
pip_job_workO()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ���Ƽ�    ������ + 2 , ���� + 2 , ���� - 3 , ���� - 3   ��*/
/*  ��        �����˽��� - N , �͸��׵Ĺ�ϵ - N , ƣ�� + 12 ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ���Ƽ�    ���� ��    �� - RND (ƣ��) >=  70 �����ɹ�  ��*/
/*  �����������ࡪ��������������������������������������������*/
	float class;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 4) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = d.charm - d.tired;
	d.shit += rand() % 5 + 14;
	d.charm += rand() % 3 + 8;
	d.offense += rand() % 3 + 8;
	count_tired(5, 22, "Y", 100, 1);
	d.hp -= (rand() % 3 + 8);
	d.social -= rand() % 6 + 12;
	d.happy -= (rand() % 4 + 8);
	d.satisfy -= rand() % 3 + 8;
	d.etchics -= rand() % 6 + 10;
	d.belief -= rand() % 6 + 10;
	if (d.etchics < 0)
		d.etchics = 0;
	if (d.belief < 0)
		d.belief = 0;

	/*
	 * show_job_pic(6);
	 */
	if (class >= 75) {
		d.relation -= rand() % 5 + 12;
		d.toman -= rand() % 5 + 12;
		workmoney = 600 + (d.charm) / 5;
		temppress("���Ǳ���ĺ����  :)");
	} else if (class < 75 && class >= 50) {
		d.relation -= rand() % 5 + 8;
		d.toman -= rand() % 5 + 8;
		workmoney = 500 + (d.charm) / 5;
		temppress("�����ܻ�ӭ��Ү..:)");
	} else if (class < 50 && class >= 25) {
		d.relation -= rand() % 5 + 5;
		d.toman -= rand() % 5 + 5;
		workmoney = 400 + (d.charm) / 5;
		temppress("���ƽ��..����������...");
	} else {
		d.relation -= rand() % 5 + 1;
		d.toman -= rand() % 5 + 1;
		workmoney = 300 + (d.charm) / 5;
		temppress("��..�������������....");
	}
	d.money += workmoney;
	if (d.relation < 0)
		d.relation = 0;
	if (d.toman < 0)
		d.toman = 0;
	d.workO += 1;
	return 0;
}

int
pip_job_workP()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ҹ�ܻᩦ���� + 3 , ���� + 1 , ���� - 2 , ���� - 1   ��*/
/*  ��        �����˽��� - N , ƣ�� + 8                     ��*/
/*  �����������ࡪ��������������������������������������������*/
/*  ����ҹ�ܻᩦ�� ��    �� - RND (ƣ��) >=  70 ��          ��*/
/*  ��        ���� �������� - RND (ƣ��) >=  30 �����ɹ�  ��*/
/*  �����������ء���������������������������������������������*/
	float class;
	float class1;
	long workmoney;

	if ((d.bbtime / 60 / 30) < 6) {
		temppress("С��̫С��,�����Ժ�������...");
		return 0;
	}
	workmoney = 0;
	class = d.charm - d.tired;
	class1 = d.art - d.tired;
	d.shit += rand() % 5 + 7;
	d.charm += rand() % 3 + 8;
	d.offense += rand() % 3 + 8;
	count_tired(5, 22, "Y", 100, 1);
	d.hp -= (rand() % 3 + 8);
	d.social -= rand() % 6 + 12;
	d.happy -= (rand() % 4 + 8);
	d.satisfy -= rand() % 3 + 8;
	d.character -= rand() % 3 + 8;
	d.wisdom -= rand() % 3 + 5;
	if (d.character < 0)
		d.character = 0;
	if (d.wisdom < 0)
		d.wisdom = 0;
	/*
	 * show_job_pic(6);
	 */
	if (class >= 75 && class1 > 30) {
		d.speech += rand() % 5 + 12;
		d.toman -= rand() % 5 + 12;
		workmoney = 1000 + (d.charm) / 5;
		temppress("����ҹ�ܻ��������������  :)");
	} else if ((class < 75 && class >= 50) && class1 > 20) {
		d.speech += rand() % 5 + 8;
		d.toman -= rand() % 5 + 8;
		workmoney = 800 + (d.charm) / 5;
		temppress("����..�����ܻ�ӭ��Ү..:)");
	} else if ((class < 50 && class >= 25) && class1 > 10) {
		d.speech += rand() % 5 + 5;
		d.toman -= rand() % 5 + 5;
		workmoney = 600 + (d.charm) / 5;
		temppress("��Ҫ��������..��������...");
	} else {
		d.speech += rand() % 5 + 1;
		d.toman -= rand() % 5 + 1;
		workmoney = 400 + (d.charm) / 5;
		temppress("��..�㲻����....");
	}
	d.money += workmoney;
	if (d.toman < 0)
		d.toman = 0;
	d.workP += 1;
	return 0;
}
