/*---------------------------------------------------------------------------*/
/* ����ѡ��:���� ���� ����                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

/*---------------------------------------------------------------------------*/
/* ����ѡ��:���� ���� ����                                                   */
/* ���Ͽ�                                                                    */
/*---------------------------------------------------------------------------*/
const char *classrank[6] = { "û��", "����", "�м�", "�߼�", "����", "רҵ" };
const int classmoney[11][2] = { {0, 0},
{60, 110}, {70, 120}, {70, 120}, {80, 130}, {70, 120},
{60, 110}, {90, 140}, {70, 120}, {70, 120}, {80, 130}
};
const int classvariable[11][4] = {
	{0, 0, 0, 0},
	{5, 5, 4, 4}, {5, 7, 6, 4}, {5, 7, 6, 4}, {5, 6, 5, 4}, {7, 5, 4, 6},
	{7, 5, 4, 6}, {6, 5, 4, 6}, {6, 6, 5, 4}, {5, 5, 4, 7}, {7, 5, 4, 7}
};

const char *classword[11][5] = {
	{"����", "�ɹ�һ", "�ɹ���", "ʧ��һ", "ʧ�ܶ�"},

	{"��Ȼ��ѧ", "�����ù�������..", "���Ǵ����� cccc...",
	 "�������ῴ������..����", "����� :~~~~~~"},

	{"��ʫ�δ�", "��ǰ���¹�...���ǵ���˪...", "�춹���Ϲ�..��������֦..",
	 "��..�Ͽβ�Ҫ����ˮ", "�㻹���..���㱳����ʫ������"},

	{"��ѧ����", "����·��  ����·��", "������ӭ������֮��",
	 "��..���ڸ����? �����ú���", "��ѧ�������..��ú�ѧ..:("},

	{"��ѧ����", "���ӱ������й�������..", "�Ӿ���������Ҫ����ȥ����",
	 "ʲ�����Ψ�?��������?? @_@", "�㻹��Ϊ����������־��?"},

	{"��������", "���ҵ�����  ���¾Ž�....", "�Ҵ� �Ҵ� �Ҵ̴̴�..",
	 "��Ҫ����һ����..", "���ڴ̵����? ���ø�һ��"},

	{"��ս��", "�����Ǽ���  ����..", "ʮ��ͭ������ɢ..",
	 "�����߸�һ����...", "ȭͷ��������û����.."},

	{"ħ������", "�ұ� �ұ� �ұ���..", "�ߵ�+���β+����+���=??",
	 "С�����ɨ����  ��Ҫ�һ�..", "�ߡ���ˮ��Ҫ����ˮ������.."},

	{"���ǽ���", "Ҫ��ֻ����ò�ļ�...", "ŷ���..�����ڶ�..",
	 "����ѧ�����??��ѽ..", "����·��û����..���.."},

	{"�滭����", "�ܲ����..���������..", "���������ɫ����ĺܺ�..",
	 "��Ҫ������..Ҫ����..", "��Ҫҧ������..����С���.."},

	{"�赸����", "�����һֻ����..", "�赸ϸ���ܺ��..",
	 "����������һ��..", "����������һ��..��Ҫ�����³.."}
};

/*---------------------------------------------------------------------------*/
/* ����ѡ��:���� ���� ����                                                   */
/* ��ʽ��                                                                    */
/*---------------------------------------------------------------------------*/

int
pip_practice_classA()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����Ȼ��ѧ������ + 1~ 4 , ���� - 0~0 , ��ħ���� - 0~0   ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 2~ 6 , ���� - 0~1 , ��ħ���� - 0~1   ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 3~ 8 , ���� - 0~2 , ��ħ���� - 0~1   ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 4~12 , ���� - 1~3 , ��ħ���� - 0~1   ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = d.wisdom / 200 + 1;	/*��ѧ */
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(1, class, 11, 12, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.wisdom += change4;
	if (body == 1) {
		d.belief -= rand() % (2 + class * 2);
		d.mresist -= rand() % 4;
	} else {
		d.belief -= rand() % (2 + class * 2);
		d.mresist -= rand() % 3;
	}
	pip_practice_gradeup(1, class, d.wisdom / 200 + 1);
	if (d.belief < 0)
		d.belief = 0;
	if (d.mresist < 0)
		d.mresist = 0;
	d.classA += 1;
	return 0;
}

int
pip_practice_classB()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ��ʫ��    ������ + 1~1 , ���� + 0~1 , �������� + 0~1    ��*/
/*  ��        ������ + 0~1                                  ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 1~2 , ���� + 0~2 , �������� + 0~1    ��*/
/*  ��        ������ + 0~1                                  ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 1~4 , ���� + 0~3 , �������� + 0~1    ��*/
/*  ��        ������ + 0~1                                  ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 2~5 , ���� + 0~4 , �������� + 0~1    ��*/
/*  ��        ������ + 0~1                                  ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.affect * 2 + d.wisdom + d.art * 2 + d.character) / 400 + 1;	/*ʫ�� */
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(2, class, 21, 21, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.affect += change3;
	if (body == 1) {
		d.wisdom += rand() % (class + 3);
		d.character += rand() % (class + 3);
		d.art += rand() % (class + 3);
	} else {
		d.wisdom += rand() % (class + 2);
		d.character += rand() % (class + 2);
		d.art += rand() % (class + 2);
	}
	body = (d.affect * 2 + d.wisdom + d.art * 2 + d.character) / 400 + 1;
	pip_practice_gradeup(2, class, body);
	d.classB += 1;
	return 0;
}

