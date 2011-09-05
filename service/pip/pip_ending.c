/*---------------------------------------------------------------------------*/
/* ��ֺ�ʽ                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

/*--------------------------------------------------------------------------*/
/*  ��ֲ����趨                                                            */
/*--------------------------------------------------------------------------*/
/*����*/
const struct newendingset endmodeall_purpose[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "��Ϊ���������Ů��", "��Ϊ��������¹���", 500 },
{ "��Ϊ���ҵ�����", "��Ϊ���ҵ�����", 400 },
{ "��Ϊ�̻��еĴ�����", "��Ϊ�̻��еĴ�����", 350 },
{ "��Ϊ���ҵĴ�", "��Ϊ���ҵĴ�", 320 },
{ "��Ϊһλ��ʿ", "��Ϊһλ��ʿ", 300 },
{ "��Ϊ�̻��е���Ů", "��Ϊ�̻��е���", 150 },
{ "��Ϊ��ͥ�ϵĴ󷨹�", "��Ϊ��ͥ�ϵķ���", 200 },
{ "��Ϊ֪����ѧ��", "��Ϊ֪����ѧ��", 120 },
{ "��Ϊһ��Ů��", "��Ϊһ���й�", 100 },
{ "������Ժ����", "������Ժ����", 100 },
{ "���ùݹ���", "���ùݹ���", 100 },
{ "��ũ������", "��ũ������", 100 },
{ "�ڲ�������", "�ڲ�������", 100 },
{ "�ڽ��ù���", "�ڽ��ù���", 100 },
{ "�ڵ�̯����", "�ڵ�̯����", 100 },
{ "�ڷ�ľ������", "�ڷ�ľ������", 100 },
{ "������Ժ����", "������Ժ����", 100 },
{ "������������", "������������", 100 },
{ "�ڹ��ع���", "�ڹ��ع���", 100 },
{ "��Ĺ԰����", "��Ĺ԰����", 100 },
{ "���μ�ͥ��ʦ����", "���μ�ͥ��ʦ����", 100 },
{ "�ھƼҹ���", "�ھƼҹ���", 100 },
{ "�ھƵ깤��", "�ھƵ깤��", 100 },
{ "�ڴ�ҹ�ܻṤ��", "�ڴ�ҹ�ܻṤ��", 100 },
{ "�ڼ��а�æ", "�ڼ��а�æ", 50 },
{ "������Ժ���", "������Ժ���", 50 },
{ "���ùݼ��", "���ùݼ��", 50 },
{ "��ũ�����", "��ũ�����", 50 },
{ "�ڲ������", "�ڲ������", 50 },
{ "�ڽ��ü��", "�ڽ��ü��", 50 },
{ "�ڵ�̯���", "�ڵ�̯���", 50 },
{ "�ڷ�ľ�����", "�ڷ�ľ�����", 50 },
{ "������Ժ���", "������Ժ���", 50 },
{ "�����������", "�����������", 50 },
{ "�ڹ��ؼ��", "�ڹ��ؼ��", 50 },
{ "��Ĺ԰���", "��Ĺ԰���", 50 },
{ "���μ�ͥ��ʦ���", "���μ�ͥ��ʦ���", 50 },
{ "�ھƼҼ��", "�ھƼҼ��", 50 },
{ "�ھƵ���", "�ھƵ���", 50 },
{ "�ڴ�ҹ�ܻ���", "�ڴ�ҹ�ܻ���", 50 },
{ NULL, NULL, 0 }
};

/*ս��*/
const struct newendingset endmodecombat[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "����Ϊ���� սʿ��", "����Ϊ���� սʿ��", 420 },
{ "����ߪ��Ϊһ���Ľ���", "����ߪ��Ϊһ���Ľ���", 300 },
{ "�����˹��ҽ����Ӷӳ�", "�����˹��ҽ����Ӷӳ�", 200 },
{ "����������ʦ", "����������ʦ", 150 },
{ "�����ʿ��Ч����", "�����ʿ��Ч����", 160 },
{ "Ͷ����������Ϊʿ��", "Ͷ����������Ϊʿ��", 80 },
{ "��ɽ�������", "��ɽ�������", 0 },
{ "��Ӷ������ά��", "��Ӷ������ά��", 0 },
{ NULL, NULL, 0 }
};

/*ħ��*/
const struct newendingset endmodemagic[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "����Ϊ���� ħ����", "����Ϊ���� ħ����", 420 },
{ "��ƸΪ����ħ��ʦ", "��ƸΪ����ħ��ʦ", 280 },
{ "����ħ����ʦ", "����ħ����ʦ", 160 },
{ "���һλħ��ʿ", "���һλħ��ʿ", 180 },
{ "����ħ��ʦ", "����ħ��ʦ", 120 },
{ "��ռ��ʦ��������Ϊ��", "��ռ��ʦ��������Ϊ��", 40 },
{ "��Ϊһ��ħ��ʦ", "��Ϊһ��ħ��ʦ", 20 },
{ "��Ϊ��ͷ����", "��Ϊ��ͷ����", 10 },
{ NULL, NULL, 0 }
};

