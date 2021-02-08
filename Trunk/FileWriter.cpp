#include "StdAfx.h"
#include "FontLib.h"
#include "ShapeExtract.h"

void WriteCharsToFile(std::list<FontExtracted*>* FontShapes, const char* FileName)
{
	FILE* f;
	errno_t er = fopen_s(&f, FileName, "wt");
	if (f == NULL)
	{
		printf("Could not open output file for writing\n");
		return;
	}
	//write version
	fprintf(f, "ver:%s\n", SHP_FILE_VERSION_STRING);
	fprintf(f, "FileType:CharactersOnly\n");

	for (auto itr = FontShapes->begin(); itr != FontShapes->end(); itr++)
	{
		FontExtracted* fe = *itr;
		if (fe->AssignedString == NULL)
			continue;
		fprintf(f, "CHAR:%d,%d,%d,%d,%s\n", fe->x, fe->y, fe->Width, fe->Height, fe->AssignedString);
	}
}

void WriteTextToFileMerged(std::list<FontExtracted*>* FontShapes, const char* FileName)
{
	FILE* f;
	errno_t er = fopen_s(&f, FileName, "wt");
	if (f == NULL)
	{
		printf("Could not open output file for writing\n");
		return;
	}
	//write version
	fprintf(f, "ver:%s\n", SHP_FILE_VERSION_STRING);
	fprintf(f, "FileType:CharactersMerged\n");

	int MaxRows = 2000;
	int MaxWidth = 2000;
	int MaxFileSize = MaxRows * MaxWidth;
	char* fcontent = (char*)malloc(MaxFileSize);
	fcontent[0] = 0;
	int RowsWritten = 0;
	for (auto itr = FontShapes->begin(); itr != FontShapes->end(); itr++)
	{
		int RowBytes = 0;
		FontExtracted* fe = *itr;
		if (fe->AssignedString == NULL)
			continue;
		if (fe->MergedPrev != NULL)
			continue;
		//if we already used this shape
		float Angle = 0;
		if (fe->fss && fe->fss->fi)
			Angle = fe->fss->fi->Angle;
		if (fe->MergedNext == NULL && fe->MergedPrev == NULL)
		{
			RowBytes += sprintf_s(&fcontent[RowsWritten* MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "CHAR:%d,%d,%d,%d,%f,%s\n", fe->x, fe->y, fe->Width, fe->Height, Angle, fe->AssignedString);
			continue;
		}
		//print sequence of chars
		RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "TEXT:%d,%d,%d,%d,%f,%s", fe->x, fe->y, fe->Width, fe->Height, Angle, fe->AssignedString);
		fe = fe->MergedNext;
		while (fe != NULL)
		{
			RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "%s", fe->AssignedString);
			fe = fe->MergedNext;
		}
		RowBytes += sprintf_s(&fcontent[RowsWritten * MaxWidth + RowBytes], MaxFileSize - (RowsWritten * MaxWidth + RowBytes), "\n");
		RowsWritten++;
	}

	//flip order of text because image was upside down
	for (int i = RowsWritten - 1; i >= 0; i--)
		fprintf(f, "%s", &fcontent[i * MaxWidth]);

	fclose(f);
}