int
pip_practice_classC()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ѧ    ������ + 1~1 , ���� + 1~2 , ��ħ���� + 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 1~1 , ���� + 1~3 , ��ħ���� + 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 1~2 , ���� + 1~4 , ��ħ���� + 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 1~3 , ���� + 1~5 , ��ħ���� + 0~1    ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.belief * 2 + d.wisdom) / 400 + 1;	/*��ѧ */
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(3, class, 31, 31, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.wisdom += change2;
	d.belief += change3;
	if (body == 1) {
		d.mresist += rand() % 5;
	} else {
		d.mresist += rand() % 3;
	}
	body = (d.belief * 2 + d.wisdom) / 400 + 1;
	pip_practice_gradeup(3, class, body);
	d.classC += 1;
	return 0;
}

int
pip_practice_classD()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ����ѧ    ������ + 1~2 , ս������ + 0~1 , ���� - 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 2~4 , ս������ + 0~1 , ���� - 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 3~4 , ս������ + 0~1 , ���� - 0~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ������ + 4~5 , ս������ + 0~1 , ���� - 0~1    ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.hskill * 2 + d.wisdom) / 400 + 1;
	if (class > 5)
		class = 5;
	body =
	    pip_practice_function(4, class, 41, 41, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.wisdom += change2;
	if (body == 1) {
		d.hskill += rand() % 3 + 4;
		d.affect -= rand() % 3 + 6;
	} else {
		d.hskill += rand() % 3 + 2;
		d.affect -= rand() % 3 + 6;
	}
	body = (d.hskill * 2 + d.wisdom) / 400 + 1;
	pip_practice_gradeup(4, class, body);
	if (d.affect < 0)
		d.affect = 0;
	d.classD += 1;
	return 0;
}

int
pip_practice_classE()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������    ��ս������ + 0~1 , �������� + 1~1             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 0~1 , �������� + 1~2             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 0~1 , �������� + 1~3             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 0~1 , �������� + 1~4             ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.hskill + d.attack) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(5, class, 51, 51, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.speed += rand() % 3 + 2;
	d.hexp += rand() % 2 + 2;
	d.attack += change4;
	if (body == 1) {
		d.hskill += rand() % 3 + 5;
	} else {
		d.hskill += rand() % 3 + 3;
	}
	body = (d.hskill + d.attack) / 400 + 1;
	pip_practice_gradeup(5, class, body);
	d.classE += 1;
	return 0;
}

int
pip_practice_classF()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������  ��ս������ + 1~1 , �������� + 0~0             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 1~1 , �������� + 0~1             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 1~2 , �������� + 0~1             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ս������ + 1~3 , �������� + 0~1             ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.hskill + d.resist) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(6, class, 61, 61, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.hexp += rand() % 2 + 2;
	d.speed += rand() % 3 + 2;
	d.resist += change2;
	if (body == 1) {
		d.hskill += rand() % 3 + 5;
	} else {
		d.hskill += rand() % 3 + 3;
	}
	body = (d.hskill + d.resist) / 400 + 1;
	pip_practice_gradeup(6, class, body);
	d.classF += 1;
	return 0;
}

int
pip_practice_classG()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ��ħ��    ��ħ������ + 1~1 , ħ������ + 0~2             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ħ������ + 1~2 , ħ������ + 0~3             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ħ������ + 1~3 , ħ������ + 0~4             ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ��ħ������ + 2~4 , ħ������ + 0~5             ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.mskill + d.maxmp) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(7, class, 71, 72, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.maxmp += change3;
	d.mexp += rand() % 2 + 2;
	if (body == 1) {
		d.mskill += rand() % 3 + 7;
	} else {
		d.mskill += rand() % 3 + 4;
	}

	body = (d.mskill + d.maxmp) / 400 + 1;
	pip_practice_gradeup(7, class, body);
	d.classG += 1;
	return 0;
}

int
pip_practice_classH()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ������    �����Ǳ��� + 1~1 , ���� + 1~1                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        �����Ǳ��� + 1~2 , ���� + 1~2                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        �����Ǳ��� + 1~3 , ���� + 1~3                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        �����Ǳ��� + 2~4 , ���� + 1~4                 ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.manners * 2 + d.character) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(8, class, 0, 0, &change1, &change2, &change3,
				  &change4, &change5);
	if (body == 0)
		return 0;
	d.social += rand() % 2 + 2;
	d.manners += change1 + rand() % 2;
	d.character += change1 + rand() % 2;
	body = (d.character + d.manners) / 400 + 1;
	pip_practice_gradeup(8, class, body);
	d.classH += 1;
	return 0;
}

