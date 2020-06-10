//#include "shell.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include<sys/wait.h>
#define del ";"
#define del1 " \n\t"
char bgProcess[100000][300];
int flagz =0;
int flagc =0;
int bgcount;
int mainPID;
int childPID;

//#include<readline/readline.h> 
//#include<readline/history.h
char home[1024];
int bg;
void piping(char **args)
{
	long long i=0,pipes=0;
	//counting number of pipes
	for(i=0;args[i]!=NULL;i++)
	{
		if(!strcmp(args[i],"|"))
			pipes++;
	
	}
	long long sz=i;
	i=0;
	long long j=0,finish=0;
	long long cnt=0;
	char *cur_com[1000];
	long long frd;
	while(1)
	{
		if(finish==1)break;
		long long p=0;
		if(args[i]==NULL)break;

		while(1)
		{
			if(args[j]==NULL)
			{
				finish=1;
				break;
			}
			if(strcmp(args[j],"|")==0)
			{
				break;
			}
			else
			{
			cur_com[p]=args[j];
				p++;	
			} 
			if(args[j]==NULL)
			{
				finish=1;
				break;
			}
			j++;
		}
		j++;
		int fd[2];
		int in,out;
		cur_com[p]=NULL;
		cnt++;
		long long z=0;
		if(cnt==1)
		{	
			if(pipe(fd)==-1)
			{
				perror("piping error :");
				return;
			}
			long long z=0;
			char infile[1000];
			for(z=0;cur_com[z]!=NULL;z++)
			{
				// printf("%s\n",cur_com[z] );
				if(strcmp(cur_com[z],"<")==0)
				{
					cur_com[z]=NULL;
					strcpy(infile,cur_com[z+1]);
					// long long zz=0;
					// while(cur_com[zz]!=NULL)
					// {
						// printf("%s\n",cur_com[zz] );
						// zz++;
					// }
					in=dup(0);
			   		frd = open(infile,O_RDONLY, 0644);
					dup2(frd,0);
				}
			}
			dup2(fd[1],1);
			close(fd[1]);
			out=dup(1);

		}
		else if(finish==1)	
		{
			// redirect
			long long z=0;
			char outfile[1000];
			long long flagg=0;
			for(z=0;cur_com[z]!=NULL;z++)
			{
				// printf("%s\n",cur_com[z] );
				if(strcmp(cur_com[z],">>")==0)
				{
					cur_com[z]=NULL;
					strcpy(outfile,cur_com[z+1]);
					out=dup(1);
					frd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
					dup2(frd,1);
					close(frd);
					flagg=1;
				}
				else if(strcmp(cur_com[z],">")==0)
				{	cur_com[z]=NULL;
					strcpy(outfile,cur_com[z+1]);
					out=dup(1);
					frd= open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(frd,1);
					close(frd);
					flagg=1;
					
				}
			}
			dup2(fd[0],0);
			close(fd[0]);
			in = dup(0);
			
		}
		else
		{
			in = dup(0);
			dup2(fd[0],0);
			if(pipe(fd)==-1)
			{
				perror("piping error :");
				return;
			}
			out=dup(1);
			dup2(fd[1],1);
			close(fd[1]);
		}
		pid_t pid;
		pid=fork(); 

		if(pid==-1)
		{
			perror("fork error : ");
			return;
		}
		else if(!pid)
		{
			if(execvp(cur_com[0],cur_com)==-1)
			{
				perror("command not found");
				exit(EXIT_FAILURE);
			}
		} 
		else
		{
			wait(NULL);
			dup2(in,0);
			dup2(out,1);
		}

	}
}
void redirect(char **arg,int inp,int out,int append)
{

long long i=0,pos=100000000;
i=0;
//while(arg[i]!=NULL)
//	printf("%s\n",arg[i++]);

char in[10000],outt[10000];
if(inp)
{
	i=0;
	while(arg[i]!=NULL)
	{
		if(!strcmp(arg[i],"<"))
		{
			strcpy(in,arg[i+1]);
			if(i<pos)
				pos=i;


		}
		i++;
	}

}
//printf("%lld\n",pos);
if(out+append)
{
	i=0;
	while(arg[i]!=NULL)
	{
		if(!strcmp(arg[i],">"))
		{
			strcpy(outt,arg[i+1]);
			if(i<pos)
			pos=i;
		}
		i++;
	}
}
arg[pos]=NULL;
//printf("%lld\n",pos);

//i=0;
///while(arg[i]!=NULL)
//	printf("%s\n",arg[i++]);
long long file[2];
long long pin,pout;
//i=0;
//while(arg[i]!=NULL)
//	printf("%s\n",arg[i++]);

pid_t pp;
pp=fork();
if(pp>0)
{
	wait(NULL);
}
else if(pp==-1)
{
//	close(file[1]);
	perror("fork error: ");
	return ;
}
else
{	struct stat ch;
if(inp)
{
	if(stat(in,&ch)!=-1)
	{
		pin=dup(0);
		file[0]=open(in,O_RDONLY,0644);
		dup2(file[0],0);
	}
	else
		perror("file don't exist");

}
if(out+append)
{
	pout=dup(1);
	if(out==1)
		file[1]=open(outt, O_WRONLY | O_CREAT | O_CREAT | O_TRUNC,0644);
	else
		file[1]=open(outt, O_WRONLY | O_CREAT | O_CREAT | O_APPEND,0644);

	if(dup2(file[1],1)==-1)
	{
		perror("dup2 failed");
		return ;
	}
}
	if(execvp(arg[0],arg)==-1)
	{
		perror("command not found");
		exit(EXIT_FAILURE);
	}
	close(file[0]);
	close(file[1]);
}
}
void bg1(char *s)
{
    if(s == NULL){
        perror("Invalid No. of arguments.\n");
        return;
    }
    if(atoi(s) <= bgcount)
    {
    int j = 0;
    for(int i= 0;i<100000;i++)
    {
        if(bgProcess[i][0] != '\0')
        {
            j++;
            if(j == atoi(s)){
                bgcount++;
                kill(i, SIGCONT);
                bgProcess[i][0] = '\0';

                return;
            }
        }
    }
    }
    else
    {
         perror("INVALID JOB ID\n");
        return;
    }
}
void exec_c(int sig)
{
    flagc=1;
}
void exec_z(int sig)
{
    flagz=1;
}
void fg(char *s)
{
    int status;
    if(s == NULL)
    {
        perror("Invalid No. of Arguments.\n");
        return;
    }
    if(atoi(s) <= bgcount)
    {
          
        int j = 0;
        int i=0;
        int x = atoi(s);
        while(i<100000)
        {
            if(bgProcess[i][0] != '\0')
            {
                j++;
                if(j == x)
                {
                    kill(i, SIGCONT);
                    flagc = 0;
                    flagz = 0;
                    signal(SIGTSTP, exec_z);
                    signal(SIGINT, exec_c);
                    while (flagz == 0 && flagc==0 && waitpid(i,&status,WNOHANG) != i);
                
                    if(flagz==1)
                    {
                        setpgid(i,i);
                        kill(i,SIGSTOP);
                        signal(SIGCHLD, SIG_IGN);
                    }
                    if(flagc==1){
                        kill(i,9);
                        wait(NULL);
                    }
                    flagc = 0;
                    flagz = 0;
                    return;
                }
               }
            i++;
        }
    }
    else
    {
          perror("INVALID JOB ID\n");
        return;

    }
}

