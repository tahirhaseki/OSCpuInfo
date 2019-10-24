#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>
#include<linux/kernel.h>

void param2(){
  FILE *fp;
  
  fp = fopen ("/proc/uptime", "r" );
  if( !fp ) perror("/proc/uptime"),exit(1);

  float booted;
  float idle;
  
  fscanf(fp,"%f %f",&booted,&idle);
  fclose(fp);
  fp = fopen("/proc/loadavg","r");
  if( !fp ) perror("/proc/loadavg"),exit(1);
  char proccesses[255];
  fgets(proccesses,255,fp);
  int i;
  // Split Data
  char delim[] = " ";
  char **splitData = malloc(2000);
  char *ptr2 = calloc(2000,1);

  i = 0;
  // Splitting rows.
  ptr2 = strtok(proccesses,delim);
  while(ptr2 != NULL){
    splitData[i] = calloc(strlen(ptr2)+1,1);
    strcpy(splitData[i],ptr2);
    i++;
    ptr2 = strtok(NULL,delim);
  }
  ptr2 = strtok(splitData[3],"/");
  char* active;
  char* total;
  active = calloc(strlen(ptr2)+1,1);
  strcpy(active,ptr2);
  ptr2 = strtok(NULL,"/");
  total = calloc(strlen(ptr2)+1,1);
  strcpy(total,ptr2);
  fclose(fp);

  printf("System Statistics:\n");
  int hours = booted / 360;
  int mins = ((int)booted % 360) / 60;
  int seconds = booted - (hours * 360 + mins * 60);
  printf("\tSystem was booted since : %d Hours %d Minutes %d Seconds\n",hours,mins,seconds);
  hours = idle / 360;
  mins = ((int)idle % 360) / 60;
  seconds = idle - (hours * 360 + mins * 60);
  printf("\tSystem has been idle since : %d Hours %d Minutes %d Seconds\n",hours,mins,seconds);
  printf("\tThe total number of active tasks : %s\n",active);
  printf("\tThe total number of processes : %s\n",total);
}

void param1(const char* proccessId){
  char* command = calloc(40,1);
  strcpy(command,"ps -u -h --pid ");
  strcat(command,proccessId);
  strcat(command," > temp");
  system(command);
  FILE *fp;
  char *buffer = malloc(512);

  char c;

  fp = fopen ("temp", "r" );
  if( !fp ) perror("temp"),exit(1);

  char *rdLine;
  rdLine = malloc(1000);
  int i = 0;

  char delim[] = " ";
  char **splitData = malloc(2000);
  char *ptr = calloc(2000,1);
  ptr = strtok(rdLine, delim);
  char *ptr2 = calloc(2000,1);
  i = 0;

  int counter = 0;
  i = 0;
  // Resetting rdLine
  free(rdLine);
  rdLine = calloc(1000,1);
  // Reading line/row.
  while((c = fgetc(fp)) != '\n' && c != EOF) {
      *(rdLine+i) = c;
      i++;
  }
  *(rdLine+i) = '\0';

  i = 0;
  // Splitting rows.
  ptr2 = strtok(rdLine,delim);
  while(ptr2 != NULL){
    splitData[i] = calloc(strlen(ptr2)+1,1);
    strcpy(splitData[i],ptr2);
    i++;
    if(i == 10)
      ptr2 = strtok(NULL,"!!!");
    else
    {
      ptr2 = strtok(NULL,delim);
    }
  }
  fclose(fp);

  system("rm temp");
  printf("Process Information:\n");
  printf("\tuser : %s\n",splitData[0]);
  printf("\tpid : %s\n",splitData[1]);
  printf("\tstart_time : %s\n",splitData[8]);
  printf("\tcommand : %s\n",splitData[10]);
  // 0 -> root
  // 1 -> pid
  // 8 -> start time
  // 10 -> command

}

void cpuinfo(){
	char * fileName = "/proc/cpuinfo";
  FILE *fp;
  char *buffer = malloc(512);

  char c;

  fp = fopen (fileName, "r" );
  if( !fp ) perror(fileName),exit(1);

  char *rdLine;
  rdLine = malloc(1000);
  int i = 0;

  char delim[] = ":";
  char **header = malloc(2000);
  char **splitData = malloc(2000);
  char *ptr = calloc(2000,1);
  ptr = strtok(rdLine, delim);
  char *ptr2 = calloc(2000,1);
  i = 0;

  int counter = 0;
  printf("CPU Information:\n");
  while(counter < 7){
    i = 0;
    // Resetting rdLine
    free(rdLine);
    rdLine = calloc(1000,1);
    // Reading line/row.
    while((c = fgetc(fp)) != '\n' && c != EOF) {
            *(rdLine+i) = c;
            i++;
    }
    *(rdLine+i) = '\0';
		if(counter != 3 && counter != 5){
			printf("\t%s\n",rdLine);
		}
    i = 0;
		counter++;
  }

  fclose(fp);
  free(buffer);
  free(header);
  free(ptr);
  free(ptr2);
  free(splitData);
}

/* asmlinkage long sys_systeminfo(int argc,char* argv[]){
  cpuinfo();
  if(argc == 2){
    param2();
  }
  else if(argc == 3){
    param1(argv[2]);
  }
  else if(argc == 4){
    if(strcmp(argv[1],"-p") == 0){
      param1(argv[2]);
      param2();
    }
    else {
      param2();
      param1(argv[3]);
    }
  }
  return 0;
} */

int main(int argc, char* argv[]){
  cpuinfo();
  if(argc == 2){
    param2();
  }
  else if(argc == 3){
    param1(argv[2]);
  }
  else if(argc == 4){
    if(strcmp(argv[1],"-p") == 0){
      param1(argv[2]);
      param2();
    }
    else {
      param2();
      param1(argv[3]);
    }
  }
	return 0;
}