/*�罻*/
const struct newendingset endmodesocial[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "��Ϊ�����ĳ���", "��ΪŮ��������ү", 170 },
{ "����ѡ��Ϊ����", "��ѡ�е�Ů���ķ���", 260 },
{ "���������У���Ϊ����", "��Ϊ��Ů�����ķ���", 130 },
{ "��Ϊ����������", "��ΪŮ�����ķ���", 100 },
{ "��Ϊ���˵�����", "��ΪŮ���˵ķ���", 80 },
{ "��Ϊũ�˵�����", "��ΪŮũ�˵ķ���", 80 },
{ "��Ϊ�������鸾", "��ΪŮ���������", -40 },
{ NULL, NULL, 0 }
};

/*����*/
const struct newendingset endmodeart[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "��Ϊ��С��", "��Ϊ��С��", 100 },
{ "��Ϊ������", "��Ϊ������", 100 },
{ "��Ϊ�˻���", "��Ϊ�˻���", 100 },
{ "��Ϊ���赸��", "��Ϊ���赸��", 100 },
{ NULL, NULL, 0 }
};

/*����*/
const struct newendingset endmodeblack[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "�����ħ��", "�����ħ��", -1000 },
{ "�����̫��", "�������å", -350 },
{ "����[�ӣ�Ů��]�Ĺ���", "����[�ӣ͹���]�Ĺ���", -150 },
{ "���˺ڽֵĴ��", "���˺ڽֵ��ϴ�", -500 },
{ "��ɸ߼�潸�", "��ɸ߼����", -350 },
{ "���թ��ʦթ�۱���", "��ɽ�⵳ƭ����Ǯ", -350 },
{ "����ݺ�Ĺ�������", "��ţ�ɵĹ�������", -350 },
{ NULL, NULL, 0 }
};

/*����*/
const struct newendingset endmodefamily[] = {
{ "Ů��ְҵ", "����ְҵ", 0 },
{ "������������", "������������", 50 },
{ NULL, NULL, 0 }
};

int				/*��ֻ��� */
pip_ending_screen()
{
	time_t now;
	char buf[256];
	char endbuf1[50];
	char endbuf2[50];
	char endbuf3[50];
	int endgrade = 0;
	int endmode = 0;

	clear();
	pip_ending_decide(endbuf1, endbuf2, endbuf3, &endmode, &endgrade);
	move(1, 9);
	prints
	    ("\033[1;33m����������������  ����������  ����������������  ��  ������  \033[0m");
	move(2, 9);
	prints
	    ("\033[1;37m��      ����    ������      ����      ����    ������      ��\033[0m");
	move(3, 9);
	prints
	    ("\033[0;37m��    ��  ��    ������      ��������������    ������  ������\033[0m");
	move(4, 9);
	prints
	    ("\033[0;37m��    ��  ��  ��  ����      ��������������  ��  ����      ��\033[0m");
	move(5, 9);
	prints
	    ("\033[1;37m��      ����  ��  ����      ����      ����  ��  ����      ��\033[0m");
	move(6, 9);
	prints
	    ("\033[1;35m��������������  ������������  ��������������  ����  ������  \033[0m");
	move(7, 8);
	prints
	    ("\033[1;31m��������������������\033[41;37m �ǿ�ս������ֱ��� \033[0;1;31m����������������������\033[0m");
	move(9, 10);
	prints("\033[1;36m���ʱ�䲻֪�����ػ��ǵ�����...\033[0m");
	move(11, 10);
	prints
	    ("\033[1;37m\033[33m%s\033[37m ���뿪�����ů�������Լ�һֻ����������������.....\033[0m",
	     d.name);
	move(13, 10);
	prints
	    ("\033[1;36m�����չ˽̵��������ʱ�⣬�����Ӵ��˺ܶ����������˺ܶ������....\033[0m");
	move(15, 10);
	prints
	    ("\033[1;37m��Ϊ��Щ����С�� \033[33m%s\033[37m ֮��������ø���ɶ�����........\033[0m",
	     d.name);
	move(17, 10);
	prints("\033[1;36m�����Ĺ��ģ���ĸ����������еİ�......\033[0m");
	move(19, 10);
	prints("\033[1;37m\033[33m%s\033[37m ����Զ���������ĵ�....\033[0m", d.name);
	temppress("��������δ����չ");
	clrchyiuan(7, 19);
	move(7, 8);
	prints
	    ("\033[1;34m��������������������\033[44;37m �ǿ�ս����δ����չ \033[0;1;34m����������������������\033[0m");
	move(9, 10);
	prints
	    ("\033[1;36m͸��ˮ����������һ������ \033[33m%s\033[36m ��δ����չ��.....\033[0m",
	     d.name);
	move(11, 10);
	prints("\033[1;37mС�� \033[33m%s\033[37m ����%s....\033[0m", d.name, endbuf1);
	move(13, 10);
	prints("\033[1;36m��Ϊ����֮ǰ��Ŭ����ʹ��������һ����%s....\033[0m",
	       endbuf2);
	move(15, 10);
	prints("\033[1;37m���С���Ļ���״����������%s���������Ǻ�����.....\033[0m",
	       endbuf3);
	move(17, 10);
	prints("\033[1;36m��..����һ������Ľ���..........\033[0m");
	temppress("����  ��һ���ܸж���.....");
	show_ending_pic(0);
	temppress("��һ��������");
	endgrade = pip_game_over(endgrade);
	temppress("��һҳ��С������  �Ͽ�copy����������");
	pip_data_list();
	temppress("��ӭ������ս....");
	/*
	 * ��¼��ʼ
	 */
	now = time(0);
	sprintf(buf,
		"\033[1;35m������������������������������������������������������������������������������\033[0m\n");
	pip_log_record(buf);
	sprintf(buf,
		"\033[1;37m�� \033[33m%s \033[37m��ʱ��\033[36m%s \033[37m��С�� \033[32m%s\033[37m �����˽��\033[0m\n",
		Cdate(now), getCurrentUser()->userid, d.name);
	pip_log_record(buf);
	sprintf(buf,
		"\033[1;37mС�� \033[32m%s \033[37mŬ����ǿ�Լ�������%s\033[0m\n\033[1;37m��Ϊ֮ǰ��Ŭ����ʹ������һ����%s\033[0m\n",
		d.name, endbuf1, endbuf2);
	pip_log_record(buf);
	sprintf(buf,
		"\033[1;37m��춻���״����������%s���������Ǻ�����.....\033[0m\n\n\033[1;37mС�� \033[32n%s\033[37m ���ܻ��֣� \033[33m%d\033[0m\n",
		endbuf3, d.name, endgrade);
	pip_log_record(buf);
	sprintf(buf,
		"\033[1;35m������������������������������������������������������������������������������\033[0m\n");
	pip_log_record(buf);
	/*
	 * ��¼��ֹ
	 */
	d.death = 3;
	pipdie("\033[1;31m��Ϸ������...\033[m  ", 3);
	return 0;
}

