#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

#include "mbuf.h"
#include "athn.h"

void *
softc_init(int ptrsize)
{
	void *softc = malloc(ptrsize);
	bzero(softc, ptrsize);
	return softc;
}

void
usb_rx_callback(struct athn_usb_softc *usc, char *buf, int actlen)
{
	struct athn_usb_rx_data data;
	data.sc = usc;
	data.buf = buf;
	athn_usb_rxeof(&data, actlen);
	printf("AR_USB_RX_STREAM_TAG: 0x%04hX\n", htole16(AR_USB_RX_STREAM_TAG));
}

unsigned char *
read_file(const char *path, size_t *size_out)
{
	FILE *fp;
	unsigned char *buf;
	long sz;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		perror(path);
		return NULL;
	}

	if (fseek(fp, 0, SEEK_END) != 0) {
		perror("fseek");
		fclose(fp);
		return NULL;
	}

	sz = ftell(fp);
	if (sz < 0) {
		perror("ftell");
		fclose(fp);
		return NULL;
	}

	rewind(fp);

	buf = malloc((size_t)sz);
	if (buf == NULL) {
		perror("malloc");
		fclose(fp);
		return NULL;
	}

	if (fread(buf, 1, (size_t)sz, fp) != (size_t)sz) {
		perror("fread");
		free(buf);
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	*size_out = (size_t)sz;
	return buf;
}


int
main(int argc, char *argv[])
{
	unsigned char *buf;
	long size;
	size_t nread;
	void *softc;
	DIR *dir;
	struct dirent *ent;
	struct stat st;
	char path[PATH_MAX];

	if (argc != 2) {
		fprintf(stderr, "usage: %s <directory>\n", argv[0]);
		return 1;
	}
	dir = opendir(argv[1]);
	if (dir == NULL) {
		perror("opendir");
		return 1;
	}

	while ((ent = readdir(dir)) != NULL) {
		/* skip . and .. */
		if (strcmp(ent->d_name, ".") == 0 ||
		    strcmp(ent->d_name, "..") == 0)
			continue;

		snprintf(path, sizeof(path), "%s/%s",
		    argv[1], ent->d_name);

		if (stat(path, &st) != 0)
			continue;

		/* process regular files only */
		if (!S_ISREG(st.st_mode))
			continue;

		printf("processing %s\n", path);

		buf = read_file(path, &nread);
		if (buf == NULL)
			continue;

		usb_rx_callback(softc, buf, nread);
		free(buf);
	}
#if 0
	if (argc < 2) {
		fprintf(stderr, "usage: %s file.bin\n", argv[0]);
		return 1;
	}

	softc = softc_init(sizeof(struct athn_usb_softc));

	for(q=1;q<argc;q++) {
		printf("processing %s\n", argv[q]);
		buf = read_file(argv[q], &nread);
		usb_rx_callback(softc, buf, nread);
		free(buf);
	}
#endif
}