int
pip_practice_classI()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ���滭    ���������� + 1~1 , ���� + 0~1                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 1~2 , ���� + 0~1                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 1~3 , ���� + 0~1                 ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 2~4 , ���� + 0~1                 ��*/
/*  �����������ࡪ��������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.art * 2 + d.character) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(9, class, 91, 91, &change1, &change2,
				  &change3, &change4, &change5);
	if (body == 0)
		return 0;
	d.art += change4;
	d.affect += change2;
	body = (d.affect + d.art) / 400 + 1;
	pip_practice_gradeup(9, class, body);
	d.classI += 1;
	return 0;
}

int
pip_practice_classJ()
{
/*  �����������ࡪ��������������������������������������������*/
/*  ���赸    ���������� + 0~1 , ���� + 0~1 , ���� + 1~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 1~1 , ���� + 0~1 , ���� + 1~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 1~2 , ���� + 0~2 , ���� + 1~1    ��*/
/*  ��        ������������������������������������������������*/
/*  ��        ���������� + 1~3 , ���� + 1~2 , ���� + 1~1    ��*/
/*  �����������ء���������������������������������������������*/
	int body, class;
	int change1, change2, change3, change4, change5;

	class = (d.art * 2 + d.charm) / 400 + 1;
	if (class > 5)
		class = 5;

	body =
	    pip_practice_function(10, class, 0, 0, &change1, &change2, &change3,
				  &change4, &change5);
	if (body == 0)
		return 0;
	d.art += change2;
	d.maxhp += rand() % 3 + 2;
	if (body == 1) {
		d.charm += rand() % (4 + class);
	} else if (body == 2) {
		d.charm += rand() % (2 + class);
	}
	body = (d.art * 2 + d.charm) / 400 + 1;
	pip_practice_gradeup(10, class, body);
	d.classJ += 1;
	return 0;
}

/*����:�κ� �ȼ� ���� ���� ���� ���� ����:����12345 return:body*/
int
pip_practice_function(classnum, classgrade, pic1, pic2, change1, change2,
		      change3, change4, change5)
int classnum, classgrade, pic1, pic2;
int *change1, *change2, *change3, *change4, *change5;
{
	int a, b, body, health;
	char inbuf[256], ans[5];
	long smoney;

	/*
	 * Ǯ���㷨
	 */
	smoney = classgrade * classmoney[classnum][0] + classmoney[classnum][1];
	move(b_lines - 2, 0);
	clrtoeol();
	sprintf(inbuf, "[%8s%4s�γ�]Ҫ�� $%ld ,ȷ��Ҫ��??[y/N]",
		classword[classnum][0], classrank[classgrade], smoney);
#ifdef MAPLE
	getdata(b_lines - 2, 1, inbuf, ans, 2, 1, 0);
#else
	getdata(b_lines-2,1,inbuf,ans,2,DOECHO,NULL,true);
#endif				// END MAPLE
	if (ans[0] != 'y' && ans[0] != 'Y')
		return 0;
	if (d.money < smoney) {
		temppress("�ܱ�Ǹ�...���Ǯ�����");
		return 0;
	}
	count_tired(4, 5, "Y", 100, 1);
	d.money = d.money - smoney;
	/*
	 * �ɹ������ж�
	 */
	health = d.hp * 1 / 2 + rand() % 20 - d.tired;
	if (health > 0)
		body = 1;
	else
		body = 2;

	a = rand() % 3 + 2;
	b = (rand() % 12 + rand() % 13) % 2;
	d.hp -= rand() % (3 + rand() % 3) + classvariable[classnum][0];
	d.happy -= rand() % (3 + rand() % 3) + classvariable[classnum][1];
	d.satisfy -= rand() % (3 + rand() % 3) + classvariable[classnum][2];
	d.shit += rand() % (3 + rand() % 3) + classvariable[classnum][3];
	*change1 = rand() % a + 2 + classgrade * 2 / (body + 1);	/* rand()%3+3 */
	*change2 = rand() % a + 4 + classgrade * 2 / (body + 1);	/* rand()%3+5 */
	*change3 = rand() % a + 5 + classgrade * 3 / (body + 1);	/* rand()%3+7 */
	*change4 = rand() % a + 7 + classgrade * 3 / (body + 1);	/* rand()%3+9 */
	*change5 = rand() % a + 9 + classgrade * 3 / (body + 1);	/* rand()%3+11 */
	if (rand() % 2 > 0 && pic1 > 0)
		show_practice_pic(pic1);
	else if (pic2 > 0)
		show_practice_pic(pic2);
	temppress(classword[classnum][body + b]);
	return body;
}

int
pip_practice_gradeup(classnum, classgrade, data)
int classnum, classgrade, data;
{
	char inbuf[256];

	if ((data == (classgrade + 1)) && classgrade < 5) {
		sprintf(inbuf, "�´λ��� [%8s%4s�γ�]",
			classword[classnum][0], classrank[classgrade + 1]);
		temppress(inbuf);
	}
	return 0;
}
