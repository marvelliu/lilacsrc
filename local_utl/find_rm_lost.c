//���Һ�ɾ����ʧ��boardĿ¼�µ�����     ylsdd   2001/6/16

#include "bbs.h"

#define MAXFILE 20000
#define MINAGE 50000		//at least 50000 sec old
#ifdef NUMBUFFER
#undef NUMBUFFER
#endif
#define NUMBUFFER 100
#define HASHSIZE 50

#ifdef SMTH
extern const char alphabet[];
#endif

char *(spcname[]) = {
"deny_users", NULL};

int nfile[HASHSIZE];
char allpost[HASHSIZE][MAXFILE][20];
int refcount[HASHSIZE][MAXFILE];
char otherfile[200];
int allfile = 0, allref = 0, alllost = 0, unknownfn = 0, nindexitem = 0,
    nstrangeitem = 0;

struct a_template *tpl;
int tpl_num;

int
hash(const char *postname){
	int i = atoi(postname + 2);
	if( i<0 ) i=0-i;
	return i % HASHSIZE;
}

int
ispostfilename(char *file)
{
	if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2)
	    &&strncmp(file, "D.", 2) && strncmp(file, "J.", 2) && strncmp(file, "Z.", 2))
		return 0;
	if (!isdigit(file[3]))
		return 0;
	if (strlen(file) >= 20)
		return 0;
	return 1;
}

int
isspcname(char *file)
{
	int i;
	for (i = 0; spcname[i] != NULL; i++)
		if (!strcmp(file, spcname[i]))
			return 1;
	return 0;
}

int countfile_new(const char *fname){
	
	int i,h;

	nindexitem++;
#ifndef SMTH
	h = hash(fname);
#else
	if( fname[1] == '/' )
		h = hash(fname+2);
	else
		h = hash(fname);
#endif
	for (i = 0; i < nfile[h]; i++) {
		if (strcmp(allpost[h][i], fname))
			continue;
		refcount[h][i]++;
		break;
	}
	return 0;
}

int countfile(const struct fileheader *fhdr,int index,void *farg){
    return countfile_new(fhdr->filename);
}


int totalsize = 0;
int
testPOWERJUNK(char *path, char *fn)
{
	char buf[1024];
	struct stat st;
	sprintf(buf, "%s/%s", path, fn);
	if (strncmp(fn, ".AUTHOR", 7) && strncmp(fn, ".TITLE", 6) && strncmp(fn, ".Search", 7) && strncmp(fn, ".SELF.", 6))
		return 0;
	if (stat(buf,&st)!=0)
		return 0;
	if (time(NULL) - st.st_mtime < 3600 * 5)
		return 0;
	unlink(buf);
	printf("%s\n", buf);
	return 0;
}

int
getallpost(char *path, char prefix)
{
	DIR *dirp;
	struct dirent *direntp;
	int h;
	dirp = opendir(path);
	if (dirp == NULL)
		return -1;
	while ((direntp = readdir(dirp)) != NULL) {
		if (direntp->d_name[0] == '.') {
			testPOWERJUNK(path, direntp->d_name);
			continue;
		}
#ifdef SMTH
		if( prefix == NULL && strlen(direntp->d_name)==1 && strchr( alphabet, direntp->d_name[0] ) ){
			char buf[200];
			sprintf(buf, "%s/%c", path, direntp->d_name[0] );
			getallpost(buf, direntp->d_name[0]);
			continue;
		}
#endif
		if (ispostfilename(direntp->d_name)) {
			h = hash(direntp->d_name);
			if (nfile[h] >= MAXFILE) {
				printf("��?!");
				exit(0);
			}
			refcount[h][nfile[h]] = 0;
#ifndef SMTH
			strcpy(allpost[h][nfile[h]++], direntp->d_name);
#else
			if( prefix == NULL ){
				strncpy(allpost[h][nfile[h]++], direntp->d_name, 20);
				allpost[h][nfile[h]][19]=0;
			}else
				snprintf(allpost[h][nfile[h]++], 20, "%c/%s", prefix, direntp->d_name);
#endif
			continue;
		}
		if (isspcname(direntp->d_name))
			continue;
		unknownfn++;
		if (strlen(otherfile) + strlen(direntp->d_name) + 1 <
		    sizeof (otherfile)) {
			strcat(otherfile, " ");
			strcat(otherfile, direntp->d_name);
		}
	}
	closedir(dirp);
	return 0;
}