void overkill()
{
    int i=0;
    while(i<100000)
    {
        if(bgProcess[i][0]!='\0')
        {
            kill(i,9);
            bgProcess[i][0]='\0';
            bgcount--;
        }
    }
}


void jobs()
{
    int i = 0;
    int j = 1;
    while(i<100000)
    {
        if(bgProcess[i][0]!='\0')
        {
    char name[1024];
            
            printf("[%d] ",j);
            sprintf(name, "/proc/%d/stat", i);

            char c[1024];
            
            char a[1024];
            char b[1024];
            int k=0;
            int f=open(name,O_RDONLY);
            if(f)
            {
                //int i = 0;
                char *buff=malloc(1000);
                read(f,buff,100);
                //fscanf(f, "%s %s %s",a,b,c);

                close(f);
                int tok=0;
                for(int l=0;l<100;l++)
                {
                	if(buff[i]==' ')
                		tok++;

                }
                if(tok==2)
                {
                	if(buff[i]=='T')
                		printf("Stopped ");
                	else if(buff[i]=='Z')
                		printf("Killed");
                	else if(buff[i]=='R')
                		printf("Running");
                	else 
						printf("Unknown");
					break;
         
                }

            }
           
            while(k<250)
            {
                if(bgProcess[i][k] == ' ')
                    break;
                printf("%c",bgProcess[i][k]);
                k++;
            }
            printf("[%d]\n",i);
            i++;
            j++;
            }       
    }
}

