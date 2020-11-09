#include <stdio.h>
#include <string.h>
#include <pthread.h>

char filelist[20][50] = {}; //original file metadata array
char g[2][10][20] = {}; //name of split files metadata array
int h[2][10][1] = {}; //size of split files metadata array
char re_filelist[20][50] = {}; //combined file metadata array

int file_division(FILE *f, char fname[20], int n, int length) {
	char s[2];
	FILE *fp[n];

	//opening split files by write mode
	for(int i = 0; i < n; i++) {
                char filename[20] = "para_";
		strcat(filename, strtok(fname, "."));
		sprintf(s, "%d", i);
                strcat(filename, s);
                strcat(filename, ".bin");
                fp[i] = fopen(filename, "wb");
                if(fp[i] == NULL) {
                        fclose(f);
                        return -1;
                }
        }
	
	// writing from original file to split files
        unsigned char data = 0;
	int temp_length = length / n;
        for(int i = 0; i < length; i++) {
                int index = (i / temp_length > n - 1) ? n - 1 : i / temp_length;
                fread(&data, 1, 1, f);
                fwrite(&data, 1, 1, fp[index]);
        }
	fclose(f);
        for(int i = 0; i < n; i++) fclose(fp[i]);
	return 0;
}

int file_combination(char fname[20], int n, int length, int count) {
	unsigned char data = 0;
	int temp_length = length / n;
	char s[2];

	//opening restore file
	FILE *fp[n];
	char restore[20] = "re_";
	strcat(restore, fname);
	FILE *fr = fopen(restore, "wb");
	if(fr == NULL) {
		fclose(fr);
		return -1;
	}

        for(int i = 0; i < n; i++) {
		//opening all split files by read mode
                char filename[20] = "para_";
                strcat(filename, strtok(fname, "g"));
                sprintf(s, "%d", i);
                strcat(filename, s);
                strcat(filename, ".bin");
                fp[i] = fopen(filename, "rb");
		if(fp[i] == NULL) {
			printf("file open failed\n");
			return -1;
		}
		
		//file length
                fseek(fp[i], 0, SEEK_END);
                int para_length = ftell(fp[i]);
                fseek(fp[i], 0, SEEK_SET);
		
		//writing from split files to restore file
                for(int j = 0; j < para_length; j++) {
                        fread(&data, 1, 1, fp[i]);
                        fwrite(&data, 1, 1, fr);
                }
		
		//producing file metadata of split files
		strcpy(g[count][i], filename);
		h[count][i][0] = para_length;
        }
        fclose(fr);
	strcpy(re_filelist[count], restore);
	return 0;
}


void file_list(char fname[50], int count)
{
	FILE *flist = fopen(fname + 1, "rb"); //open combined file
	FILE *re_flist = fopen(re_filelist[count-1], "rb");

	//file size
	fseek(flist, 0, SEEK_END);
        int len = ftell(flist);
        fseek(flist, 0, SEEK_SET);
	fclose(flist);

	fseek(re_flist, 0, SEEK_END);
        int re_len = ftell(re_flist);
        fseek(re_flist, 0, SEEK_SET);
        fclose(re_flist);

	//combined file
	printf("%d %23s %10.1fKB %8c\n", count, fname + 1, len / 1000.0, fname[0]);
	
	//split files
	for(int i = 0; i < fname[0] - 48; i++)
		printf("%25s %10.1fKB\n", g[count - 1][i], h[count - 1][i][0] / 1000.0);

	//combined file
        printf("%d %23s %10.1fKB\n", count, re_filelist[count-1], re_len / 1000.0);

}

int main() {
	int i, n1 = 6, n2 = 8;; // number of split files
	int file_count = 0, count = 0;
	pthread_t thread[n1];

	//opening 2 image files by binary read mode
	char fname1[20] = "xps.jpg";
	char fname2[20] = "scenery.jpg";
	strcpy(filelist[0] + 1, fname1);
	strcpy(filelist[1] + 1, fname2);
	filelist[0][0] = n1 + 48;
	filelist[1][0] = n2 + 48;
	FILE *f1 = fopen(fname1, "rb");
	FILE *f2 = fopen(fname2, "rb");
	if(f1 == NULL || f2 == NULL) {
		printf("file open failed\n");
		return 0;
	}
	
	//file size
	fseek(f1, 0, SEEK_END);
	int length1 = ftell(f1);
	fseek(f1, 0, SEEK_SET);
	fseek(f2, 0, SEEK_END);
	int length2 = ftell(f2);
	fseek(f2, 0, SEEK_SET);

	// file division of f1 and f2
	if(file_division(f1, fname1, n1, length1) == -1) return 0;
	if(file_division(f2, fname2, n2, length2) == -1) return 0;
	
	// file combination of f1 and f2
	if(file_combination(fname1, n1, length1, count++) == -1) return 0;
	if(file_combination(fname2, n2, length2, count++) == -1) return 0;
	
	// printing out file metadata
	printf("No.           File name       File size   # of blocks\n");
	file_list(filelist[0], ++file_count); //f1 <- filelist[0]
	file_list(filelist[1], ++file_count); //f2 <- filelist[1]

	return 0;
}