int
pip_ending_decide(endbuf1, endbuf2, endbuf3, endmode, endgrade)
char *endbuf1, *endbuf2, *endbuf3;
int *endmode, *endgrade;
{
	const static char *name[8][2] = { {"�е�", "Ů��"},
	{"�޸�����", "Ȣ�˹���"},
	{"�޸���", "Ȣ����"},
	{"�޸����ˣ�", "Ȣ��Ů���ˣ�"},
	{"�޸����ˣ�", "Ȣ��Ů���ˣ�"},
	{"�޸����ˣ�", "Ȣ��Ů���ˣ�"},
	{"�޸����ˣ�", "Ȣ��Ů���ˣ�"},
	{"�޸����ˣ�", "Ȣ��Ů���ˣ�"}
	};
	int m = 0, n = 0, grade = 0;
	int modeall_purpose = 0;
	char buf1[256];
	char buf2[256];

	*endmode = pip_future_decide(&modeall_purpose);
	switch (*endmode) {
		/*
		 * 1:���� 2:���� 3:���� 4:սʿ 5:ħ�� 6:�罻 7:����
		 */
	case 1:
		pip_endingblack(buf1, &m, &n, &grade);
		break;
	case 2:
		pip_endingart(buf1, &m, &n, &grade);
		break;
	case 3:
		pip_endingall_purpose(buf1, &m, &n, &grade, modeall_purpose);
		break;
	case 4:
		pip_endingcombat(buf1, &m, &n, &grade);
		break;
	case 5:
		pip_endingmagic(buf1, &m, &n, &grade);
		break;
	case 6:
		pip_endingsocial(buf1, &m, &n, &grade);
		break;
	case 7:
		pip_endingfamily(buf1, &m, &n, &grade);
		break;
	}

	grade += pip_marry_decide();
	strcpy(endbuf1, buf1);
	if (n == 1) {
		*endgrade = grade + 300;
		sprintf(buf2, "�ǳ���˳��..");
	} else if (n == 2) {
		*endgrade = grade + 100;
		sprintf(buf2, "���ֻ�����..");
	} else if (n == 3) {
		*endgrade = grade - 10;
		sprintf(buf2, "�������ܶ�����....");
	}
	strcpy(endbuf2, buf2);
	if (d.lover >= 1 && d.lover <= 7) {
		if (d.sex == 1)
			sprintf(buf2, "%s", name[d.lover][1]);
		else
			sprintf(buf2, "%s", name[d.lover][0]);
	} else if (d.lover == 10)
		sprintf(buf2, "%s", buf1);
	else if (d.lover == 0) {
		if (d.sex == 1)
			sprintf(buf2, "Ȣ��ͬ�е�Ů��");
		else
			sprintf(buf2, "�޸���ͬ�е�����");
	}
	strcpy(endbuf3, buf2);
	return 0;
}

