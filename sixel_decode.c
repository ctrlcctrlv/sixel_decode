#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <errno.h>

#include <sixel.h>

int main(int argc, char** argv) {
	char* filename = NULL;
	char* outname = NULL;
	bool quiet = false;
	
	int opt;
	while ((opt = getopt(argc, argv, ":i:o:q")) != -1) {
		switch (opt) {
			case 'i':
				filename = optarg;
				continue;
			case 'o':
				outname = optarg;
				continue;
			case 'q':
				quiet = true;
				continue;
			default:
				continue;
		}	
	}

	if (filename == NULL) {
		fprintf(stderr, "Usage: sixel_decode -i input_file [-o output_file] [-q]\n"
						"\n"
						"Output file will be RGB data with no header. If output specified,\n"
						"you'll get width and height on stdout separated by 'x', like 32x32.\n");
		return EX_USAGE;
	}

	FILE *fh;
	int r;

	fh = fopen(filename,"rb");
	if (fh == NULL) {
		fprintf(stderr, "Unable to open %s\n", filename);
		return EX_NOINPUT;
	}

	fseek(fh, 0, SEEK_END);
	int length = ftell(fh);
	fseek(fh, 0, SEEK_SET);

	unsigned char* buffer = malloc(length);
	
	unsigned char* pixels = NULL;
	int width;
	int height;
	unsigned char* palette = NULL;
	int ncolors;
	r = fread( buffer, 1, length, fh );

	fclose(fh);

	sixel_decode_raw(buffer, length, &pixels, &width, &height, &palette, &ncolors, NULL);

	if (quiet)
		goto output;

	fprintf(stderr, "%d bytes read from %s\n\n", r, filename);

	fprintf(stderr, "      Found %d x %d image,\n"
			"	having %d colors.\n\n", width, height, ncolors);

	for (int i = 0; i < ncolors; i++) {
		int r = palette[i*3+0];
		int g = palette[i*3+1];
		int b = palette[i*3+2];
		fprintf(stderr, "\033[48;2;%d;%d;%dm", r, g, b);
		//fprintf(stderr, "Found a color: %d %d %d\n", r, g, b);
		fprintf(stderr, "　");
		if (i % 16 == 15) fprintf(stderr, "\n");
	}
	fprintf(stderr, "\033[0m");

output:

	if (outname == NULL) {
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr, "\n");
	}

	if (access(outname, W_OK) != 0 && errno != ENOENT) {
		fprintf(stderr, "Error with file: %s\n", strerror(errno));
		return EX_OSERR;
	}
	struct stat s;
	int stat_error = stat(outname, &s);
	if (!stat_error) {
		if (S_ISREG(s.st_mode)) {
			fprintf(stderr, "Cowardly refusing to overwrite file\n");
			return EX_TEMPFAIL;
		}
	}

	fh = fopen(outname, "wb+");
	fseek(fh, 0, SEEK_SET);
	for (int p = 0; p < height*width; p++) {
		int pix = pixels[p];
		int r = palette[pix*3+0];
		int g = palette[pix*3+1];
		int b = palette[pix*3+2];
		fwrite(&r, 1, 1, fh);
		fwrite(&g, 1, 1, fh);
		fwrite(&b, 1, 1, fh);
	}
	if (!quiet)
		fprintf(stderr, "Wrote %d pixels\n", height*width);
	fclose(fh);

	return EXIT_SUCCESS;
}