int
useindexfile(char *filename)
{
	return apply_record(filename, (APPLY_FUNC_ARG)countfile, sizeof (struct fileheader),
				NULL,false,false);
}

int
rm_lost(char *path)
{
	int i, h, total, totalref, lost, t;
	char buf[MAXPATH];
	for (h = 0, total = 0, totalref = 0, lost = 0; h < HASHSIZE; h++) {
		total += nfile[h];
		for (i = 0; i < nfile[h]; i++) {
			totalref += refcount[h][i];
			if (!refcount[h][i]) {
				lost++;
#ifndef SMTH
				t = atoi(allpost[h][i] + 2);
#else
				if(allpost[h][i][1]=='/')
					t = atoi(allpost[h][i] + 4);
				else
					t = atoi(allpost[h][i] + 2);
#endif
				if (time(NULL) - t < MINAGE) {
					//printf("Too young to die, %d %d\n", t, time(NULL));
					continue;
				}
				sprintf(buf, "%s/%s", path, allpost[h][i]);
				unlink(buf);
			}
		}
	}
	allfile += total;
	allref += totalref;
	alllost += lost;
	printf(" total %d, refcount %d, %d file(s) was lost\n", total, totalref,
	       lost);
	if (strlen(otherfile) > 0)
		printf("%s\n", otherfile);
	return 0;
}

int
find_rm_lost(struct boardheader *bhp,void* arg)
{
	char buf[200];
	int i;
	for (i = 0; i < HASHSIZE; i++) {
		nfile[i] = 0;
	}
	otherfile[0] = 0;
	printf("~~~starting board: %s\n", bhp->filename);
	sprintf(buf, "boards/%s", bhp->filename);
	if (getallpost(buf, NULL) < 0)
		return -1;
	sprintf(buf, "boards/%s/.DIR", bhp->filename);
	if (dashf(buf))
		if (useindexfile(buf) < 0)
			return -1;
	sprintf(buf, "boards/%s/.DIGEST", bhp->filename);
	if (dashf(buf))
		if (useindexfile(buf) < 0)
			return -1;
	sprintf(buf, "boards/%s/.DELETED", bhp->filename);
	if (dashf(buf))
		if (useindexfile(buf) < 0)
			return -1;
	sprintf(buf, "boards/%s/.JUNK", bhp->filename);
	if (dashf(buf))
		if (useindexfile(buf) < 0)
			return -1;
	sprintf(buf, "boards/%s/.DINGDIR", bhp->filename);
	if (dashf(buf))
		if (useindexfile(buf) < 0)
			return -1;

	tpl_num = orig_tmpl_init(bhp->filename, 1, &tpl);
	if( tpl_num > 0 ){
		for(i=0; i<tpl_num; i++){
			if( tpl[i].tmpl->filename[0] )
				countfile_new(tpl[i].tmpl->filename);
		}
	}
	if( tpl_num >= 0) orig_tmpl_free(&tpl, tpl_num);

	sprintf(buf, "boards/%s", bhp->filename);
	rm_lost(buf);
	return 0;
}

int
main(int argc,char **argv)
{
	struct boardheader bh;

	chdir(BBSHOME);
	resolve_boards();
	
	printf("find_rm_lost is running~\n");
	if(argc > 1){
    	strncpy(bh.filename,argv[1],STRLEN);
    	find_rm_lost(&bh,NULL);
	}else
		apply_boards(find_rm_lost,NULL);
	printf("allfile %d, allref %d, alllost %d\n", allfile, allref, alllost);
	printf("unknownfn %d, nindexitem %d, nstrangeitem %d\n", unknownfn,
	       nindexitem, nstrangeitem);
	printf("totalsize = %d", totalsize);
	return 0;
}

