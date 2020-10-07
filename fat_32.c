#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct common
{
	uint8_t		info[3];
	uint64_t	oem;
	uint16_t	num_byte;
	uint8_t		num_sect;
	uint16_t	num_no_sect;
	uint8_t		num_fat;
	uint16_t	count_or_null;
	uint16_t	common_sect;
	uint8_t		type;
	uint16_t	copy_or_null;
	uint16_t	num_in_track;
	uint16_t	num_golova;
	uint32_t	num_hid;
	uint32_t	com_num;
}__attribute__((packed));

struct fat_12_16
{
	uint8_t		num_disk;
	uint8_t		winda;
	uint8_t		priznak;
	uint32_t	num_log_disk;
	uint8_t		metka[11];
	uint64_t	text;
}__attribute__((packed));

struct fat_32
{
	uint32_t	count;
	uint16_t	num_act_fat;
	uint16_t	num_fat;
	uint32_t	num_first;
	uint16_t	fsinfo;
	uint16_t	num_copy;
	uint32_t	not[3];
	uint8_t		num_disk;
	uint8_t		no;
	uint8_t		ext_rec;
	uint32_t	num_log_disk;
	uint8_t		metka[11];
	uint64_t	text;
}__attribute__((packed));

struct fsinfo
{
	uint32_t	sign;
	uint8_t		no[480];
	uint32_t	sign2;
	uint32_t	free_class;
	uint32_t	class_for_begin;
	uint32_t	not[3];
	uint32_t	sign_end;
}__attribute__((packed));

struct katalog
{
	uint8_t name[11];
	uint8_t atribut;
	uint8_t no_32;
	uint8_t time_mm_32;
	uint16_t time_32;
	uint16_t data_32;
	uint16_t data_last_32;
	uint16_t old_word_32;
	uint16_t time_last;
	uint16_t data_last;
	uint16_t young_word;
	uint32_t size;
}__attribute__((packed));

