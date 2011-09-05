#include <time.h>
 
int main(){
	    time_t t;
		    time(&t);
			    printf("%d\t%s\n",t,ctime(&t));
}
