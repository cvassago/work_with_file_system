#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
struct part {
	unsigned char	flag;
	char			st_chs[3];
	unsigned char	tip;
	char			fin_chs[3];
	unsigned int	st_lba;
	unsigned int	size;
};

struct mbr {
	unsigned char	code[446];
	struct part		parts[4];
	unsigned short	sign;
}__attribute__((packed));

int	main(int argc, char **argv)
{
	int		fd;
	char	*nptr,	*ptr, *nnptr;
	struct stat	st;
	struct mbr *mbr;
	int i;
//	printf("%d\n", sizeof(struct mbr));
//	return 0;
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		write(2, "ERROR\n", 6);
		return (0);
	}
	fstat(fd, &st);
	ptr = (char *)mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	memset(ptr,0,st.st_size);
	mbr = ptr;

	mbr->sign = 0xaa55;
	mbr->parts[0].tip =5;
	mbr->parts[0].st_lba = 1;
	mbr->parts[0].size = 2+2048;
	
	mbr = ptr + (mbr->parts[0].st_lba * 512);
	mbr->sign = 0xaa55;
	
	for ( i = 0 ; i < 4 ; i++) {
		mbr->parts[i].tip =6;
		mbr->parts[i].size = 512;
		if ( i == 0 ) {
			mbr->parts[i].st_lba = 1;
		} else {
			mbr->parts[i].st_lba = mbr->parts[i-1].st_lba + mbr->parts[i-1].size;
		}
	}
/*	
	(*(unsigned short*)(ptr + 510)) = 0xaa55;
	(*(unsigned char*)(ptr + 0x1be + 4)) = 6;
	(*(unsigned int*)(ptr + 0x1be + 8)) = 2;
	(*(unsigned int*)(ptr + 0x1be + 12)) = (1024 * 1024) / 512;
	
	(*(unsigned char*)(ptr + 0x1ce + 4)) = 6;
	(*(unsigned int*)(ptr + 0x1ce + 8)) = 2 + (1024 * 1024) / 512;
	(*(unsigned int*)(ptr + 0x1ce + 12)) = (1024 * 1024) / 512;

	(*(unsigned char*)(ptr + 0x1de + 4)) = 6;
	(*(unsigned int*)(ptr + 0x1de + 8)) = 2 + 2 * (1024 * 1024) / 512;
	(*(unsigned int*)(ptr + 0x1de + 12)) = (1024 * 1024) / 512;
	
	(*(unsigned char*)(ptr + 0x1ee + 4)) = 5;
	(*(unsigned int*)(ptr + 0x1ee + 8)) = 2 + 3 * (1024 * 1024) / 512;
	(*(unsigned int*)(ptr + 0x1ee + 12)) = 4 * (1024 * 1024) / 512 + 1;
	
	nptr = ptr + (2 + 3 * (1024 * 1024) / 512) * 512;
	(*(unsigned short*)(nptr + 510)) = 0xaa55;
	(*(unsigned char*)(nptr + 0x1be + 4)) = 6;
	(*(unsigned int*)(nptr + 0x1be + 8)) = 2;
	(*(unsigned int*)(nptr + 0x1be + 12)) = (1024 * 1024) / 512;

	(*(unsigned char*)(nptr + 0x1ce + 4)) = 6;
	(*(unsigned int*)(nptr + 0x1ce + 8)) = 2 + (1024 * 1024) / 512;
	(*(unsigned int*)(nptr + 0x1ce + 12)) = (1024 * 1024) / 512;

	(*(unsigned char*)(nptr + 0x1de + 4)) = 6;
	(*(unsigned int*)(nptr + 0x1de + 8)) = 2 + 2 * (1024 * 1024) / 512;
	(*(unsigned int*)(nptr + 0x1de + 12)) = (1024 * 1024) / 512;
	
	(*(unsigned char*)(nptr + 0x1ee + 4)) = 5;
	(*(unsigned int*)(nptr + 0x1ee + 8)) = 2 + 3 * (1024 * 1024) / 512;
	(*(unsigned int*)(nptr + 0x1ee + 12)) = 4 * (1024 * 1024) / 512 + 1;

	nnptr = nptr + (2 + 3 * (1024 * 1024) / 512) * 512;
	(*(unsigned short*)(nnptr + 510)) = 0xaa55;
	(*(unsigned char*)(nnptr + 0x1be + 4)) = 6;
	(*(unsigned int*)(nnptr + 0x1be + 8)) = 2;
	(*(unsigned int*)(nnptr + 0x1be + 12)) = (1024 * 1024) / 512;

	(*(unsigned char*)(nnptr + 0x1ce + 4)) = 6;
	(*(unsigned int*)(nnptr + 0x1ce + 8)) = 2 + (1024 * 1024) / 512;
	(*(unsigned int*)(nnptr + 0x1ce + 12)) = (1024 * 1024) / 512;

	(*(unsigned char*)(nnptr + 0x1de + 4)) = 6;
	(*(unsigned int*)(nnptr + 0x1de + 8)) = 2 + 2 * (1024 * 1024) / 512;
	(*(unsigned int*)(nnptr + 0x1de + 12)) = (1024 * 1024) / 512;
*/
	//printf("%04x\n", (*(unsigned short*)(ptr + 510)));
	munmap(ptr, st.st_size);
	close(fd);
	return (0);
}
