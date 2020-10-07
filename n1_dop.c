#include "dop.h"

#define POLYNOM (0xEDB88320)
uint32_t crc32(uint8_t* ptr, uint32_t len)
{
	uint32_t crc = (uint32_t)(~0);
	int i;

	while(len--)
	{
		crc ^= *ptr++;
		for (i = 0; i < 8; i++) 
		{
			crc = (crc >>1) ^ ((crc&1) ? POLYNOM : 0);
		}
	}
	return crc^(uint32_t)(~0);
}

int	main(int argc, char **argv)
{
	int				fd;
	int				n;
	int				k;
	char			*ptr;
	struct stat		st;
	struct stat		g_st;
	struct mbr		*mbr;
	struct gpt		*gpt, *gpt2;
	struct r_gpt	*r_gpt;
	int				num;
    int				meta_size;
	int				min_size;
	int				sum;
	int				f_fd;
	char			buf;
	unsigned int	*sizes_f;

	sum = 0;
	if (argc < 3)
	{
		write(1, "Use: ./a.out <floppy> and file/files with informations\n", 55);
		return __LINE__;
	}

	sizes_f = (unsigned int*)malloc((argc - 2) * sizeof(unsigned int));
/*	for (n = 0; n > argc - 2; n++) */
	for (n = 0; n < argc - 2; n++)
	{
		stat(argv[n + 2], &st);
		if (st.st_size % 512 != 0)
			sum = sum + st.st_size + 512 - st.st_size % 512;
		else
			sum = sum + st.st_size;
		sizes_f[n] = st.st_size;
	}

	num = argc - 2;
	/* 3 = mbr + gpt + gpt */
	meta_size = 3 + ((num * sizeof(struct r_gpt)) + 511)/512;
	min_size = (meta_size /*+ num*/) * 512;
	
	fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fd < 0)
	{
		perror("open");
		fprintf(stderr,"Error\n");
		write(2, "ERROR2\n", 7);
		return (0);
	}
	fstat(fd, &g_st);
	g_st.st_size = /*sizeof(struct part) + sizeof(struct mbr) + 2 * sizeof(struct gpt) +*/ min_size + sum;

	lseek(fd, g_st.st_size - 1, SEEK_SET);
	write(fd, &fd, 1);
	lseek(fd, 0, SEEK_SET);

	ptr = (char *)mmap(NULL, g_st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memset(ptr, 0, g_st.st_size);

	mbr = (struct mbr*)ptr;

	mbr->sign = 0xaa55;
	mbr->parts[0].lba = 1;
	mbr->parts[0].size = (g_st.st_size / 512) - 1;
	mbr->parts[0].type = 0xee;
	
	gpt = (struct gpt*)(ptr + 512);

	gpt->sign = 0x5452415020494645;
	gpt->vers = 0x100;
	gpt->size_h = sizeof(struct gpt);
	gpt->adress_now = 0x1;
	gpt->adress_other = mbr->parts[0].size;
	gpt->adress_start = 2 + (num * sizeof(struct r_gpt) + 511) / 512;
	gpt->guid[0] = 1;
	gpt->adress_stop = mbr->parts[0].size - 1;
	gpt->adress_start_tab = 2;
	gpt->count = num;
	gpt->size_r = sizeof(struct r_gpt);
	
	r_gpt = (struct r_gpt*) (ptr + gpt->adress_start_tab * 512);
	k = 2;
	for ( n = 0; n < num; n++)
	{
		r_gpt[n].c_guid[0] = 1;
		f_fd = open(argv[k], O_RDONLY);
		if (f_fd < 0)
		{
			write(2, "ERROR\n", 6);
			return 0;
		}
		if ( n == 0 )
		{
			r_gpt[n].start = gpt->adress_start;
			lseek(fd, r_gpt[n].start * 512, SEEK_SET);
		}
		else 
		{
			r_gpt[n].start = r_gpt[n-1].stop + 1;
			lseek(fd, r_gpt[n].start * 512, SEEK_SET);
		}
		r_gpt[n].stop = r_gpt[n].start + sizes_f[n] / 512;
		while (read(f_fd, &buf, sizeof(buf)))
			write(fd, &buf, 1);
		close(f_fd);
		k++;
	}
	gpt->sum_2 = crc32((uint8_t*)(r_gpt), gpt->size_r * gpt->count);
	
	/*k = 0;
	for (n = 2; n < argc; n++)
	{
		f_fd = open(argv[n], O_RDONLY);
		//if (n == 2)
		//	lseek(fd, gpt->adress_start, 1);
		//else
		//	lseek(fd, r_gpt[k].start, SEEK_SET);
		if (f_fd < 0)
		{
			write(2, "ERROR\n", 6);
			return (0);
		}
		lseek(fd, r_gpt[k].start, SEEK_SET);

		while (read(f_fd, &buf, sizeof(buf)))
		{
		//	lseek(fd, r_gpt[k].start, SEEK_SET);
			write(fd, &buf, 1);
		}
		close(f_fd);
		k++;
	}*/

	gpt2 = (struct gpt*)(ptr + gpt->adress_other * 512);
	memcpy(gpt2, gpt, sizeof(struct gpt));
	gpt2->adress_now = gpt->adress_other;
	gpt2->adress_other = gpt->adress_now;

	gpt->sum = crc32((uint8_t*)gpt, gpt->size_h);
	gpt2->sum = crc32((uint8_t*)gpt2, gpt2->size_h);
	
	munmap(ptr, st.st_size);
	close(fd);
	return (0);
}
