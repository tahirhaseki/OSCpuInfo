#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]){
	long int amma;
	if(argc == 2){
		printf("1");
		amma =syscall(335,0,"0");
	}
	else if(argc == 3){
		printf("2");
		amma =syscall(335,0,argv[2]);
	}
	else if(argc == 4){
		if(strcmp(argv[1],"-p") == 0){
			printf("3");
			amma =syscall(335,1,argv[2]);
		}
		else {
			printf("4");
			amma = syscall(335,2,argv[3]);
		}
	}
	return 0;
}
