#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define POLYNOM (0xEDB88320)
uint32_t crc32(uint8_t* ptr, uint32_t len){
	uint32_t crc = (uint32_t)(~0);
	int i;

	while(len--) {
		crc ^= *ptr++;
		for (i = 0; i < 8; i++) {
			crc = (crc >>1) ^ ((crc&1) ? POLYNOM : 0);
		}
	}
	return crc^(uint32_t)(~0);
}
int	main(int argc, char **argv)
{
	int			fd;
	int			n;
	char		*nptr;
	char		*ptr;
	char		*nnptr;
	struct stat	st;
	
	n = 0;
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		write(2, "ERROR\n", 6);
		return (0);
	}
	fstat(fd, &st);
	ptr = (char *)mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	memset(ptr,0,st.st_size);
	
	(*(unsigned short*)(ptr + 510)) = 0xaa55;
	(*(unsigned char*)(ptr + 0x1be + 4)) = 0xee;
	(*(unsigned int*)(ptr + 0x1be + 8)) = 1; //2;
	(*(unsigned int*)(ptr + 0x1be + 12)) = 10 * (1024 * 1024) / 512 - 1; //10 * (1024 * 1024) / 512;
	
	nnptr = ptr + 512;
	(*(unsigned long long*)(nnptr + 0x00)) = 0x5452415020494645;
	(*(unsigned int*)(nnptr + 0x08)) = 0x100;
	(*(unsigned int*)(nnptr + 0x0c)) = 0x5c;
	//(*(unsigned int*)(nnptr + 0x10)) = 0x2dfd2dd4; //CRC32 - 0x04c11db7
	(*(unsigned long long*)(nnptr + 0x18)) = 0x1;
	(*(unsigned long long*)(nnptr + 0x20)) = (*(unsigned int*)(ptr + 0x1be + 12)); //18433;
	(*(unsigned long long*)(nnptr + 0x28)) = 0x22;
	(*(unsigned long long*)(nnptr + 0x30)) = (*(unsigned long long*)(nnptr + 0x20)) - 1;//18432;
	(*(unsigned long long*)(nnptr + 0x48)) = 0x2;
	(*(unsigned int*)(nnptr + 0x50)) = 0x9;
	(*(unsigned int*)(nnptr + 0x54)) = 0x80;
	//(*(unsigned int*)(nnptr + 0x58)) = 0x8bca0f08;

	
	nptr = nnptr + 512; //0x0c + 2 * 512;
	while (n < 9)
	{
		(*(unsigned char*)(nptr + 0x00 + n * 0x80)) = 0x0f; //{0x0F, 0xC6, 0x3D, 0xAF, 0x84, 0x83, 0x47, 0x72, 0x8E, 0x79, 0x3D, 0x69, 0xD8, 0x47, 0x7D, 0xE4};
		(*(unsigned char*)(nptr + 0x01 + n * 0x80)) = 0xc6;
		(*(unsigned char*)(nptr + 0x02 + n * 0x80)) = 0x3d;
		(*(unsigned char*)(nptr + 0x03 + n * 0x80)) = 0xaf;
		(*(unsigned char*)(nptr + 0x04 + n * 0x80)) = 0x84;
		(*(unsigned char*)(nptr + 0x05 + n * 0x80)) = 0x83;
		(*(unsigned char*)(nptr + 0x06 + n * 0x80)) = 0x47;
		(*(unsigned char*)(nptr + 0x07 + n * 0x80)) = 0x73;
		(*(unsigned char*)(nptr + 0x08 + n * 0x80)) = 0x8e;
		(*(unsigned char*)(nptr + 0x09 + n * 0x80)) = 0x79;
		(*(unsigned char*)(nptr + 0x0a + n * 0x80)) = 0x3d;
		(*(unsigned char*)(nptr + 0x0b + n * 0x80)) = 0x69;
		(*(unsigned char*)(nptr + 0x0c + n * 0x80)) = 0xd8;
		(*(unsigned char*)(nptr + 0x0d + n * 0x80)) = 0x47;
		(*(unsigned char*)(nptr + 0x0e + n * 0x80)) = 0x7d;
		(*(unsigned char*)(nptr + 0x0f + n * 0x80)) = 0xe4;
		(*(unsigned long long*)(nptr + 0x20 + n * 0x80)) = 4 * 512 / 512 + n * (1024 * 1024) / 512;
		(*(unsigned long long*)(nptr + 0x28 + n * 0x80)) = 4 * 512 / 512 + (n + 1) * (1024 * 1024) / 512;
		(*(unsigned char*)(nptr + 0x56 + n * 0x80)) = 'L';
		(*(unsigned char*)(nptr + 0x57 + n * 0x80)) = 'i';
		(*(unsigned char*)(nptr + 0x58 + n * 0x80)) = 'n';
		(*(unsigned char*)(nptr + 0x59 + n * 0x80)) = 'u';
		(*(unsigned char*)(nptr + 0x5a + n * 0x80)) = 'x';
		(*(unsigned char*)(nptr + 0x5b + n * 0x80)) = ' ';
		(*(unsigned char*)(nptr + 0x5c + n * 0x80)) = 'f';
		(*(unsigned char*)(nptr + 0x5d + n * 0x80)) = 'i';
		(*(unsigned char*)(nptr + 0x5e + n * 0x80)) = 'l';
		(*(unsigned char*)(nptr + 0x5f + n * 0x80)) = 'e';
		(*(unsigned char*)(nptr + 0x60 + n * 0x80)) = 's';
		(*(unsigned char*)(nptr + 0x61 + n * 0x80)) = 'y';
		(*(unsigned char*)(nptr + 0x62 + n * 0x80)) = 's';
		(*(unsigned char*)(nptr + 0x63 + n * 0x80)) = 't';
		(*(unsigned char*)(nptr + 0x64 + n * 0x80)) = 'e';
		(*(unsigned char*)(nptr + 0x65 + n * 0x80)) = 'm';
		(*(unsigned char*)(nptr + 0x66 + n * 0x80)) = ' ';
		(*(unsigned char*)(nptr + 0x67 + n * 0x80)) = 'd';
		(*(unsigned char*)(nptr + 0x68 + n * 0x80)) = 'a';
		(*(unsigned char*)(nptr + 0x69 + n * 0x80)) = 't';
		(*(unsigned char*)(nptr + 0x6a + n * 0x80)) = 'a';
		n++;
	}
	(*(unsigned int*)(nnptr + 0x58)) = crc32(nptr,0x9*0x80);
	(*(unsigned int*)(nnptr + 0x10)) = crc32(nnptr,0x5c);

	//(*(unsigned long long*)(nptr + 0x20 )) = 4;
	//(*(unsigned long long*)(nptr + 0x28 )) = 4 + (1024 * 1024) / 512;

	//printf("%04x\n", (*(unsigned short*)(ptr + 510)));
	munmap(ptr, st.st_size);
	close(fd);
	return (0);
}