int	main(int argc, char **argv)
{
	int					fd;
	char				*ptr;
	struct common		common;
	struct fat_12_16	fat_12;
	struct fat_32		fat_32;
	struct fsinfo		fs;
	struct katalog		katalog;
	int					i;

	if (argc < 2)
	{
		write(2, "ERROR 1\n", 8);
		return (0);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		write(2, "ERROR 2\n", 8);
		return (0);
	}
	read(fd, &common, sizeof(struct common));
	if ((common.count_or_null == 0 && common.copy_or_null != 0) || (common.count_or_null != 0 && common.copy_or_null == 0))
	{
		write(2, "ERROR 3\n", 8);
		return (0);
	}
	if (argc == 2)
	{
		printf("----------------------------Загрузочный Сектор--------------------------\n");
		printf("Инструкция перехода: ");
		for(i = 0; i < 3; ++i)
			printf("%u", common.info[i]);
		printf("\n");
		printf("OEM: %llu\n", common.oem);
		printf("Число байт в секторе: %u\n", common.num_byte);
		printf("Число секторов в класстере: %u\n", common.num_sect);
		printf("Число резервных секторов: %u\n", common.num_no_sect);
		printf("Число таблиц FAT: %u\n", common.num_fat);
		if (common.count_or_null == 0)
			printf("Файловая система: FAT32\n");
		else
			printf("Файловая система: FAT12/16\nКоличество файлов в корневом каталоге: %u\n", common.count_or_null);
		if (common.common_sect != 0)
			printf("Общее число секторов в разделе: %u\n", common.common_sect);
		else
			printf("Общее число секторов в разделе: %u\n", common.com_num);
		if (common.type == 0xF0)
			printf("Тип носителя: Гибкий диск, 2 стороны, 18 секторов на дорожке\n");
		else if (common.type == 0xF8)
			printf("Тип носителя: Жесткий диск\n");
		if (!(common.count_or_null == 0))
			printf("Количество секторов, занимаемое одной копией FAT: %u\n", common.copy_or_null);
		printf("Число секторов на дорожке: %u\n", common.num_in_track);
		printf("Число головок: %u\n", common.num_golova);
		printf("Число скрытых секторов: %u\n", common.num_hid);
	
		if (common.count_or_null == 0)
		{	
			read(fd, &fat_32, sizeof(fat_32));
			printf("Количество секторов, занимаемый одной копией FAT: %u\n", fat_32.count);
			printf("Номер активной FAT: %u\n", fat_32.num_act_fat);
			printf("Номер версии FAT32: %u\n", fat_32.num_fat);
			printf("Номер первого класстера корневого каталога: %u\n", fat_32.num_first);
			printf("FSINFO: %u\n", fat_32.fsinfo);
			printf("Номер сектора структуры в резервной ...: %u\n", fat_32.num_copy);
			printf("Номер диска для прерывания 13h: %u\n", fat_32.num_disk);
			printf("Признак расширенной загрузочной записи: %u\n", fat_32.ext_rec);
			printf("Номер логического диска: %u\n", fat_32.num_log_disk);
			printf("Метка диска: ");
			for (i = 0; i < 11; i++)
				printf("%c", fat_32.metka[i]);
			printf("\n");
			
			printf("Текст: %s\n", (char*)&fat_32.text);
			read(fd, &fs, sizeof(fs));
			if (fs.sign != 0x41625252)
			{
				write(2, "ERROR\n", 6);
				return (0);
			}
			printf("Сигнатура: %x\n", fs.sign);
			if (fs.sign2 != 0x61417272)
			{
				write(2, "ERROR\n", 6);
				return (0);
			}
			printf("Сигнатура: %x\n", fs.sign2);
			if (fs.free_class == -1)
				printf("Число свободных класстеров: Вычисли сам:)\n");
			else
				printf("Число свободных класстеров: %u\n", fs.free_class);
			if (fs.class_for_begin == -1)
				printf("Советую начать искать свободный класстер со второго\n");
			else
				printf("Номер класстера для начала поиска свободных класстеров: %u\n", fs.class_for_begin);
			if (fs.sign_end != 0xAA550000)
			{
				write(2, "ERROR\n", 6);
				return (0);
			}
			printf("Сигнатура: %X\n", fs.sign_end);
			lseek(fd, );
		}
		else
		{
			read(fd, &fat_12, sizeof(fat_12));
			printf("Номер диска для прерывания 13h: %u\n", fat_12.num_disk);
			printf("Признак расширенной загрузочной записи: %u\n", fat_12.winda);
			printf("Номер логического диска: %u\n", fat_12.num_log_disk);
			printf("Метка диска: ");
			for (i = 0; i < 11; i++)
			    printf("%c", fat_12.metka[i]);
			printf("\n");
			fat_12.text &= 0xffffffffffff;
			printf("Текст: %s\n", (char*)&fat_12.text);
			lseek(fd, (common.num_no_sect + common.num_fat * common.copy_or_null) * 512, SEEK_SET);
			printf("\n----------------------------Корневой Каталог----------------------------\n");
			for(i = 0; i < common.count_or_null / 10; i++)
			{
				read(fd, &katalog, sizeof(struct katalog));
				printf("************%s***********\n", katalog.name);
				printf("Атрибуты: %X\n", katalog.atribut);
				printf("Время последнее: %u\n", katalog.time_last);
				printf("Дата последняя: %u\n", katalog.data_last);
				printf("Младшее слово: %u\n", katalog.young_word);
				printf("Размер: %u\n", katalog.size);
			}
		}
		//printf("\n----------------------------Корневой Каталог----------------------------\n");
		//lseek(fd, common.num_no_sect + common.num_fat * common.copy_or_null, SEEK_SET);
		//printf("\n%d\n", common.num_no_sect + common.num_fat * common.copy_or_null);
	}
	return (0);
}