/*����ж�*/
/*1:���� 2:���� 3:���� 4:սʿ 5:ħ�� 6:�罻 7:����*/
int
pip_future_decide(modeall_purpose)
int *modeall_purpose;
{
	int endmode;

	/*
	 * ����
	 */
	if ((d.etchics == 0 && d.offense >= 100)
	    || (d.etchics > 0 && d.etchics < 50 && d.offense >= 250))
		endmode = 1;
	/*
	 * ����
	 */
	else if (d.art > d.hexp && d.art > d.mexp && d.art > d.hskill
		 && d.art > d.mskill && d.art > d.social && d.art > d.family
		 && d.art > d.homework && d.art > d.wisdom && d.art > d.charm
		 && d.art > d.belief && d.art > d.manners && d.art > d.speech
		 && d.art > d.cookskill && d.art > d.love)
		endmode = 2;
	/*
	 * ս��
	 */
	else if (d.hexp >= d.social && d.hexp >= d.mexp && d.hexp >= d.family) {
		*modeall_purpose = 1;
		if (d.hexp > d.social + 50 || d.hexp > d.mexp + 50
		    || d.hexp > d.family + 50)
			endmode = 4;
		else
			endmode = 3;
	}
	/*
	 * ħ��
	 */
	else if (d.mexp >= d.hexp && d.mexp >= d.social && d.mexp >= d.family) {
		*modeall_purpose = 2;
		if (d.mexp > d.hexp || d.mexp > d.social || d.mexp > d.family)
			endmode = 5;
		else
			endmode = 3;
	} else if (d.social >= d.hexp && d.social >= d.mexp
		   && d.social >= d.family) {
		*modeall_purpose = 3;
		if (d.social > d.hexp + 50 || d.social > d.mexp + 50
		    || d.social > d.family + 50)
			endmode = 6;
		else
			endmode = 3;
	}

	else {
		*modeall_purpose = 4;
		if (d.family > d.hexp + 50 || d.family > d.mexp + 50
		    || d.family > d.social + 50)
			endmode = 7;
		else
			endmode = 3;
	}
	return endmode;
}

/*�����ж�*/
int
pip_marry_decide()
{
	int grade;

	if (d.lover != 0) {
		/*
		 * 3 4 5 6 7:���� 
		 */
		d.lover = d.lover;
		grade = 80;
	} else {
		if (d.royalJ >= d.relation && d.royalJ >= 100) {
			d.lover = 1;	/*���� */
			grade = 200;
		} else if (d.relation > d.royalJ && d.relation >= 100) {
			d.lover = 2;	/*���׻�ĸ�� */
			grade = 0;
		} else {
			d.lover = 0;
			grade = 40;
		}
	}
	return grade;
}

int
pip_endingblack(buf, m, n, grade)	/*���� */
char *buf;
int *m, *n, *grade;
{
	if (d.offense >= 500 && d.mexp >= 500) {	/*ħ�� */
		*m = 1;
		if (d.mexp >= 1000)
			*n = 1;
		else if (d.mexp < 1000 && d.mexp >= 800)
			*n = 2;
		else
			*n = 3;
	}

	else if (d.hexp >= 600) {	/*��å */
		*m = 2;
		if (d.wisdom >= 350)
			*n = 1;
		else if (d.wisdom < 350 && d.wisdom >= 300)
			*n = 2;
		else
			*n = 3;
	} else if (d.speech >= 100 && d.art >= 80)
		/*SM*/ {
		*m = 3;
		if (d.speech > 150 && d.art >= 120)
			*n = 1;
		else if (d.speech > 120 && d.art >= 100)
			*n = 2;
		else
			*n = 3;
	} else if (d.hexp >= 320 && d.character > 200 && d.charm < 200) {	/*�ڽ��ϴ� */
		*m = 4;
		if (d.hexp >= 400)
			*n = 1;
		else if (d.hexp < 400 && d.hexp >= 360)
			*n = 2;
		else
			*n = 3;
	} else if (d.character >= 200 && d.charm >= 200 && d.speech > 70 && d.toman > 70) {	/*�߼�潸� */
		*m = 5;
		if (d.charm >= 300)
			*n = 1;
		else if (d.charm < 300 && d.charm >= 250)
			*n = 2;
		else
			*n = 3;
	}

	else if (d.wisdom >= 450) {	/*թƭʦ */
		*m = 6;
		if (d.wisdom >= 550)
			*n = 1;
		else if (d.wisdom < 550 && d.wisdom >= 500)
			*n = 2;
		else
			*n = 3;
	}

	else {			/*��ݺ */

		*m = 7;
		if (d.charm >= 350)
			*n = 1;
		else if (d.charm < 350 && d.charm >= 300)
			*n = 2;
		else
			*n = 3;
	}
	if (d.sex == 1)
		strcpy(buf, endmodeblack[*m].boy);
	else
		strcpy(buf, endmodeblack[*m].girl);
	*grade = endmodeblack[*m].grade;
	return 0;
}

