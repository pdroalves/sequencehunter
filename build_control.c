/*################################################################################################################
#        Developer: Pedro Alves - pdroalves@gmail.com - 14/08/2012
#	 
#		 You need to user a file name to save your actual build version. It will create historys
#	in a seconde file tagged with "_history".
#		You can add this file to a build script and compile with your application. Remember to first
#	execute this to update build version before compile your app.
#
#
#	Example:
#		./build_control build
#
#	It will increase the build value in build.txt file and save a history to build_history.txt. 
#################################################################################################################*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print(FILE *f,int build){
 
	struct tm *local;
	time_t t;
	char *tempo;
	
	t = time(NULL);
	local = localtime(&t);
	//tempo = (char*)malloc(100*sizeof(char));
	//strcpy(tempo,asctime(local));
				
	fprintf(f,"const int build =  %d;",build);
	return;
}

int main(int argc,char *argv[]){

	int build;
	char *tmp;
 	char *buildfilename;
	char *buildhistoryfilename;

	if(argc > 1){

		FILE *build_control;
		FILE *build_history;

		buildfilename = (char*)malloc(100*sizeof(char));
		strcpy(buildfilename,argv[1]);
		strcat(buildfilename,".h");
		build_control = fopen(buildfilename,"a+");
		
		buildhistoryfilename = (char*)malloc(100*sizeof(char));
		
		strcpy(buildhistoryfilename,argv[1]);
		strcat(buildhistoryfilename,"_history.txt");
		build_history = fopen(buildhistoryfilename,"a+");

		if(build_control != NULL){
			if(feof(build_control) != 0){
				print(build_control,0);
				print(build_history,0);
			}else{
				tmp = (char*)malloc(100*sizeof(char));
				fscanf(build_control,"const int build =  %d;",&build);
				
				fclose(build_control);
				build_control = fopen(buildfilename,"w+");
				print(build_control,build+1);
				print(build_history,build+1);
			}	
			fclose(build_control);
		}
	}
	return 0;
}
