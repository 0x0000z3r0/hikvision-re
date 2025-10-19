// cc -Wall -Wextra -fsanitize=leak,address,undefined rm-obb.c -o rm-obb
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
        if (argc != 3) {
                fprintf(stderr, "[e] %s <input_file> <output_file>\n", argv[0]);
                return 1;
        }

        const char *input_file = argv[1];
        const char *output_file = argv[2];

        FILE *fin = fopen(input_file, "rb");
        if (!fin) {
                perror("[e] failed to open input file");
                return 1;
        }

        FILE *fout = fopen(output_file, "wb");
        if (!fout) {
                perror("[e] failed to open output file");
                fclose(fin);
                return 1;
        }

        const size_t block_size = 0x800;
        const size_t skip_size  = 0x40;

        unsigned char *buffer = malloc(block_size);
        if (!buffer) {
                perror("[e] memory allocation failed");
                fclose(fin);
                fclose(fout);
                return 1;
        }

        size_t cursor = 0x0;
        while (1) {
                size_t read_bytes = fread(buffer, 1, block_size, fin);
                if (read_bytes > 0) {
                        fwrite(buffer, 1, read_bytes, fout);
                }

                if (fseek(fin, skip_size, SEEK_CUR) != 0) {
                        printf("[w] failed to seek, cursor: 0x%08lx\n", cursor);
                        break;
                }

                if (read_bytes < block_size) {
                        printf("[i] read less than desired, cursor: 0x%08lx\n", cursor);
                        break;
                }

                cursor += read_bytes;
        }

        free(buffer);
        fclose(fin);
        fclose(fout);

        printf("[i] finished removing OBB data.\n");
        return 0;
}

