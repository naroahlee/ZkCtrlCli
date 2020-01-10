#include <stdio.h>
#include <stdlib.h>
#include "tracing.h"

int main(int argc, char* argv[])
{
	FILE* fpin;
	FILE* fpout;
	size_t s32fsize;
	int s32reccnt;
	int s32i;
	int s32ret;
	double d64lat;

	event_record* pstrevt;
	fpin = fopen("all_sort.tracing", "rb");

	/* Get Record Number */
	fseek(fpin, 0, SEEK_END); // seek to end of file
	s32fsize = ftell(fpin); // get current file pointer
	fseek(fpin, 0, SEEK_SET); // seek back to beginning of file
	s32reccnt = s32fsize / sizeof(event_record);
	printf("Record #[%d]\n", s32reccnt);

	/* Read Record */
	pstrevt = (event_record*)malloc(s32reccnt * sizeof(event_record));
	s32ret = fread(pstrevt, sizeof(event_record), s32reccnt, fpin);
	s32ret = s32ret;
	fclose(fpin);

	fpout = fopen("res.csv", "w");
	for(s32i = 0; s32i < s32reccnt; s32i += 2)
	{
		d64lat = (double)(pstrevt[s32i + 1].u64tsc - pstrevt[s32i].u64tsc) / KHZ;
		fprintf(fpout, "%.2lf\n", d64lat);
		
	}
	fclose(fpout);
	free(pstrevt);
	return 0;

}
