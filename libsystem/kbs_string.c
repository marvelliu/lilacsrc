/* ʹ��Boyer-Moore-Horspool-Sunday �㷨�����ַ���ƥ���ϵ�к���
�㷨���:BOYER, R., and S. MOORE. 1977. "A Fast String Searching Algorithm." 
			 HORSPOOL, R. N. 1980. "Practical Fast Searching in Strings."
			  Software - Practice and Experience, 10, 501-06. Further improvements by HUME, A., and D. M. SUNDAY. 1991. 
   2002.08.20 ���� KCN
*/
#include "system.h"
/* �ַ������Һ���*/


/* �ڴ�ƥ�亯��memfind
*/
static void *memfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              int *init)
{                               /* �Ƿ���Ҫ��ʼ����λ�� */
    size_t byte_nbr,            /* Distance through block */
     match_size,                /* Size of matched part */
     limit;
    const unsigned char *match_ptr = NULL;
    const unsigned char *block = (unsigned char *) in_block,    /* Concrete pointer to block data */
    *pattern = (unsigned char *) in_pattern;    /* Concrete pointer to search value */

    if (block == NULL || pattern == NULL || shift == NULL)
        return (NULL);

/* ���ҵĴ���Ӧ��С�� ���ݳ���*/
    if (block_size < pattern_size)
        return (NULL);

    if (pattern_size == 0)      /* �մ�ƥ���һ�� */
        return ((void *) block);

/* ���û�г�ʼ����������λ��*/
    if (!init || !*init) {
        for (byte_nbr = 0; byte_nbr < 256; byte_nbr++)
            shift[byte_nbr] = pattern_size + 1;
        for (byte_nbr = 0; byte_nbr < pattern_size; byte_nbr++)
            shift[(unsigned char) pattern[byte_nbr]] = pattern_size - byte_nbr;

        if (init)
            *init = 1;
    }

/*��ʼ�������ݿ飬ÿ��ǰ����λ���е�����*/
    limit = block_size - pattern_size + 1;
    for (byte_nbr = 0; byte_nbr < limit; byte_nbr += shift[block[byte_nbr + pattern_size]]) {
        if (block[byte_nbr] == *pattern) {
            /*
             * �����һ���ֽ�ƥ�䣬��ô����ƥ��ʣ�µ�
             */
            match_ptr = block + byte_nbr + 1;
            match_size = 1;

            do {
                if (match_size == pattern_size)
                    return (void *) (block + byte_nbr);
            } while (*match_ptr++ == pattern[match_size++]);
        }
    }
    return NULL;
}

/* ��Сд�����е�ƥ�亯��txtfind
*/
static void *txtfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              int *init)
{                               /* �Ƿ���Ҫ��ʼ����λ�� */
    size_t byte_nbr,            /* Distance through block */
     match_size,                /* Size of matched part */
     limit;
    const unsigned char *match_ptr = NULL;
    const unsigned char *block = (unsigned char *) in_block,    /* Concrete pointer to block data */
    *pattern = (unsigned char *) in_pattern;    /* Concrete pointer to search value */

    if (block == NULL || pattern == NULL || shift == NULL)
        return (NULL);

/* ���ҵĴ���Ӧ��С�� ���ݳ���*/
    if (block_size < pattern_size)
        return (NULL);

    if (pattern_size == 0)      /* �մ�ƥ���һ�� */
        return ((void *) block);

/* ���û�г�ʼ����������λ��*/
    if (!init || !*init) {
        for (byte_nbr = 0; byte_nbr < 256; byte_nbr++)
            shift[byte_nbr] = pattern_size + 1;
        for (byte_nbr = 0; byte_nbr < pattern_size; byte_nbr++)
            shift[(unsigned char) tolower(pattern[byte_nbr])] = pattern_size - byte_nbr;

        if (init)
            *init = 1;
    }

/*��ʼ�������ݿ飬ÿ��ǰ����λ���е�����*/
    limit = block_size - pattern_size + 1;
    for (byte_nbr = 0; byte_nbr < limit; byte_nbr += shift[tolower(block[byte_nbr + pattern_size])]) {
        if (tolower(block[byte_nbr]) == tolower(*pattern)) {
            /*
             * �����һ���ֽ�ƥ�䣬��ô����ƥ��ʣ�µ�
             */
            match_ptr = block + byte_nbr + 1;
            match_size = 1;

            do {
                if (match_size == pattern_size)
                    return (void *) (block + byte_nbr);
            } while (tolower(*match_ptr++) == tolower(pattern[match_size++]));
        }
    }
    return NULL;
}

char *bm_strstr(const char *string, const char *pattern)
{
    size_t shift[256];
    int init = 0;

    return (char *) memfind(string, strlen(string), pattern, strlen(pattern), shift, &init);
}

/* �ַ������ƥ�亯��*/
char *bm_strstr_rp(const char *string, const char *pattern, size_t * shift, int *init)
{
    return (char *) memfind(string, strlen(string), pattern, strlen(pattern), shift, init);
}

/* �ַ�����Сд�����е�ƥ�亯��*/
char *bm_strcasestr(const char *string, const char *pattern)
{
    size_t shift[256];
    int init = 0;

    return (char *) txtfind(string, strlen(string), pattern, strlen(pattern), shift, &init);
}

/* �ַ�����δ�Сд������ƥ�亯��*/
char *bm_strcasestr_rp(const char *string, const char *pattern, size_t * shift, int *init)
{
    return (char *) txtfind(string, strlen(string), pattern, strlen(pattern), shift, init);
}


#ifndef HAVE_STRCASESTR
char *strcasestr(const char *haystack, const char *needle)
{
    return bm_strcasestr(haystack, needle);
}
#endif /* HAVE_STRCASESTR */



#ifndef HAVE_MEMMEM
void *memmem(const void *s, size_t slen, const void *p, size_t plen)
{
    size_t shift[256];
    int init = 0;
    return memfind(s, slen, p, plen, shift, &init);
}
#endif /* HAVE_MEMMEM */



#ifndef HAVE_STRSEP
char* strsep(char **strptr,const char *delim){
    char *ptr;
    if(!(ptr=*strptr))
        return NULL;
    if(!*delim){
        *strptr=NULL;
        return ptr;
    }
    if(!(*strptr=(!*(delim+1)?strchr(*strptr,*delim):strpbrk(*strptr,delim))))
        return ptr;
    *((*strptr)++)=0;
    return ptr;
}
#endif /* HAVE_STRSEP */