void childSignalHandler(int sign)
{
    int stat;
    int i=0;
    while(i<100000)
    {
        if(bgProcess[i][0]!='\0')
        {
            if(waitpid(i,&stat,WNOHANG))
            {   bgcount--;
                printf("\n%s\n",bgProcess[i]);
                bgProcess[i][0]='\0';

            }
        }
    }
    return;
}
void kjob(char **s)
{
    if(atoi(s[1]) > bgcount)
    {
        perror("INVALID JOB ID\n");
        return;
    }


    if((s[1] == NULL || s[2] == NULL) && s[3] != NULL){
        perror("Invalid No. of arguments.\n");
        return;
    }
    int j = 1;
    int i = 0;
    int l;
   
    while(i<100000)
    {
        if(bgProcess[i][0] != '\0')
        {   l=atoi(s[2]);
            if(j == atoi(s[1]))
                kill(i,l);
            j++;
        }
        i++;
    }
}


int cd(char **arg)
{
	if(arg[1]==NULL || strcmp(arg[1],"&")==0)
		chdir(home);
	else
	{
		if(chdir(arg[1])!=0)
			perror("Unable to run command");
	}
	return 1;
}
int pwd(char **arg)
{
	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	printf("%s\n",cwd);
	return 1;
}
int echo(char **arg)
{
	int i=1;
	if(arg[i]==NULL)
		printf("\n");
	while(arg[i]!=NULL)
	{
		printf("%s ",arg[i]);
		i++;
	}
	//printf("\n");
	return 1;
}
/*void cronjob(char** arg)
{

}*/
void ls(char **arg)
{
	char *path=(char*)malloc(sizeof(char)*1024);
char *tmp=(char*)malloc(sizeof(char)*1024);
//	char path[1024];
//	char tmp[1024];
	int pn=1;
	int flg_a=0;
	int flg_l=0;
	int flg_pth=0;
	//printf("chk0\n");
	while(arg[pn])
	{
		if(arg[pn][0]=='-')
		{
		if(arg[pn][1]=='a')
			flg_a=1;
		if(arg[pn][1]=='l')
			flg_l=1;
		if(arg[pn][2]=='a')
			flg_a=1;
		if(arg[pn][2]=='l')
			flg_l=1;

		}
		else 
		{
			if(arg[pn]!=NULL && strcmp(arg[pn],"&")!=0)
			{
				flg_pth=1;
				strcpy(tmp,arg[pn]);

		
			}
			

		}
		pn++;
	}
		if(flg_pth)
	{	strcpy(path,tmp);
	getcwd(tmp,1024);
	strcat(tmp,"/");
	strcat(tmp,path);
	strcpy(path,tmp);
}

	else
	getcwd(path,1024);
//	getcwd(tmp,1024);
//	if(path[0]=='~')
//	{
//	strcat(tmp,"/");
//	strcat(tmp,path);
//	strcpy(path,tmp);
//	}*/
	//printf("%s",path);
	//printf("chk\n");
	struct stat st;
	stat(path,&st);
	if(!S_ISDIR(st.st_mode))
	{
		perror("NOT A DIRECTORY\n");
		return;
	}
	
	struct dirent* dst;
	struct file_data{
		char* f_permissions;
		nlink_t f_nhlinks;
		char* f_uown;
		char* f_gown;
		off_t f_size;
		char* f_mtime;
		char* f_name;
	};
	DIR *dir=opendir(path);
	int it = 0, f_sz = 64;
	struct file_data* files = (struct file_data*)malloc(f_sz*sizeof(struct file_data));
	if (files == NULL)
	{
		printf("Allocation Error\n");
		exit(EXIT_FAILURE);
	}
	//printf("chk2\n");
	while (dst = readdir(dir))
	{
		files[it++].f_name = dst->d_name;
		if (it >= f_sz)
		{
			f_sz <<= 1;
			files = realloc(files, f_sz);
			if (files == NULL)
			{
				printf("Reallocation Error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	//printf("chk3\n");
	it = 0;
	while (files[it].f_name)
	{
		char* s = (char*)malloc((strlen(path)+strlen(files[it].f_name)+5)*sizeof(char));
		struct stat st;
		strcpy(s, path);
		if (s[(int)strlen(s)-1] != '/') 
		strcat(s,"/");
		strcat(s, files[it].f_name);
		if(stat(s, &st)==-1)
		{
			printf("Error getting file info\n");
			exit(EXIT_FAILURE);
		}
		
		files[it].f_permissions = (char*)malloc(sizeof(char)*11);
		if (S_ISREG(st.st_mode)) files[it].f_permissions[0] = '-';
		else if (S_ISCHR(st.st_mode)) files[it].f_permissions[0] = 'c';
		else if (S_ISDIR(st.st_mode)) files[it].f_permissions[0] = 'd';
		else if (S_ISBLK(st.st_mode)) files[it].f_permissions[0] = 'b';
		else if (S_ISLNK(st.st_mode)) files[it].f_permissions[0] = 'l';
		else if (S_ISFIFO(st.st_mode)) files[it].f_permissions[0] = 'p';
		else files[it].f_permissions[0] = 's';

		if (!(st.st_mode & S_IRUSR)) files[it].f_permissions[1] = '-';
		else files[it].f_permissions[1] = 'r';
		if (!(st.st_mode & S_IWUSR)) files[it].f_permissions[2] = '-';
		else files[it].f_permissions[2] = 'w';
		if (!(st.st_mode & S_IXUSR)) files[it].f_permissions[3] = '-';
		else files[it].f_permissions[3] = 'x';

		if (!(st.st_mode & S_IRGRP)) files[it].f_permissions[4] = '-';
		else files[it].f_permissions[4] = 'r';
		if (!(st.st_mode & S_IWGRP)) files[it].f_permissions[5] = '-';
		else files[it].f_permissions[5] = 'w';
		if (!(st.st_mode & S_IXGRP)) files[it].f_permissions[6] = '-';
		else files[it].f_permissions[6] = 'x';

		if (!(st.st_mode & S_IROTH)) files[it].f_permissions[7] = '-';
		else files[it].f_permissions[7] = 'r';
		if (!(st.st_mode & S_IWOTH)) files[it].f_permissions[8] = '-';
		else files[it].f_permissions[8] = 'w';
		if (!(st.st_mode & S_IXOTH)) files[it].f_permissions[9] = '-';
		else files[it].f_permissions[9] = 'x';

		files[it].f_nhlinks = st.st_nlink;
		files[it].f_permissions[10] = '\0';
		
		files[it].f_gown = getgrgid(st.st_gid)->gr_name;
		files[it].f_uown = getpwuid(st.st_uid)->pw_name;
		files[it].f_mtime = (char*)malloc(sizeof(char)*100);
		files[it].f_size = st.st_size;
		strftime(files[it].f_mtime, 100, "%b %d %H:%M", localtime(&st.st_mtime));
		it++;
		free(s);
	}
	//printf("chk4\n");
	
	it = 0;
	while (files[it].f_name)
	{
		if (!flg_a && files[it].f_name[0] == '.')
		{
			it++;
			continue;
		}
		
		if(!flg_l) printf("%s\n", files[it].f_name);
		else printf("%s\t%d\t%s\t%s\t%d\t%s\t%s\n", files[it].f_permissions, (int)files[it].f_nhlinks, files[it].f_uown, files[it].f_gown, (int)files[it].f_size, files[it].f_mtime, files[it].f_name);
		it++;
	}
	//printf("chk5\n");
	free(files);
}
void unsetenv1(char **arg)
{
	if(arg[0]==NULL || arg[3]!=NULL || arg[1]==NULL)
	{
		printf("Invalid arguments\n");
		return;
	}
	if(unsetenv(arg[1])!=0)
	{
		printf("error:unsetenv error\n");
		return;
	}


}
void setenv1(char **arg)
{

	if(arg[2]==NULL)
		arg[2]=" ";
	if(arg[0]==NULL || arg[3]!=NULL)
	{
		printf("Invalid arguments\n");
		return;
	}
	if(setenv(arg[1],arg[2],1)!=0)
	{
		printf("error:setenv error\n");
		return;
	}
}

int background(char **arg)
{
int pp=fork();
	if(pp>0)
	{
		//signal(SIGCHLD,SIG_IGN);
		strcpy(bgProcess[pp],arg[0]);
		bgcount++;
		return 1;
	}
	else if(pp<0)
	{
		perror("ERROR:Forking background process failed");
	}
	else
	{
		int pp1=fork();
		if(pp1>0)
		{
			int var=0;
			int status;
			for(;arg[var]!=NULL;var++)
				{	if(strcmp(arg[var],"&"))
				printf("%s ",arg[var]);
			}
			printf("with PID %d has been initiated successfully\n",pp1);
			int cid=waitpid(pp1,&status,0);			
			if(WIFEXITED(status)==1)
			{
				int exitstatus=WEXITSTATUS(status);
				printf("process ' %s ' with pid %d exited normally with exit status:%d\n",arg[0],cid,exitstatus);
			}
			else
			{
				printf("process ' %s ' with pid %d exited abnormally\n",arg[0],cid); 
			}
						
		}
		else if(pp1<0)
			perror("ERROR:Forking background process failed");
		else
		{
			int var=0;
			execvp(arg[0],arg);
		}
	} 
}
	
	char **khel1(char *line)
	{
		char **lline;

		lline=malloc(1024*sizeof(char *));
		char *line1=(char *)malloc(1024*sizeof(char));
		if(!lline)
		{
			perror("Allocation error");
		}

		//printf("ADD2\n");
		int pos=0;
		line1=strtok(line,del1);
		while(line1!=NULL)
		{
		lline[pos++]=line1;
		line1=strtok(NULL,del1);
		}
		lline[pos]=NULL;
		free(line1);

		//printf("ADD2\n");
		return lline;



	}
	char **khel(char *line)
	{
		//printf("ADD\n");
		char **lline;

		lline=malloc(1024*sizeof(char *));
		char *line1=(char *)malloc(1024*sizeof(char));
		if(!lline)
		{
			perror("Allocation error");
		}

		//printf("ADD2\n");
		int pos=0;
		line1=strtok(line,del);
		while(line1!=NULL)
		{
		lline[pos++]=line1;
		line1=strtok(NULL,del);
		}
		lline[pos]=NULL;
		free(line1);

		//printf("ADD2\n");
		return lline;

	}
	void pinfo(char **arg)
	{
		int pid;
		char path[1000];
		char pathexe[1000];

		char spid[1000];
		char buff[1000];
		
		char* line=NULL;
		buff[0]='\0';
		if(arg[1]==NULL)
		{
		pid=getpid();
		sprintf(spid,"%d",pid);
		}
		else
		strcpy(spid,arg[1]);
		strcpy(path,"/proc/");
		strcat(path,spid);
		strcpy(pathexe,path);
		strcat(pathexe,"/exe");
		strcat(path,"/stat");
		//printf("%arg %s",path,pathexe);
		FILE* fil=fopen(path,"r");
		if(fil ==NULL)
		{
			perror("NO SUCH PROCESS\n");
			return;
		}
		int cnt=1;
		char ll;
    	printf("PID:%s",spid);
    		ll=fgetc(fil);
    	
    	while(ll!=EOF)
    	{	//printf("%c",ll);
    		//printf("%c",ll);
    		ll=fgetc(fil);
    		if(ll==' ')
    		{	cnt++;
    			if(cnt==3)
    				printf("\nProcess Status:");
    			else if(cnt==23)
    				printf("\nMemory:");
    		}
    		else if(cnt==3)
    			printf("%c",ll);
    		else if(cnt==23)
    			printf("%c",ll);



    	}
    	fclose(fil);
    	int rn=readlink(pathexe,buff,sizeof(buff));
    	if(rn==-1)
    		printf("\nCannot access the executable path\n");
    	else
    		printf("\nExecutable Path:%s\n,",buff);

	}

int main()
{	//char home[1024];
	//signal(SIGCHLD, bgChecker);
	bg=0;

	for( int i = 0;i<100000;i++)
	bgProcess[i][0] = '\0';
	bgcount=0;
	int inp=0,out=0,append=0,pipes=0;
	char cwd[1024];
	char buf[1024];
	char buf1[1024];
	mainPID = getpid();

	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
signal(SIGINT, SIG_IGN);
	char path[1024];
	char *line;
	char *buf2=(char *)malloc(1024*sizeof(char));
		int ite;
		getcwd(home,sizeof(home));

	//ecec_fg("clear");
	while(1)
	{
		childPID = -1;
		signal(SIGCHLD, childSignalHandler);
		getcwd(cwd,sizeof(cwd));
		//printf("%s ",home);
		getlogin_r(buf,sizeof(buf));
		gethostname(buf1,sizeof(buf1));
		if(strlen(cwd)<strlen(home))
		strcpy(path,cwd);
		else
		{	path[0]='~';
			ite=1;
			//printf("%ld %ld",strlen(home),strlen(cwd));
			for(int i=strlen(home);i<strlen(cwd);i++)
			path[ite++]=cwd[i];
			//printf("%c",path[ite-1]);}
			path[ite]='\0';
		}
		printf("<%s@%s:%s>",buf,buf1,path);
		fgets(buf2,1024,stdin);
		//printf("%s",buf2);
		char **commands;
		char **command;
		//printf("%s",buf2);

		command=malloc(10000);
		//printf("%s",buf2);

		commands=malloc(10000);
		//printf("%s",buf2);

		commands=khel(buf2);
		//printf("%s",buf2);
		//printf("%s%s%s",commands[0],commands[1],commands[1]);
		///	printf("lol%s\nlol\n",command[1]);

			
		for(int i=0;commands[i]!=NULL;i++)
		{	
			bg=0;
			command=khel1(commands[i]);
			int k=0;
			while(command[k]!=NULL)
					{
						if(!strcmp(command[k],"&"))
						bg=1;	
						k++;					
					}
					k=0;
			while(command[k]!=NULL)
				{
					if(!strcmp(command[k],">"))
						out=1;
					if(!strcmp(command[k],"<"))
						inp=1;
					if(!strcmp(command[k],">>"))
						append=1;
					if(!strcmp(command[k],"|"))
						pipes=1;
					k++;
					//printf("%d\n",bg);
				}
			//int 	si=0;
//while(command[si]!=NULL)
	//printf("%s\n",command[si++]);

				if(pipes)
					piping(command);
				else if(out+inp+append)
					{
					redirect(command,inp,out,append);
					continue;}
		//	printf("%s%s",command[1],command[1]);
			
			else if(!strcmp(command[0],"jobs"))
			{
				jobs();
			}
			else if(!strcmp(command[0],"kjob"))
			{
				kjob(command);
			}
			else if(!strcmp(command[0],"bg"))
			{
				bg1(command[1]);
			}
			else if(!strcmp(command[0],"fg"))
			{
				fg(command[1]);
			}
			else if(!strcmp(command[0],"overkill"))
			{
				overkill();
			}
			else if(!strcmp(command[0],"setenv"))
			{
				setenv1(command);
			}
			else if(!strcmp(command[0],"unsetenv"))
			unsetenv1(command);
		else if(!strcmp(command[0],"ls"))
				{	//printf("chk*\n");
				ls(command);
			}
			else if(!strcmp(command[0],"cd")){
		//		printf("ADD5.5\n");
				cd(command);
		//		printf("ADD6\n");

			}
			else if(!strcmp(command[0],"pwd"))
			{	//printf("AA\n");
				pwd(command);

			}
			 else if(!strcmp(command[0],"echo"))
				echo(command);
			else if(!strcmp(command[0],"pinfo"))
				pinfo(command);
			else if(!strcmp(command[0],"quit"))
				exit(0);
			else
				{	if(!bg)
					{int pid=fork();
						int status;
						if(pid<0)
							perror("ERROR:Forking Child process failed");
						else if(pid==0)
						{
					if(execvp(command[0],command)<0)
						perror("ERROR:exec failed\n");}
						else
							waitpid(pid,&status,0);
					}
					else
					background(command);
				}
		//	else if(!strcmp(command[0],"pinfo"))
		//		pinfo(command);
		}
		free(command);
		free(commands);


		}

	
	}
	
