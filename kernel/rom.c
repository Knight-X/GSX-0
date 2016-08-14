#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	DIR *dir;

	struct dirent *ptr;
	int i;
	dir = opendir("/home/kazami/workspace");

	while ((ptr = readdir(dir)) != NULL) {
		if (ptr->d_type == DT_DIR) 
			printf("d_name: %s\n", ptr->d_name);
	}

	closedir(dir);
}
