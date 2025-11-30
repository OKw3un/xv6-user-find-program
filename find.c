#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *target)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	
	if ((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	if (fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}
	switch(st.type){
	case T_FILE:
		{
			char *name = path;
			for(int i =strlen(path) - 1; i >= 0; i--){
				if(path[i] == '/'){
					name = &path[i+1];
					break;
				}
			}
			if (strcmp(name, target) == 0){
				printf("%s\n", path);
			}
		}
		break;
	case T_DIR: //if it is a directory, go inside it
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';

		while(read(fd, &de, sizeof(de)) == sizeof(de)){ //read one by one
			if(de.inum == 0)
				continue;
			if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
				continue; //don't recurse into . and ..
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
	
			if(stat(buf, &st) < 0){
				printf("find: cannot stat %s\n", buf);
				continue;
			}
			if(strcmp(de.name, target) == 0){ //check the name of file
				printf("%s\n", buf);
			}
			if(st.type == T_DIR){ //if sub-directory, then recursive
				find(buf, target);
			}
		}
		break;
	}
	close(fd);
}
int
main(int argc, char *argv[])
{
	if(argc < 3){
		fprintf(2, "usage: find <path> <name>\n");
		exit(0);
	}
	//first one is path, second one is file name
	find(argv[1], argv[2]);
	exit(0);
}
