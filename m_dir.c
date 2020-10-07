#include "m_dir.h"


int f_dir(char *name)
{
	int				n;
	int				fd;
	struct dirent	**namelist;
	char			dir_name[PATH_MAX + 1];
	char			old_dir[PATH_MAX + 1];
	uint64_t		imito;
	char			buf[512];
	int				ret;
	struct stat		st;
	getcwd(old_dir, PATH_MAX);
	chdir(name);
	n = scandir(".", &namelist, NULL, alphasort);
	if (n < 0)
	{
		fprintf(stderr,"Could not scan dir %s: %s\n",name, strerror(errno));
		return (-1);
	}
	while (n--)
	{
		crc = crc32_init();
		if (lstat(namelist[n]->d_name, &st))
		{
			fprintf(stderr,"Could get stat %s: %s\n",name, strerror(errno));
			continue;
		}
		if ((st.st_mode & S_IFLNK) == S_IFLNK) continue;
		if (strcmp(namelist[n]->d_name,".") == 0) continue;
		if (strcmp(namelist[n]->d_name,"..") == 0) continue;

		if ((st.st_mode & S_IFDIR) == S_IFDIR)
		{
			f_dir(namelist[n]->d_name);
			continue;
		}
		fd = open(namelist[n]->d_name, O_RDONLY);
		if ( fd < 0 )
		{
			printf("This file %s don't open", namelist[n]->d_name);
			continue;
		}
		while ((ret = read(fd, buf, 512)) > 0)
			crc = crc32_update(crc, buf, ret);
		close(fd);
		crc = crc32_fin(crc);
		getcwd(dir_name, PATH_MAX);
		printf("%X %s/%s\n", imito, dir_name, namelist[n]->d_name);
		free(namelist[n]);
	}
	free(namelist);
	chdir(old_dir);
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "ERROR\n", 6);
		return (0);
	}
	if (f_dir(argv[1]) < 0)
		return (0);
	return (0);
}
