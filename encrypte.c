#include<stdio.h>
#include<string.h>
#include<error.h>
#include<fcntl.h>
#include<dirent.h>
#define BUF_SIZE 8192

void encryptDecrypt(char *input, char *output) {
 char key[] = {'K', 'C', 'Q'}; //Can be any chars, and any size array
 
 int i;
 for(i = 0; i < strlen(input); i++) {
   output[i] = input[i] ^ key[i % (sizeof(key)/sizeof(char))];
 }
}

int startswith(char * prefix, char * str) {
  if (prefix == NULL || str == NULL) 
	return -1;
  if (strncmp(prefix, str, strlen(prefix))==0)
	return 1;
  return 0;
}


int main(){
  int input_fd;
  int input2_fd;
  int encrypt_fd;
  int decrypt_fd ;
  ssize_t ret_in, ret_out, ret_recover;
  char buffer[BUF_SIZE];
  char buffer2[BUF_SIZE];
	char out_buffer[BUF_SIZE];
	char origin_buffer[BUF_SIZE];
  DIR *dir=NULL;
  struct dirent * enter;
  char * path= "/mnt/";
  if((dir=opendir(path))==NULL){
		printf("opendir failed!");
  }
  else{
		int enc=1;
		int dec=1;
		while(enter=readdir(dir)){
			char * backImageName1=".";
			char * backImageName2="..";
			
			printf("%s\n",enter->d_name);
			if (strcmp(enter->d_name,backImageName1)==0 || strcmp(enter->d_name,backImageName2)==0){
				continue;
			}else{
				
				if (startswith("toCrypt",enter->d_name)){
					char dest[80];
					printf("================= Begin ===========\n");
					printf ("toCrypt\n");
					strcpy(dest,path);
					strcat(dest,enter->d_name);
					input_fd=open(dest,O_RDONLY);
					//input_fd=open("/mnt/toCrypt1.txt",O_RDONLY);
					if (input_fd==-1){
						perror("open");
						printf("could not open original file\n");
					}
					char str[80];
					sprintf(str,"/mnt/crypted%d.txt",enc);
					printf("%s\n",str);
					encrypt_fd = open(str, O_WRONLY | O_CREAT, 0644);
					if(encrypt_fd == -1){perror("open");}
					while((ret_in = read (input_fd, &buffer, BUF_SIZE)) > 0){
						encryptDecrypt(&buffer,&out_buffer);
						ret_out = write (encrypt_fd, &out_buffer, (ssize_t) ret_in);
						if(ret_out != ret_in){
							perror("write");
							return -3;
						}
					}
					dest[0]='\0';
					enc++;
					close (input_fd);
					close (encrypt_fd);
					printf ("============== End ============\n");
				}else{
					if (startswith("toDecrypt",enter->d_name)){
	
					printf("================= Begin ===========\n");
					printf ("toDecrypt\n");
						char dest[80];
						strcpy(dest,path);
						strcat(dest,enter->d_name);
						input2_fd=open(dest,O_RDONLY);
						//input2_fd=open("/mnt/toDecrypt.txt",O_RDONLY);
						char str[80];
						if (input2_fd==-1){perror("open");}
						sprintf(str,"/mnt/decrypted%d.txt",dec);
						printf("%s\n",str);
						decrypt_fd = open(str, O_WRONLY | O_CREAT, 0644);
						if(decrypt_fd == -1){
		    					perror("open");
						}
						while((ret_in = read (input2_fd, &buffer2, BUF_SIZE)) > 0){
							encryptDecrypt(&buffer2,&out_buffer);
							ret_out = write (decrypt_fd, &out_buffer, (ssize_t) ret_in);
							if(ret_out != ret_in){
								perror("write");
								return -3;
							}
						}
						dest[0]='\0';
						close (input2_fd);
						close (decrypt_fd);
						dec++;
					
					printf ("============== End ============\n");
					}
				}
			}
		}	
	}
  closedir(dir);			
  return 0;
}