int
pip_endingsocial(buf, m, n, grade)	/*�罻 */
char *buf;
int *m, *n, *grade;
{
	int class;

	if (d.social > 600)
		class = 1;
	else if (d.social > 450)
		class = 2;
	else if (d.social > 380)
		class = 3;
	else if (d.social > 250)
		class = 4;
	else
		class = 5;

	switch (class) {
	case 1:
		if (d.charm > 500) {
			*m = 1;
			d.lover = 10;
			if (d.character >= 700)
				*n = 1;
			else if (d.character < 700 && d.character >= 500)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 2;
			d.lover = 10;
			if (d.character >= 700)
				*n = 1;
			else if (d.character < 700 && d.character >= 500)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 2:
		*m = 1;
		d.lover = 10;
		if (d.character >= 700)
			*n = 1;
		else if (d.character < 700 && d.character >= 500)
			*n = 2;
		else
			*n = 3;
		break;

	case 3:
		if (d.character >= d.charm) {
			*m = 3;
			d.lover = 10;
			if (d.toman >= 250)
				*n = 1;
			else if (d.toman < 250 && d.toman >= 200)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 4;
			d.lover = 10;
			if (d.character >= 400)
				*n = 1;
			else if (d.character < 400 && d.character >= 300)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 4:
		if (d.wisdom >= d.affect) {
			*m = 5;
			d.lover = 10;
			if (d.toman > 120 && d.cookskill > 300
			    && d.homework > 300)
				*n = 1;
			else if (d.toman < 120 && d.cookskill < 300
				 && d.homework < 300 && d.toman > 100
				 && d.cookskill > 250 && d.homework > 250)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 6;
			d.lover = 10;
			if (d.hp >= 400)
				*n = 1;
			else if (d.hp < 400 && d.hp >= 300)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 5:
		*m = 7;
		d.lover = 10;
		if (d.charm >= 200)
			*n = 1;
		else if (d.charm < 200 && d.charm >= 100)
			*n = 2;
		else
			*n = 3;
		break;
	}
	if (d.sex == 1)
		strcpy(buf, endmodesocial[*m].boy);
	else
		strcpy(buf, endmodesocial[*m].girl);
	*grade = endmodesocial[*m].grade;
	return 0;
}

int
pip_endingmagic(buf, m, n, grade)	/*ħ�� */
char *buf;
int *m, *n, *grade;
{
	int class;

	if (d.mexp > 800)
		class = 1;
	else if (d.mexp > 600)
		class = 2;
	else if (d.mexp > 500)
		class = 3;
	else if (d.mexp > 300)
		class = 4;
	else
		class = 5;

	switch (class) {
	case 1:
		if (d.affect > d.wisdom && d.affect > d.belief
		    && d.etchics > 100) {
			*m = 1;
			if (d.etchics >= 800)
				*n = 1;
			else if (d.etchics < 800 && d.etchics >= 400)
				*n = 2;
			else
				*n = 3;
		} else if (d.etchics < 50) {
			*m = 4;
			if (d.hp >= 400)
				*n = 1;
			else if (d.hp < 400 && d.hp >= 200)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 2;
			if (d.wisdom >= 800)
				*n = 1;
			else if (d.wisdom < 800 && d.wisdom >= 400)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 2:
		if (d.etchics >= 50) {
			*m = 3;
			if (d.wisdom >= 500)
				*n = 1;
			else if (d.wisdom < 500 && d.wisdom >= 200)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 4;
			if (d.hp >= 400)
				*n = 1;
			else if (d.hp < 400 && d.hp >= 200)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 3:
		*m = 5;
		if (d.mskill >= 300)
			*n = 1;
		else if (d.mskill < 300 && d.mskill >= 150)
			*n = 2;
		else
			*n = 3;
		break;

	case 4:
		*m = 6;
		if (d.speech >= 150)
			*n = 1;
		else if (d.speech < 150 && d.speech >= 60)
			*n = 2;
		else
			*n = 3;
		break;

	case 5:
		if (d.character >= 200) {
			*m = 7;
			if (d.speech >= 150)
				*n = 1;
			else if (d.speech < 150 && d.speech >= 60)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 8;
			if (d.speech >= 150)
				*n = 1;
			else if (d.speech < 150 && d.speech >= 60)
				*n = 2;
			else
				*n = 3;
		}
		break;

	}

	if (d.sex == 1)
		strcpy(buf, endmodemagic[*m].boy);
	else
		strcpy(buf, endmodemagic[*m].girl);
	*grade = endmodemagic[*m].grade;
	return 0;
}

int
pip_endingcombat(buf, m, n, grade)	/*ս�� */
char *buf;
int *m, *n, *grade;
{
	int class;

	if (d.hexp > 1500)
		class = 1;
	else if (d.hexp > 1000)
		class = 2;
	else if (d.hexp > 800)
		class = 3;
	else
		class = 4;

	switch (class) {
	case 1:
		if (d.affect > d.wisdom && d.affect > d.belief
		    && d.etchics > 100) {
			*m = 1;
			if (d.etchics >= 800)
				*n = 1;
			else if (d.etchics < 800 && d.etchics >= 400)
				*n = 2;
			else
				*n = 3;
		} else if (d.etchics < 50) {

		} else {
			*m = 2;
			if (d.wisdom >= 800)
				*n = 1;
			else if (d.wisdom < 800 && d.wisdom >= 400)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 2:
		if (d.character >= 300 && d.etchics > 50) {
			*m = 3;
			if (d.etchics >= 300 && d.charm >= 300)
				*n = 1;
			else if (d.etchics < 300 && d.charm < 300
				 && d.etchics >= 250 && d.charm >= 250)
				*n = 2;
			else
				*n = 3;
		} else if (d.character < 300 && d.etchics > 50) {
			*m = 4;
			if (d.speech >= 200)
				*n = 1;
			else if (d.speech < 150 && d.speech >= 80)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 7;
			if (d.hp >= 400)
				*n = 1;
			else if (d.hp < 400 && d.hp >= 200)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 3:
		if (d.character >= 400 && d.etchics > 50) {
			*m = 5;
			if (d.etchics >= 300)
				*n = 1;
			else if (d.etchics < 300 && d.etchics >= 150)
				*n = 2;
			else
				*n = 3;
		} else if (d.character < 400 && d.etchics > 50) {
			*m = 4;
			if (d.speech >= 200)
				*n = 1;
			else if (d.speech < 150 && d.speech >= 80)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 7;
			if (d.hp >= 400)
				*n = 1;
			else if (d.hp < 400 && d.hp >= 200)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 4:
		if (d.etchics >= 50) {
			*m = 6;
		} else {
			*m = 8;
		}
		if (d.hskill >= 100)
			*n = 1;
		else if (d.hskill < 100 && d.hskill >= 80)
			*n = 2;
		else
			*n = 3;
		break;
	}

	if (d.sex == 1)
		strcpy(buf, endmodecombat[*m].boy);
	else
		strcpy(buf, endmodecombat[*m].girl);
	*grade = endmodecombat[*m].grade;
	return 0;
}

int
pip_endingfamily(buf, m, n, grade)	/*���� */
char *buf;
int *m, *n, *grade;
{
	*m = 1;
	if (d.charm >= 200)
		*n = 1;
	else if (d.charm < 200 && d.charm > 100)
		*n = 2;
	else
		*n = 3;

	if (d.sex == 1)
		strcpy(buf, endmodefamily[*m].boy);
	else
		strcpy(buf, endmodefamily[*m].girl);
	*grade = endmodefamily[*m].grade;
	return 0;
}

int
pip_endingall_purpose(buf, m, n, grade, mode)	/*���� */
char *buf;
int *m, *n, *grade;
int mode;
{
	int data;
	int class = 0;
	int num = 0;

	if (mode == 1)
		data = d.hexp;
	else if (mode == 2)
		data = d.mexp;
	else if (mode == 3)
		data = d.social;
	else if (mode == 4)
		data = d.family;
	if (class > 1000)
		class = 1;
	else if (class > 800)
		class = 2;
	else if (class > 500)
		class = 3;
	else if (class > 300)
		class = 4;
	else
		class = 5;

	data = pip_max_worktime(&num);
	switch (class) {
	case 1:
		if (d.character >= 1000) {
			*m = 1;
			if (d.etchics >= 900)
				*n = 1;
			else if (d.etchics < 900 && d.etchics >= 600)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 2;
			if (d.etchics >= 650)
				*n = 1;
			else if (d.etchics < 650 && d.etchics >= 400)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 2:
		if (d.belief > d.etchics && d.belief > d.wisdom) {
			*m = 3;
			if (d.etchics >= 500)
				*n = 1;
			else if (d.etchics < 500 && d.etchics >= 250)
				*n = 2;
			else
				*n = 3;
		} else if (d.etchics > d.belief && d.etchics > d.wisdom) {
			*m = 4;
			if (d.wisdom >= 800)
				*n = 1;
			else if (d.wisdom < 800 && d.wisdom >= 600)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 5;
			if (d.affect >= 800)
				*n = 1;
			else if (d.affect < 800 && d.affect >= 400)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 3:
		if (d.belief > d.etchics && d.belief > d.wisdom) {
			*m = 6;
			if (d.belief >= 400)
				*n = 1;
			else if (d.belief < 400 && d.belief >= 150)
				*n = 2;
			else
				*n = 3;
		} else if (d.etchics > d.belief && d.etchics > d.wisdom) {
			*m = 7;
			if (d.wisdom >= 700)
				*n = 1;
			else if (d.wisdom < 700 && d.wisdom >= 400)
				*n = 2;
			else
				*n = 3;
		} else {
			*m = 8;
			if (d.affect >= 800)
				*n = 1;
			else if (d.affect < 800 && d.affect >= 400)
				*n = 2;
			else
				*n = 3;
		}
		break;

	case 4:
		if (num >= 2) {
			*m = 8 + num;
			switch (num) {
			case 2:
				if (d.love > 100)
					*n = 1;
				else if (d.love > 50)
					*n = 2;
				else
					*n = 3;
				break;
			case 3:
				if (d.homework > 100)
					*n = 1;
				else if (d.homework > 50)
					*n = 2;
				else
					*n = 3;
				break;
			case 4:
				if (d.hp > 600)
					*n = 1;
				else if (d.hp > 300)
					*n = 2;
				else
					*n = 3;
				break;
			case 5:
				if (d.cookskill > 200)
					*n = 1;
				else if (d.cookskill > 100)
					*n = 2;
				else
					*n = 3;
				break;
			case 6:
				if ((d.belief + d.etchics) > 600)
					*n = 1;
				else if ((d.belief + d.etchics) > 200)
					*n = 2;
				else
					*n = 3;
				break;
			case 7:
				if (d.speech > 150)
					*n = 1;
				else if (d.speech > 50)
					*n = 2;
				else
					*n = 3;
				break;
			case 8:
				if ((d.hp + d.wrist) > 900)
					*n = 1;
				else if ((d.hp + d.wrist) > 600)
					*n = 2;
				else
					*n = 3;
				break;
			case 9:
			case 11:
				if (d.art > 250)
					*n = 1;
				else if (d.art > 100)
					*n = 2;
				else
					*n = 3;
				break;
			case 10:
				if (d.hskill > 250)
					*n = 1;
				else if (d.hskill > 100)
					*n = 2;
				else
					*n = 3;
				break;
			case 12:
				if (d.belief > 500)
					*n = 1;
				else if (d.belief > 200)
					*n = 2;
				else
					*n = 3;
				break;
			case 13:
				if (d.wisdom > 500)
					*n = 1;
				else if (d.wisdom > 200)
					*n = 2;
				else
					*n = 3;
				break;
			case 14:
			case 16:
				if (d.charm > 1000)
					*n = 1;
				else if (d.charm > 500)
					*n = 2;
				else
					*n = 3;
				break;
			case 15:
				if (d.charm > 700)
					*n = 1;
				else if (d.charm > 300)
					*n = 2;
				else
					*n = 3;
				break;
			}
		} else {
			*m = 9;
			if (d.etchics > 400)
				*n = 1;
			else if (d.etchics > 200)
				*n = 2;
			else
				*n = 3;
		}
		break;
	case 5:
		if (num >= 2) {
			*m = 24 + num;
			switch (num) {
			case 2:
			case 3:
				if (d.hp > 400)
					*n = 1;
				else if (d.hp > 150)
					*n = 2;
				else
					*n = 3;
				break;
			case 4:
			case 10:
			case 11:
				if (d.hp > 600)
					*n = 1;
				else if (d.hp > 300)
					*n = 2;
				else
					*n = 3;
				break;
			case 5:
				if (d.cookskill > 150)
					*n = 1;
				else if (d.cookskill > 80)
					*n = 2;
				else
					*n = 3;
				break;
			case 6:
				if ((d.belief + d.etchics) > 600)
					*n = 1;
				else if ((d.belief + d.etchics) > 200)
					*n = 2;
				else
					*n = 3;
				break;
			case 7:
				if (d.speech > 150)
					*n = 1;
				else if (d.speech > 50)
					*n = 2;
				else
					*n = 3;
				break;
			case 8:
				if ((d.hp + d.wrist) > 700)
					*n = 1;
				else if ((d.hp + d.wrist) > 300)
					*n = 2;
				else
					*n = 3;
				break;
			case 9:
				if (d.art > 100)
					*n = 1;
				else if (d.art > 50)
					*n = 2;
				else
					*n = 3;
				break;
			case 12:
				if (d.hp > 300)
					*n = 1;
				else if (d.hp > 150)
					*n = 2;
				else
					*n = 3;
				break;
			case 13:
				if (d.speech > 100)
					*n = 1;
				else if (d.speech > 40)
					*n = 2;
				else
					*n = 3;
				break;
			case 14:
			case 16:
				if (d.charm > 1000)
					*n = 1;
				else if (d.charm > 500)
					*n = 2;
				else
					*n = 3;
				break;
			case 15:
				if (d.charm > 700)
					*n = 1;
				else if (d.charm > 300)
					*n = 2;
				else
					*n = 3;
				break;
			}
		} else {
			*m = 25;
			if (d.relation > 100)
				*n = 1;
			else if (d.relation > 50)
				*n = 2;
			else
				*n = 3;
		}
		break;
	}

	if (d.sex == 1)
		strcpy(buf, endmodeall_purpose[*m].boy);
	else
		strcpy(buf, endmodeall_purpose[*m].girl);
	*grade = endmodeall_purpose[*m].grade;
	return 0;
}

int
pip_endingart(buf, m, n, grade)	/*���� */
char *buf;
int *m, *n, *grade;
{
	if (d.speech >= 100) {
		*m = 1;
		if (d.hp >= 300 && d.affect >= 350)
			*n = 1;
		else if (d.hp < 300 && d.affect < 350 && d.hp >= 250
			 && d.affect >= 300)
			*n = 2;
		else
			*n = 3;
	} else if (d.wisdom >= 400) {
		*m = 2;
		if (d.affect >= 500)
			*n = 1;
		else if (d.affect < 500 && d.affect >= 450)
			*n = 2;
		else
			*n = 3;
	} else if (d.classI >= d.classJ) {
		*m = 3;
		if (d.affect >= 350)
			*n = 1;
		else if (d.affect < 350 && d.affect >= 300)
			*n = 2;
		else
			*n = 3;
	} else {
		*m = 4;
		if (d.affect >= 200 && d.hp > 150)
			*n = 1;
		else if (d.affect < 200 && d.affect >= 180 && d.hp > 150)
			*n = 2;
		else
			*n = 3;
	}
	if (d.sex == 1)
		strcpy(buf, endmodeart[*m].boy);
	else
		strcpy(buf, endmodeart[*m].girl);
	*grade = endmodeart[*m].grade;
	return 0;
}

int
pip_max_worktime(num)
int *num;
{
	int data = 20;

	if (d.workA > data) {
		data = d.workA;
		*num = 1;
	}
	if (d.workB > data) {
		data = d.workB;
		*num = 2;
	}
	if (d.workC > data) {
		data = d.workC;
		*num = 3;
	}
	if (d.workD > data) {
		data = d.workD;
		*num = 4;
	}
	if (d.workE > data) {
		data = d.workE;
		*num = 5;
	}

	if (d.workF > data) {
		data = d.workF;
		*num = 6;
	}
	if (d.workG > data) {
		data = d.workG;
		*num = 7;
	}
	if (d.workH > data) {
		data = d.workH;
		*num = 8;
	}
	if (d.workI > data) {
		data = d.workI;
		*num = 9;
	}
	if (d.workJ > data) {
		data = d.workJ;
		*num = 10;
	}
	if (d.workK > data) {
		data = d.workK;
		*num = 11;
	}
	if (d.workL > data) {
		data = d.workL;
		*num = 12;
	}
	if (d.workM > data) {
		data = d.workM;
		*num = 13;
	}
	if (d.workN > data) {
		data = d.workN;
		*num = 14;
	}
	if (d.workO > data) {
		data = d.workO;
		*num = 16;
	}
	if (d.workP > data) {
		data = d.workP;
		*num = 16;
	}

	return data;
}

int
pip_game_over(endgrade)
int endgrade;
{
	long gradebasic;
	long gradeall;

	gradebasic =
	    (d.maxhp + d.wrist + d.wisdom + d.character + d.charm + d.etchics +
	     d.belief + d.affect) / 10 - d.offense;
	clrchyiuan(1, 23);
	gradeall = gradebasic + endgrade;
	move(8, 17);
	prints("\033[1;36m��л�����������ǿ�С������Ϸ.....\033[0m");
	move(10, 17);
	prints("\033[1;37m����ϵͳ����Ľ����\033[0m");
	move(12, 17);
	prints("\033[1;36m����С�� \033[37m%s \033[36m�ܵ÷֣� \033[1;5;33m%d \033[0m", d.name,
	       gradeall);
	return gradeall;
}

int
pip_divine()
{				/*ռ��ʦ���� */
	char buf[256];
	char ans[4];
	char endbuf1[50];
	char endbuf2[50];
	char endbuf3[50];
	int endgrade = 0;
	int endmode = 0;
	long money;
	int tm;
	int randvalue;

	tm = d.bbtime / 60 / 30;
	move(b_lines - 2, 0);
	money = 300 * (tm + 1);
	clrchyiuan(0, 24);
	move(10, 14);
	prints("\033[1;33;5mߵߵߵ...\033[0;1;37mͻȻ���������������.........\033[0m");
	temppress("ȥ������˭��......");
	clrchyiuan(0, 24);
	move(10, 14);
	prints("\033[1;37;46m    ԭ���������ĺ���ռ��ʦ������.......    \033[0m");
	temppress("��������������....");
	if (d.money >= money) {
		randvalue = rand() % 5;
		sprintf(buf, "��Ҫռ����? Ҫ��%ldԪ�...[Y/n]", money);
#ifdef MAPLE
		getdata(12, 14, buf, ans, 2, 1, 0);
#else
		getdata(12,14,buf,ans,2,DOECHO,NULL,true);
#endif				// END MAPLE
		if (ans[0] != 'N' && ans[0] != 'n') {
			pip_ending_decide(endbuf1, endbuf2, endbuf3, &endmode,
					  &endgrade);
			if (randvalue == 0)
				sprintf(buf,
					"\033[1;37m  ���С��%s�Ժ���ܵ������%s  \033[0m",
					d.name,
					endmodemagic[2 + rand() % 5].girl);
			else if (randvalue == 1)
				sprintf(buf,
					"\033[1;37m  ���С��%s�Ժ���ܵ������%s  \033[0m",
					d.name,
					endmodecombat[2 + rand() % 6].girl);
			else if (randvalue == 2)
				sprintf(buf,
					"\033[1;37m  ���С��%s�Ժ���ܵ������%s  \033[0m",
					d.name,
					endmodeall_purpose[6 +
							   rand() % 15].girl);
			else if (randvalue == 3)
				sprintf(buf,
					"\033[1;37m  ���С��%s�Ժ���ܵ������%s  \033[0m",
					d.name,
					endmodeart[2 + rand() % 6].girl);
			else if (randvalue == 4)
				sprintf(buf,
					"\033[1;37m  ���С��%s�Ժ���ܵ������%s  \033[0m",
					d.name, endbuf1);
			d.money -= money;
			clrchyiuan(0, 24);
			move(10, 14);
			prints("\033[1;33m����ռ���������....\033[0m");
			move(12, 14);
			prints(buf);
			temppress("лл�ݹˣ���Ե�ټ�����.(��׼���ܹ����)");
		} else {
			temppress("�㲻��ռ����?..���ϧ..��ֻ�е��´ΰ�...");
		}
	} else {
		temppress("���Ǯ�����..���ǿ�ϧ..���´ΰ�...");
	}
	return 0;
}
