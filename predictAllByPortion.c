/*!
 * Copyright 2019 XGBoost contributors
 *
 * \file c-api-demo.c
 * \brief A simple example of using xgboost C API.
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/stat.h> 
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>

#include <time.h>
#include "../xgboost/include/xgboost/c_api.h"fr

#define safe_xgboost(call) {                                            \
int err = (call);                                                       \
if (err != 0) {                                                         \
  fprintf(stderr, "%s:%d: error in %s: %s\n", __FILE__, __LINE__, #call, XGBGetLastError()); \
  exit(1);                                                              \
}                                                                       \
}
typedef struct TStCol {
	int size;
	char** StrCol1;
	char** StrCol2;
} TStCol;
////////////////////////////////////////////////////////////
typedef struct StrColIntMtrx {
	size_t size_h;
	size_t size_l;
	int** IntMtrx;
	char** StrCol;
} StrColIntMtrx;
//////
////////////////////////////////////////////////////////////
typedef struct ArraySet {
	size_t raws;
	size_t cols;
	//int** IntMtrx;
	//int* Labels;
	float** IntMtrx;
	float* Labels;
	
} ArraySet;
//////
/////////////////////////////////////////////////////////////////////////////
int cmpfunc(const void * pa, const void * pb) 
{
const char** a = *(const char ***)pa;
const char** b = *(const char ***)pb;
return strcmp(*a,*b);
}
////////////////////////////////////////////////////////////////////////
int cmpfunc2(const void* key, const void* b)
{
    const char* bb = *((const char**)b);
    return strcmp((const char*)key, bb);
}
////////////////////////////////////////////////////////////
//a_str - string to split, a_delim - delimiter, 
//Returns size - returns parts amount, returns array of tokens
char** str_split(char* a_str, const char a_delim, int *size)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);
	*size = count;

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
////////////////////////////////////////////////////////////////////////////////////
//fp - file to read, sp1Stolbec - column of key label strings, sp1Data - array[][] of int
//columns, row - max possible cols and raws
int countTableElemFromFile(const char* Name, int *columns, size_t *rows)
{
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) 
	{
		printf("countTableElemFromFile %s failed to open \n",Name);
		//exit(EXIT_FAILURE);
	}
	int result=1;
	size_t amount_str=0;
	char* line = NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	while ((getline(&line, &len, fp)) != -1) 
	{

		//printf("%s", line); 	//string to parts
		char** tokens;

		//printf("line=[%s]\n\n", line); 	//length of the string
		amount_str++;
		tokens = str_split(line, '\t', &len_str);
		//remeber max line length
		if (len_str > len_str_max) len_str_max=len_str;
		//printf("Size of String %d/n",len_str); 	//printf("Amount of String %d/n",amount_str);

		//free memory
		if (tokens)
		{
			int i;
			for (i = 0; *(tokens + i); i++)
			{
				free(*(tokens + i));
			}
			free(tokens);
		}
		//if (line) free(line);
		//end of string to parts
	}
		
	*columns=len_str_max;
	*rows=amount_str;
	fclose(fp);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
//fp - file to read, sp1Stolbec - column of key label strings, sp1Data - array[][] of int
//columns, row - Real cols and raws. Raws may be smaller 1,2.
int ReadKeyColAndIntTableFromFile(const char* Name, char** sp1Stopbec, int ** sp1Data,int *columns, int *rows)
{
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	int result=1;
	int amount_str=0;
	char* line = NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	if (!sp1Stopbec || !sp1Data) return 0;
	while ((getline(&line, &len, fp)) != -1) 
	{
		char** tokens;
		tokens = str_split(line, '\t', &len_str);

		//remember max line length
		if (len_str > len_str_max) len_str_max=len_str;

		//read to arrays allocate and free memory
		if (tokens)
		{
			int i;
			len_str_max=0;
			for (i = 0; *(tokens + i); i++)
			{
				if (i==0) 
				{ 
					sp1Stopbec[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					if (sp1Stopbec[amount_str])
					{
						strcpy(sp1Stopbec[amount_str], *(tokens + i));
					} 
					else
					{
						//return 0;
					}
				}
				else
				{
					(sp1Data[amount_str][i-1])=atoi(*(tokens + i));
					len_str_max++;
				}

				free(*(tokens + i));
			}
			free(tokens);
		}
		amount_str++;

	}
	(*columns)=len_str_max;
	(*rows)=amount_str;
	//printf("READ of FILE to memory was good");
	fclose(fp);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////
int WriteKeyColAndIntTableToFile(const char* Name, char** sp1Stopbec, int ** sp1Data,int *len_str_max, int *amount_str)
{
	FILE* mf=fopen(Name,"w");
	

	if (mf == NULL) return 0;
	//else printf ("open for write done\n");

	//fprintf (mf,"write file test");
	int a=0;
	int b=0;
	for (a=0;a<*amount_str;a++)
	{
		fprintf(mf,"%s\t",sp1Stopbec[a]);
		//printf("%s\t",sp1Stopbec[a]);

		for (b=0;b<*len_str_max;b++)
		{
			if (b<*len_str_max-1)
			{
				fprintf(mf,"%d\t",(sp1Data[a][b]));
				//printf("[%d][%d]=%d ",a,b,(sp1Data[a][b]));
			}
			else
			{
				fprintf(mf,"%d",(sp1Data[a][b]));
				//printf("[%d][%d]=%d_end",a,b,(sp1Data[a][b]));
			}


		}
		fprintf(mf,"\n");
		//printf("\n");
	}


	//printf ("Write done\n");


	fclose (mf);
	//printf ("File Closed\n");
	return 1;
}
//////////////////////////////////////////////////////////////////////////
int ReadTwoKeyColAndFloatColFromFile(const char* Name, char** StrCol1, char** StrCol2, float *FloatData, int *columns, int *rows)
{
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	int result=1;
	int amount_str=0;
	char* line = NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	if (!StrCol1 || !StrCol2) return 0;
	while ((getline(&line, &len, fp)) != -1) 
	{
		char** tokens;
		tokens = str_split(line, '\t', &len_str);

		//remember max line length
		if (len_str > len_str_max) len_str_max=len_str;

		//read to arrays allocate and free memory
		if (tokens)
		{
			int i;
			len_str_max=0;
			for (i = 0; *(tokens + i); i++)
			{
				if (i==0) 
				{ 
					//string
					StrCol1[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					if (StrCol1[amount_str])
					{
						strcpy(StrCol1[amount_str], *(tokens + i));
					} 
				}
				else if (i==1)
				{
					//string
					StrCol2[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					if (StrCol2[amount_str])
					{
						strcpy(StrCol2[amount_str], *(tokens + i));
					} 
				}
				else if (i==2)
				{
					FloatData[amount_str]=strtod(*(tokens + i),NULL);
					//float 2.552
				}

				free(*(tokens + i));
			}
			free(tokens);
		}
		amount_str++;

	}
	(*columns)=len_str_max;
	(*rows)=amount_str;
	//printf("READ of FILE to memory was good");
	fclose(fp);
	return result;
}
/////////////////////////////////////////////////////////////////////////
int ReadTwoKeyColFromFile(const char* Name, char** StrCol1, char** StrCol2,int *columns, int *rows)
{
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	int result=1;
	int amount_str=0;
	char* line = NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	if (!StrCol1 || !StrCol2) return 0;
	while ((getline(&line, &len, fp)) != -1) 
	{
		char** tokens;
		tokens = str_split(line, '\t', &len_str);

		//remember max line length
		if (len_str > len_str_max) len_str_max=len_str;

		//read to arrays allocate and free memory
		if (tokens)
		{
			int i;
			len_str_max=0;
			for (i = 0; *(tokens + i); i++)
			{
				if (i==0) 
				{ 
					StrCol1[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					if (StrCol1[amount_str])
					{
						strcpy(StrCol1[amount_str], *(tokens + i));
					} else printf("Lack of Malloc in 1 of ReadKnownTwoKeyColFromFile");
				}
				else if (i==1)
				{
					StrCol2[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					if (StrCol2[amount_str])
					{
						strcpy(StrCol2[amount_str], *(tokens + i));
					} else printf("Lack of Malloc in 2 of ReadKnownTwoKeyColFromFile");
					
				}

				free(*(tokens + i));
			}
			free(tokens);
		}
		amount_str++;

	}
	(*columns)=len_str_max;
	(*rows)=amount_str;
	//printf("READ of FILE to memory was good");
	fclose(fp);
	return result;
}
/////////////////////////////////////////////////////////////////////////
//StrCol1, StrCol2 must be of equal amount of str
int WriteTwoKeyColToFile(const char* Name, char** StrCol1, char** StrCol2, int *amount_str)
{
	FILE* mf=fopen(Name,"w");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");

	int a=0;
	for (a=0;a<*amount_str;a++)
	{
		fprintf(mf,"%s\t",StrCol1[a]);
		fprintf(mf,"%s\n",StrCol2[a]);
		//printf("%s\t",StrCol1[a]);
		//printf("%s\n",StrCol2[a]);
	}
	fclose (mf);
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
int FPrintCOut(const char* Name, char* Str)
{
	FILE* mf=fopen(Name,"a");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");
	//time 
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(mf, "Time...\n");
	fprintf(mf, "now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(mf,"%s\n",Str);
	fclose (mf);
	return 1;
}
///////////////////////////////////////////////////////////////////////////
int WriteTwoKeyColAndFloatColToFile(const char* Name, char** StrCol1, char** StrCol2, float *FloatData, int *amount_str)
{
	FILE* mf=fopen(Name,"w");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");

	int a=0;
	for (a=0;a<*amount_str;a++)
	{
		fprintf(mf,"%s\t",StrCol1[a]);
		fprintf(mf,"%s\t",StrCol2[a]);
		fprintf(mf,"%.4f\n", FloatData[a]);
	}
	fclose (mf);
	return 1;
}
///////////////////////////////////////////////////////////////////////////
int MakeListOfDir(char* DirName,char **nameList)
{
	DIR           *d;
	struct dirent *dir;
	int count = 0;
	int index = 0;
	d = opendir(DirName);

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if( strcmp( dir->d_name, "." ) == 0 || strcmp( dir->d_name, ".." ) == 0 )
				continue;
			if( dir->d_type == DT_DIR )
			{
				nameList[count] = (char*) malloc (strlen(dir->d_name)+1);
				strcpy(nameList[count],dir->d_name);
				printf("<DIR> %s\n", dir->d_name);
				count++;
			}
			else
			{
				//it is not directory do nothing
				//printf("%s\n", dir->d_name);
				//strcpy(name[count],dir->d_name);
			}

		}

		closedir(d);
	}
	return count;
}

///returns number of printed rows
int PrintTwoStringStructure( struct TStCol *src)
{
	int i = 0;
	for (i = 0; i<src->size; i++)
	{
		printf("%s[%d] ",src->StrCol1[i],i); 
		printf("%s[%d]\n",src->StrCol2[i],i); 
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////
int FPrintTwoStringStructure(const char* Name, struct TStCol *src)
{
	FILE* mf=fopen(Name,"w");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");

	int i = 0;
	for (i = 0; i<src->size; i++)
	{
		//fprintf(mf,"%s[%d] ",src->StrCol1[i],i); 
		//fprintf(mf,"%s[%d]\n",src->StrCol2[i],i); 
		fprintf(mf,"%s\t",src->StrCol1[i],i); 
		fprintf(mf,"%s\n",src->StrCol2[i],i); 
	}
	//fprintf(mf,"\nEnd List Element\n"); 
	fclose (mf);
	return i;
}
/////////////////////////////////////////////////////////////////////////
//int sizeof dst or rows
int TStColCopy(struct TStCol *src, struct TStCol *dst)
{
	dst->StrCol1 = (char**)malloc(sizeof(char*) * (src->size));
	dst->StrCol2 = (char**)malloc(sizeof(char*) * (src->size));
	
	if (!(dst->StrCol1) || !(dst->StrCol2))
	printf("Lack of Malloc of TStColCopy");
	
	for (int i = 0; i< src->size; i++)
	{
			asprintf(&(dst->StrCol1[i]),"%s",src->StrCol1[i]); 
			asprintf(&(dst->StrCol2[i]),"%s",src->StrCol2[i]); 
	}
	dst->size=src->size;
	return sizeof(*dst);
}
int FreeTStCol(struct TStCol *dst)
{
	for (int i = 0; i< dst->size; i++)
	{
			free(dst->StrCol1[i]); 
			free(dst->StrCol2[i]); 
	}
	free(dst->StrCol1);
	free(dst->StrCol2);;
	dst->size=0;
	return sizeof(dst->size);
}
///////////////////////////////////////////////
int FreeArraySet(struct ArraySet *dst)
{
	for (int i = 0; i< dst->raws; i++)
		{
				dst->Labels[i]=0; 
				for (int j = 0; j< dst->cols; j++)
					{
						dst->IntMtrx[i][j]=0; 
					}
				dst->IntMtrx[i]=NULL; 
		}	

	free(dst->Labels);
	free(dst->IntMtrx);
	dst->raws=0;
	dst->cols=0;
	return sizeof(*dst);
}
/*
typedef struct ArraySet {
	int raws;
	int cols;
	int** IntMtrx;
	int* Labels;
} ArraySet;
typedef struct TStCol {
	int size;
	char** StrCol1;
	char** StrCol2;
} TStCol;
*/
//////////////////////////////////////////////////////////////////////////
int FreeStrColIntMtrx(struct StrColIntMtrx *dst)
{
	//char *diag;
	//asprintf(&diag,"%s %s %s",dst->size_h,dst->size_l); 
	//FPrintCOut("result.log","1432\n");
	//FPrintCOut("result.log","1432\n");
	for (int i = 0; i< dst->size_h; i++)
	{
			for (int j = 0; j< dst->size_l; j++)
			{
				dst->IntMtrx[i][j]=0; 
			}
			dst->IntMtrx[i]=NULL;
			free(dst->StrCol[i]); 
	}
	free(dst->StrCol);
	free(dst->IntMtrx);
	dst->size_l=0;
	dst->size_h=0;
	return 1;
}
/////////////////////////////////////////////////////////////////////////
//int sizeof dst or rows
//it is supposed that the row number is already counted, but it is possible
//to implement this function for adding new elements to structure
int CopyTwoStringToStruct(char** StrCol1, char** StrCol2, int *amount_str, struct TStCol *dst)
{
	int add= *amount_str; 
	dst->StrCol1 = (char**)realloc(NULL, sizeof(char*) * (add));
	dst->StrCol2 = (char**)realloc(NULL, sizeof(char*) * (add));
	
	if (!(dst->StrCol1) || !(dst->StrCol2))
	printf("CopyTwoStringToStruct");
	
	dst->size=amount_str;
	
	for (int i = 0; i<*amount_str; i++)
	{
		asprintf(&(dst->StrCol1[i]),"%s",StrCol1[i]); 
		asprintf(&(dst->StrCol2[i]),"%s",StrCol2[i]); 
	}
	dst->size=amount_str;
	return sizeof(*dst);
}
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
char* DeleteLastN(char *str)
{
	char* pch=NULL;
	char* toTrim = (char*)malloc((strlen(str)+1)*sizeof(char*));
	strcpy(toTrim,str);
	pch=strchr(toTrim,'\n');
	if (pch)
	{ 
		memmove(pch, pch+1, strlen(pch));
		//printf("found ");
	}				
	strcpy(str,toTrim);
	free(toTrim);
	return str;
}
/////////////////////////////////////////////////////////////////////////
//int sizeof dst or rows
//it is supposed that the row number is already counted, but it is possible
//to implement this function for adding new elements to structure
int ApndTStColTSColl(char** StrCol1, char** StrCol2, int amount_str, struct TStCol *dst)
{
	int start=dst->size;
	//printf("int start=dst->size %d\n",start);
	int add=amount_str;
	//printf("add=amount_str %d\n",add);
	if (dst->StrCol1 && dst->StrCol2 && dst->size)
	{	
		dst->StrCol1 = (char**)realloc(dst->StrCol1, sizeof(char*)*(start+add));
		dst->StrCol2 = (char**)realloc(dst->StrCol2, sizeof(char*)*(start+add));
		
		if (!(dst->StrCol1) || !(dst->StrCol2)) printf("CopyTwoStringToStruct");
		
		dst->size=start+add;
		//printf("\nRealloc\n");
	}
	else
	{	
		dst->StrCol1 = (char**)malloc(sizeof(char*)*(add));
		dst->StrCol2 = (char**)malloc(sizeof(char*)*(add));
		
		if (!(dst->StrCol1) || !(dst->StrCol2)) printf("CopyTwoStringToStruct");
		
		dst->size=add;
		start=0;
		//printf("\nFirst malloc\n");
	}

	for (int i = 0; i<add; i++)
	{
		asprintf(&(dst->StrCol1[i+start]),"%s",StrCol1[i]); 
		DeleteLastN(dst->StrCol1[i+start]);
		asprintf(&(dst->StrCol2[i+start]),"%s",StrCol2[i]); 
		//printf(" strings are transmitting[%d] from StrCol1[%d] ",(i+start),i);
	}
	return sizeof(*dst);
}
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//Merdge two TStCol structures
//If destination is empty it MUST be initialized by zeros!!!!!!!!!!!!!
//arbitrary dst->size leads to segmentation fault 
int MrgeTwoTStCol(struct TStCol *src, struct TStCol *dst)
{
	int start=dst->size;
	int add=src->size;
	//printf("int start=dst->size %d\n",start);
	//printf("add=amount_str %d\n",add);
	if (!(src && src->StrCol1 && src->StrCol2 && src->size)) return 0;

	if (dst->StrCol1 && dst->StrCol2 && dst->size)
	{	
		dst->StrCol1 = (char**)realloc(dst->StrCol1, sizeof(char*)*(start+add));
		dst->StrCol2 = (char**)realloc(dst->StrCol2, sizeof(char*)*(start+add));
		
		if (!(dst->StrCol1) || !(dst->StrCol2)) printf("MrgeTwoTStCol");
		
		dst->size=start+add;
		//printf("\nRealloc\n");
	}
	else
	{	
		dst->StrCol1 = (char**)malloc(sizeof(char*)*(add));
		dst->StrCol2 = (char**)malloc(sizeof(char*)*(add));
		
		if (!(dst->StrCol1) || !(dst->StrCol2)) printf("MrgeTwoTStCol2");
		
		dst->size=add;
		start=0;
		//printf("\nFirst malloc\n");
	}

	for (int i = 0; i<add; i++)
	{
		asprintf(&(dst->StrCol1[i+start]),"%s",src->StrCol1[i]); 
		asprintf(&(dst->StrCol2[i+start]),"%s",src->StrCol2[i]); 
		//printf(" strings are transmitting[%d] from StrCol1[%d] ",(i+start),i);
	}
	return sizeof(*dst);
}
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////
//struct StrColIntMtrx s; Matrix of Int hXl
struct StrColIntMtrx *pStrColIntMtrx(struct StrColIntMtrx *s,char **sp1Stopbec, int **sp1Data, size_t size_l, size_t size_h)
{
	//struct StrColIntMtrx *s;
	s->size_h=size_h;
	s->size_l=size_l;
	s->StrCol=malloc(sizeof(char*) * (size_h));
	s->IntMtrx=malloc(size_h*sizeof(int*) + size_h*size_l*sizeof(int));
	
	if (!(s->StrCol) || !(s->IntMtrx)) printf("pStrColIntMtrx");
	
	if (sp1Stopbec && sp1Data)
	{	
	char* pc = s->IntMtrx;
	pc += size_h*sizeof(int*);
	for (size_t i=0; i < size_h; i++) 
	{
		s->IntMtrx[i] = pc + i*size_l*sizeof(int);
	}
	//StrCol initializing
	for (int i=0; i<size_h; i++)
	{
		asprintf(&(s->StrCol[i]),"%s",sp1Stopbec[i]); 
		for (int j=0; j<size_l; j++)
		{
			s->IntMtrx[i][j]=sp1Data[i][j];
		}
	}
	}
	if (s->StrCol && s->IntMtrx && size_h && size_l)
	{
		return s;
	}
}
///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////
//This takes ***pppChr of sorted char**, and takes position of the first unsorted element char** Start
//to replace int row according to the new sorted position ot strCol
//struct StrColIntMtrx s; Matrix of Int hXl, s must be created, and filled with zeros.
struct StrColIntMtrx *pPStrColIntMtrx(struct StrColIntMtrx *s,char ***pppChr,char** Start, int **sp1Data, int size_l, int size_h)
{
	//struct StrColIntMtrx *s;
	s->size_h=size_h;
	s->size_l=size_l;
	s->StrCol=malloc(sizeof(char*) * (size_h));
	s->IntMtrx=malloc(size_h*sizeof(int*) + size_h*size_l*sizeof(int));
	
	if (!(s->StrCol) || !(s->IntMtrx)) printf("pStrColIntMtrx");
	
	if (pppChr && sp1Data)
	{	
	char* pc = s->IntMtrx;
	pc += size_h*sizeof(int*);
	for (int i=0; i < size_h; i++) 
	{
		s->IntMtrx[i] = pc + i*size_l*sizeof(int);
	}
	//StrCol initializing
	for (int i=0; i<size_h; i++)
	{
		asprintf(&(s->StrCol[i]),"%s",*(pppChr)[i]); 
		for (int j=0; j<size_l; j++)
		{
			s->IntMtrx[i][j]=sp1Data[(int)(pppChr[i]-Start)][j];
		}
	}
	}
	if (s->StrCol && s->IntMtrx && size_h && size_l)
	{
		return s;
	}
}
///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////
//This sorts unsorted element char** Start strings accending and repleses int raw according to.
//to replace int row according to the new sorted position ot strCol
//struct StrColIntMtrx s; Matrix of Int hXl, s must be created, and filled with zeros.
struct StrColIntMtrx *pStrSortedColIntMtrx(struct StrColIntMtrx *s,char** Start, int **sp1Data, int size_l, int size_h)
{
	//sort string column accending
	/////////////////////////////////////
	char*** pVal;
	int n=size_h;
	pVal=malloc(n*sizeof(char**));
	
	if (!(pVal)) printf("pStrSortedColIntMtrx");
	
	for( int i = 0; i < n; i++)
	{
		pVal[i] =Start+i;
	}
	qsort(pVal, n, sizeof(char*), cmpfunc);
	/////////////////////////////////////
	
	//initializing s structure	
	s->size_h=size_h;
	s->size_l=size_l;
	s->StrCol=malloc(sizeof(char*) * (size_h));
	s->IntMtrx=malloc(size_h*sizeof(int*) + size_h*size_l*sizeof(int));
	
	if (!(s->StrCol) || !(s->IntMtrx)) printf("pStrColIntMtrx");
	
	if (pVal && sp1Data)
	{	
	char* pc = s->IntMtrx;
	pc += size_h*sizeof(int*);
	for (int i=0; i < size_h; i++) 
	{
		s->IntMtrx[i] = pc + i*size_l*sizeof(int);
	}
	//StrCol initializing by acceding sorted pVal *Start, Start is array of unsorted char*
	for (int i=0; i<size_h; i++)
	{
		asprintf(&(s->StrCol[i]),"%s",*(pVal)[i]); 
		//changing rows according to the sorted str
		for (int j=0; j<size_l; j++)
		{	
			s->IntMtrx[i][j]=sp1Data[(int)(pVal[i]-Start)][j];
		}
	}
	}
	free(pVal);
	if (s->StrCol && s->IntMtrx && size_h && size_l)
	{
		return s;
	}
}
///////////////////////////////////
struct StrColIntMtrx *MrgeTwoStrIntMtrx(struct StrColIntMtrx *src, struct StrColIntMtrx *dst)
{
	int start=dst->size_h;
	int add=src->size_h;
	int size_h=add+start;
	int size_l=dst->size_l;
	//if (!(src && src->StrCol && src->IntMtrx && src->size_h && src->size_l)) return 0;
	//if (src->size_l != dst->size_l && dst->size_l ) return 0;

	if (dst->StrCol && dst->size_h)
	{	

		//create dst copy		
		StrColIntMtrx s;
		memset(&s,0,sizeof(StrColIntMtrx));
		StrColIntMtrx* ps=&s;
		ps=pStrColIntMtrx(ps,dst->StrCol,dst->IntMtrx,dst->size_l,dst->size_h);

//FPrintStrColIntMtrx("Test1Matr", ps)

		//StrCol
		dst->StrCol=(char**)realloc(dst->StrCol,sizeof(char*)*(add+start));
		
		if (!(dst->StrCol)) printf("MrgeTwoStrIntMtrx");
		
		//StrCol initializing
		//First destination Str is already in place, so copy only ones from src

		for (int i=0; i<start; i++)
		{
			asprintf(&(dst->StrCol[i]),"%s",ps->StrCol[i]); 
			//printf("%s[%d]",dst->StrCol[i+start],(i+start)); 
		}
		for (int i=0; i<add; i++)
		{
			asprintf(&(dst->StrCol[i+start]),"%s",src->StrCol[i]); 
			//printf("%s[%d]",dst->StrCol[i+start],(i+start)); 
		}
		
		//IntMtrx
		dst->IntMtrx=realloc(dst->IntMtrx,(size_h*sizeof(int*) + size_h*size_l*sizeof(int)));
		char* pc = dst->IntMtrx;
		pc += (add+start)*sizeof(int*);		
		for (int i=0; i < add+start; i++) 
		{
				dst->IntMtrx[i] = pc + i*size_l*sizeof(int);
		}
		//printf("630\n");
		for (int i=0; i < start; i++) 
		{	
			//printf("\nrow=%d",i);
			for (int j=0; j<size_l; j++)
			{
				dst->IntMtrx[i][j]=ps->IntMtrx[i][j];
			}
		}
		for (int i=start; i < size_h; i++) 
		{	
			//printf("\nrow=%d",i);
			for (int j=0; j<size_l; j++)
			{
				dst->IntMtrx[i][j]=src->IntMtrx[i-start][j];
			}
		}

		//new size_h 
		dst->size_h=start+add;
		//free ps
		FreeStrColIntMtrx(ps);



	}
	else
	{	
		StrColIntMtrx *s=pStrColIntMtrx(s,src->StrCol,src->IntMtrx,src->size_l,src->size_h);
		dst=s;
		size_l=dst->size_l;
		size_h=dst->size_h;
	}
	//if (dst->StrCol && dst->IntMtrx && size_h && size_l)
	//{
		return dst;
	//} 
	//else return NULL;
}
/////////////////////////////////////////////////////////////////////////
int FPrintStrColIntMtrx(const char* Name, struct StrColIntMtrx *src)
{
	FILE* mf=fopen(Name,"a");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");
	for (size_t i=0; i < src->size_h; i++)
	{
		fprintf(mf,"%s\t",src->StrCol[i]);
		//printf("%s[%d]\t",src->StrCol[i],i);
		for (size_t j=0; j < src->size_l; j++)
		{
			if (j < src->size_l-1)
			{
				fprintf(mf,"%d\t",(src->IntMtrx[i][j]));
				//printf("%d[%d][%d]\t",(src->IntMtrx[i][j]),i,j);
			}
			else
			{
				fprintf(mf,"%d",(src->IntMtrx[i][j]));
				//printf("%d[%d][%d]",(src->IntMtrx[i][j]),i,j);
			}
		}
		fprintf(mf,"\n");
	}
	fclose (mf);
	return 1;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int FPrintLblIntMtrx(const char* Name, struct ArraySet *src)
{
	FILE* mf=fopen(Name,"w");
	if (mf == NULL) return 0;
	//else printf ("open for write done\n");
	for (int i=0; i < src->raws; i++)
	{
		fprintf(mf,"%.0f\t",src->Labels[i]);
		//printf("%s[%d]\t",src->StrCol[i],i);
		for (int j=0; j < src->cols; j++)
		{
			if (j < src->cols-1)
			{
				fprintf(mf,"%.0f\t",(src->IntMtrx[i][j]));
				//printf("%d[%d][%d]\t",(src->IntMtrx[i][j]),i,j);
			}
			else
			{
				fprintf(mf,"%.0f",(src->IntMtrx[i][j]));
				//printf("%d[%d][%d]",(src->IntMtrx[i][j]),i,j);
			}
		}
		fprintf(mf,"\n");
	}
	fclose (mf);
	return 1;
}
//Find key in src->StrCol and gives its row number in IntMtrx 
//src->StrCol must be accending sorted
//returns -1 if there is no occurences
int FindStrInKeyStrColIntMtrx(const char* key, struct StrColIntMtrx *src)
{
	char** pItem = NULL;
	if (!src || !key || !src->StrCol) return -1;
	//asprintf(&key,"%s",sp1Stopbec[amount_str-5]); 
	//printf("Let us find %s\n",key);
	pItem = (char **)bsearch (key, src->StrCol, src->size_h, sizeof(char*), cmpfunc2);
    if (pItem != NULL)
        return (int)(pItem-src->StrCol);
    else
        return -1;
	//how to get find IntMtrx row consistent key
	//for (i=0; i<sp2Espr->size_l; i++)
	    //printf("%d[%d,%d] ",sp2Espr->IntMtrx[(int)(pItem-sp2Espr->StrCol)][i], (int)(pItem-sp2Espr->StrCol), i);
	//printf("%s ",*pItem);
	//printf("%d ",(int)(pItem-sp2Espr->StrCol));
	//return 1;
}
///////////////////////////////////////////////////////////////////////////
int ReadKnownTwoKeyColFromFile(const char* Name, char** StrCol1, char** StrCol2,int *columns, int *rows,struct StrColIntMtrx *src1,struct StrColIntMtrx *src2)
{
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	int result=1;
	int amount_str=0;
	int dropped_str=0;
	char* line = NULL;
	char* toSearchFor=NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	int ItemInSrc1=0, ItemInSrc2=0;
	if (!StrCol1 || !StrCol2) return 0;
	while ((getline(&line, &len, fp)) != -1) 
	{
		char** tokens;
		tokens = str_split(line, '\t', &len_str);

		//remember max line length
		if (len_str > len_str_max) len_str_max=len_str;

		//read to arrays allocate and free memory
		if (tokens)
		{
			int i;
			len_str_max=0;
			for (i = 0; *(tokens + i); i++)
			{
					toSearchFor = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					
					if (!toSearchFor) printf("ReadKnownTwoKeyColFromFile failed to allocate memory");
					
					strcpy(toSearchFor, *(tokens + i));
					DeleteLastN(toSearchFor);
					ItemInSrc1=FindStrInKeyStrColIntMtrx(toSearchFor, src1);
					ItemInSrc2=FindStrInKeyStrColIntMtrx(toSearchFor, src2);
				if (ItemInSrc1 > -1 || ItemInSrc2 > -1)
				{
					if (i==0) 
					{ 
						StrCol1[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
						if (StrCol1[amount_str])
						{
							strcpy(StrCol1[amount_str], *(tokens + i));
						} else printf("Failed to allocate in ReadKnownTwoKeyColFromFile-2\n");
					}
					else if (i==1)
					{
						StrCol2[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
						if (StrCol2[amount_str])
						{
							strcpy(StrCol2[amount_str], *(tokens + i));
						} else printf("Failed to allocate in ReadKnownTwoKeyColFromFile-3\n");
					}
				} 
				else 
				{
					printf("Doesn`t added %s %d %d ",StrCol1[amount_str], ItemInSrc1,ItemInSrc2);
					dropped_str++;
				}

				free(*(tokens + i));
				free(toSearchFor);
			}
			free(tokens);
		}
		amount_str++;

	}
	(*columns)=len_str_max;
	(*rows)=amount_str-dropped_str;
	printf("Added Rows %d ",(*rows));
	//printf("READ of FILE to memory was good");
	fclose(fp);
	return result;
}
/////////////////////////////////////////////////////////////////////////////
//returns amount of read and no dropped string
///////////////////////////////////////////////////////////////////////////
size_t ReadTwoColFromFileMtoN(const char* Name, char** StrCol1, char** StrCol2,size_t *columns, size_t *SartR,size_t *EndR, struct StrColIntMtrx *src1,struct StrColIntMtrx *src2)
{
	//counting strings-rows
	//int r=0;
	//int* rows=&r; //dummy for bachword compatibility
	
	size_t start=(*SartR);
	size_t end=(*EndR);
	size_t pos=0; //counter position
	
	FILE* fp = fopen(Name, "r");
	if (fp == NULL) exit(EXIT_FAILURE);
	int result=1;
	size_t amount_str=0;
	size_t dropped_str=0;
	char* line = NULL;
	char* toSearchFor=NULL;
	size_t len = 0;
	size_t len_str=0;
	size_t len_str_max=0;
	int ItemInSrc1=0, ItemInSrc2=0;
	int firstIsPresent=0;
FPrintCOut("result.log","ReadTwoColFromFileMtoN 1112\n");		
	if (!StrCol1 || !StrCol2) return 0;
	while ((getline(&line, &len, fp)) != -1) 
	{
		pos=pos+1;
		if (end < start) break; //smth go wrong
		//must read only form start row
		if (pos < start) {
		continue;
		}
		//must finish after end
		if (pos > end) 
			break;
//FPrintCOut("result.log","ReadTwoColFromFileMtoN 1124 ");				
		char** tokens;
		tokens = str_split(line, '\t', &len_str);
		//remember max line length
		if (len_str > len_str_max) len_str_max=len_str;
		//read to arrays allocate and free memory
		if (tokens)
		{
			int i;
			len_str_max=0;
			for (i = 0; *(tokens + i); i++)
			{
					toSearchFor = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
					
					if (!toSearchFor) printf("ReadKnownTwoKeyColFromFile failed to allocate memory");
					
					strcpy(toSearchFor, *(tokens + i));
					DeleteLastN(toSearchFor);
					ItemInSrc1=FindStrInKeyStrColIntMtrx(toSearchFor, src1);
					ItemInSrc2=FindStrInKeyStrColIntMtrx(toSearchFor, src2);
				//if (ItemInSrc1 > -1 || ItemInSrc2 > -1)
				{
					if (i==0 && ItemInSrc1 > -1) 
					{ 
						StrCol1[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
						if (StrCol1[amount_str])
						{
							strcpy(StrCol1[amount_str], *(tokens + i));
							firstIsPresent=1;
						} else printf("Failed to allocate in ReadKnownTwoKeyColFromFile-2\n");
					}
					else if (i==1 && ItemInSrc2 > -1 && firstIsPresent==1)
					{
						StrCol2[amount_str] = (char*)malloc((strlen(*(tokens + i))+1)*sizeof(char*));
						if (StrCol2[amount_str])
						{
							strcpy(StrCol2[amount_str], *(tokens + i));
						} else printf("Failed to allocate in ReadKnownTwoKeyColFromFile-3\n");
					}
					else if (i == 0 || i == 1)
						{
							if (firstIsPresent==1) free(StrCol1[amount_str]);
							dropped_str=1;
						}
						
				} 
				free(*(tokens + i));
				free(toSearchFor);
			}
			free(tokens);
		}
		
		amount_str=amount_str+1-dropped_str;

	}
FPrintCOut("result.log","ReadTwoColFromFileMtoN 1179 \n");		
	(*columns)=len_str_max;
	printf("Added Rows %zu ",(amount_str));
	//printf("READ of FILE to memory was good");
	fclose(fp);
	return amount_str;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int FillTrainSet(TStCol* ListOfTrain, TStCol* trainSet, StrColIntMtrx *sp1Espr, StrColIntMtrx *sp2Espr, char* labels)
{		
	int Item=-1;
	char* IntStr=NULL;
	char** arLabel=malloc(sizeof(char*));
	asprintf(&arLabel[0],"%s",labels); 
	//asprintf(&arLabel[0],"0"); 
	char* Result=NULL;
	char* temp=NULL;
		int count=0;
		for (int i = 0; i<ListOfTrain->size; i++)
		{
			//printf("iter %d ",i);	
			//printf("Let us find %s -",ListOfTrain.StrCol1[i]);
			Item=-1;
			Item = FindStrInKeyStrColIntMtrx(ListOfTrain->StrCol1[i], sp1Espr);
			if (Item > -1)
			{
			    //printf("It is %d place",Item);
				for (int ii=0; ii<sp1Espr->size_l; ii++)
				{
					//printf("%d[%d,%d] ",sp1Espr->IntMtrx[Item][i],Item,i);
					//printf("TEST");
					if (IntStr)
					{
						//new copy to repair memory leak 05022020 AVM
						asprintf(&temp,"%s",IntStr);
						free(IntStr);
						asprintf(&IntStr,"%s\t%d",temp,sp1Espr->IntMtrx[Item][ii]); 
						free(temp);
					}
					else
						asprintf(&IntStr,"%d",sp1Espr->IntMtrx[Item][ii]); 
					//printf("%s ",IntStr);
					//getchar();
				}
				//ApndTStColTSColl(&(ListOfTrain.StrCol1[i]), &IntStr, 1, &trainSet);
				//free(IntStr);
				//IntStr=NULL;
				//PrintTwoStringStructure(&trainSet);
			}
			else
			    printf ("- doesn't exist\n");
			
			/////find str2 in sp2Espr
			Item=-1;
			Item = FindStrInKeyStrColIntMtrx(ListOfTrain->StrCol2[i], sp2Espr);
			if (Item > -1)
			{
			    //printf("It is %d place",Item);
				for (int ii=0; ii<sp2Espr->size_l; ii++)
				{
					if (IntStr)
					{
						//new copy to repair memory leak 05022020 AVM
						asprintf(&temp,"%s",IntStr);
						free(IntStr);
						asprintf(&IntStr,"%s\t%d",temp,sp2Espr->IntMtrx[Item][ii]); 
						free(temp);
					}
					else
					asprintf(&IntStr,"%d",sp2Espr->IntMtrx[Item][ii]); 
					//printf("%s ",IntStr);
				//getchar();
				}
				//ApndTStColTSColl(&(ListOfTrain.StrCol2[i]), &IntStr, 1, &trainSet);
				//arLabel[0]=labels;
				//arLabel[1]='\0';
				
				ApndTStColTSColl(arLabel, &IntStr, 1, trainSet);
				count++;
				//PrintTwoStringStructure(&trainSet);
			}
			else
			    printf ("- doesn't exist\n");
							
			free(IntStr);
			IntStr=NULL;
		}
		free(arLabel[0]);
		free(arLabel);
	return count;
} 
/////////////////////////////////////////////////////////////////////////////
ArraySet* MakeArrayTrainSet(TStCol* src, ArraySet* dst)
{		
	int raws=src->size;
	int cols=0;
	//dst->raws=src->size;
	
	//int result=1;
	//int amount_str=0;
	char* line = NULL;
	size_t len = 0;
	int len_str=0;
	int len_str_max=0;
	
	//count elements
	for (int i=0; i < raws; i++)
	{

			//printf("%s[%d]\t",src->StrCol[i],i);
			//printf("%s", line); 	//string to parts
			char** tokens;
			asprintf(&line,"%s",src->StrCol2[i]);
			//printf("line=[%s]\n\n", line); 	//length of the string
			tokens = str_split(line, '\t', &len_str);
		
			//free memory
			if (tokens)
			{	cols=0;
				int ii;
				for (ii = 0; *(tokens + ii); ii++)
				{
					cols++;
					free(*(tokens + ii));
				}
				free(tokens);

			}
			free(line);
	}
	
	//allocate memory
	dst->Labels=malloc(sizeof(float) * raws);
FPrintCOut("result.log","1201\n");
char *Mem=NULL;
asprintf(&Mem,"%zu",(raws*sizeof(float*) + raws*cols*sizeof(float)));	

	dst->IntMtrx=malloc(raws*sizeof(float*) + raws*cols*sizeof(float));

FPrintCOut("result.log",Mem); 	
if (dst->IntMtrx) FPrintCOut("result.log","allocated OK! 1206"); 
else FPrintCOut("result.log","Failed to allocate"); 	
free(Mem);
FPrintCOut("result.log","1211\n");
	char* pc = dst->IntMtrx;
	pc += raws*sizeof(float*);
	for (int jj=0; jj < raws; jj++) 
	{
		dst->IntMtrx[jj] = pc + jj*cols*sizeof(float);
	}
	//if (dst->IntMtrx && src->StrCol2)
	{	
		for (int i=0; i < raws; i++) 
		{
			//dst->Labels[i] = atoi(src->StrCol1[i]);
			dst->Labels[i] = atof(src->StrCol1[i]);
			char** tokens;
			asprintf(&line,"%s",src->StrCol2[i]);
			tokens = str_split(line, '\t', &len_str);
			//remeber max line length
			if (tokens)
			{
				int ii;
				for (ii = 0; *(tokens + ii); ii++)
				{
					//(dst->IntMtrx[i][ii])=atoi(*(tokens + ii));
					dst->IntMtrx[i][ii]=atof(*(tokens + ii));
					free(*(tokens + ii));
				}
				free(tokens);
			}
			free(line);
			
		}
	}
	dst->raws=raws;
	dst->cols=cols;
	return dst;

} 
/////////////////////////////////////////////////////////////////////////////
//this is attempt to combine two function MakeArrayTrainSet and FillTrainSet
/////////////////////////////////////////////////////////////////////////////
//int FillTrainSet(TStCol* ListOfTrain, TStCol* trainSet, StrColIntMtrx *sp1Espr, StrColIntMtrx *sp2Espr, char* labels)
ArraySet* MkArrayTrainSetFromIntMtrx(TStCol* ListOfTrain, StrColIntMtrx *sp1Espr, StrColIntMtrx *sp2Espr, char* labels, ArraySet* dst)
{			
	int raws,cols;
	raws=ListOfTrain->size;
	cols=sp1Espr->size_l+sp2Espr->size_l;
	int Item=-1;
	char* toSearchFor=NULL;

	//allocate memory
	dst->Labels=malloc(sizeof(float) * raws);
	dst->IntMtrx=malloc(((size_t)raws)*sizeof(float*) + ((size_t)raws)*((size_t)cols)*sizeof(float));
	
//debug
char *Mem=NULL;
char *Mem2=NULL;
char* IntStr=NULL;
asprintf(&Mem2,"%zu - raws, %zu - sizeof(float*),%zu - cols, %zu - sizeof(float)",raws, sizeof(float*), cols, sizeof(float) );	
asprintf(&Mem,"%zu",(((size_t)raws)*sizeof(float*) + ((size_t)raws)*((size_t)cols)*sizeof(float)));	
FPrintCOut("result.log",Mem); 	
FPrintCOut("result.log",Mem2); 	
if (dst->IntMtrx) FPrintCOut("result.log","allocated OK! 1272"); 
else FPrintCOut("result.log","Failed to allocate"); 	
free(Mem);
free(Mem2);
//debug

	char* pc = dst->IntMtrx;
	pc += raws*sizeof(float*);
	for (int jj=0; jj < ListOfTrain->size; jj++) 
	{
		dst->IntMtrx[jj] = pc + ((size_t)jj)*((size_t)cols)*sizeof(float);
	}	
		//int count=0;
		//int deduction=0;
		for (int i = 0; i<ListOfTrain->size; i++)
		{
			//printf("iter %d ",i);	
			//printf("Let us find %s -",ListOfTrain.StrCol1[i]);
			dst->Labels[i] = atof(labels);
			
			toSearchFor = (char*)malloc(strlen(ListOfTrain->StrCol1[i])*sizeof(char));
			if (!toSearchFor) printf("ReadKnownTwoKeyColFromFile failed to allocate memory");
			strcpy(toSearchFor, ListOfTrain->StrCol1[i]);
			//DeleteLastN(toSearchFor);
			
			Item=-1;
			Item = FindStrInKeyStrColIntMtrx(toSearchFor, sp1Espr);
			free(toSearchFor);
			if (Item > -1)
			{
			    //printf("It is %d place",Item);
				for (int ii=0; ii<sp1Espr->size_l; ii++)
				{
					//printf("%d[%d,%d]->",sp1Espr->IntMtrx[Item][ii],Item,ii);
					//printf("TEST");
					//if (dst->IntMtrx[i][ii])
					{
						//asprintf(&IntStr,"%d",sp1Espr->IntMtrx[Item][ii]); 
						//dst->IntMtrx[i][ii]=atof(IntStr);
						//free(IntStr);
						dst->IntMtrx[i][ii]=(float)(sp1Espr->IntMtrx[Item][ii]);
						//printf("%.0f[%d,%d] ",dst->IntMtrx[i][ii],i,ii);
						//does it wrong?
					}
				}
				//PrintTwoStringStructure(&trainSet);
			}
			else
			{
			    printf ("%s - doesn't exist in the file 1",ListOfTrain->StrCol1[i]);
				goto nextRaw;
			}
			/////find str2 in sp2Espr
			toSearchFor = (char*)malloc(strlen(ListOfTrain->StrCol2[i]+1)*sizeof(char));
			if (!toSearchFor) printf("ReadKnownTwoKeyColFromFile failed to allocate memory");
			strcpy(toSearchFor, ListOfTrain->StrCol2[i]);
			//DeleteLastN(toSearchFor);
			Item=-1;
			Item = FindStrInKeyStrColIntMtrx(toSearchFor, sp2Espr);
			free(toSearchFor);
			
			if (Item > -1)
			{
			    //printf("It is %d place",Item);
				for (int ii=0; ii<sp2Espr->size_l; ii++)
				{
					//printf("%d[%d,%d]->",sp2Espr->IntMtrx[Item][ii],Item,ii);
					//if (dst->IntMtrx[i][(sp1Espr->size_l)+ii])
					{
						//asprintf(&IntStr,"%d",sp2Espr->IntMtrx[Item][ii]); 
						//dst->IntMtrx[i][(sp1Espr->size_l)+ii]=atof(IntStr);
						//free(IntStr);
						//printf("%f ",sp1Espr->IntMtrx[Item][i]);
						dst->IntMtrx[i][(sp1Espr->size_l)+ii]=(float)(sp2Espr->IntMtrx[Item][ii]);
						//printf("%.0f[%d,%d] ",dst->IntMtrx[i][(sp1Espr->size_l)+ii],i,((sp1Espr->size_l)+ii));
						//does it wrong?
						//dst->IntMtrx[i][sp1Espr->size_l+ii]=(float)(sp2Espr->IntMtrx[Item][ii]);
					}
				}
				//ApndTStColTSColl(arLabel, &IntStr, 1, trainSet);
				//count++;
				//PrintTwoStringStructure(&trainSet);
			}
			else
			{
			    printf ("%s - doesn't exist in the file 2",ListOfTrain->StrCol2[i]);
				goto nextRaw;
			}
			
			//printf("\n");
			nextRaw:
			;
			////getchar();
			//deduction=deduction+1;
			
		}
	dst->raws=raws;
	dst->cols=cols;
	return dst;
}
/////////////////////////////////////////////////////////////////////////////
void* getDataFromRun(int argc, char** argv)
{
	
}
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) 
{
	//initialisation
	
		char* max_depth;
		char* eta;
		char* subsample;
		char* colsample_bytree;
		char* colsample_bylevel;
		char* min_child_weight;
		char* gamma;
		char* alpha;
		char* lambdaParam;
		int numIterations;
		char* eval_metric;
		char* scale_pos_weight;
		char* modelNam;
		char* species1File;
		char* species2File;
		char* predictPairsFile;
		char* modelDir;
		char* PredDir;
		char* isHeader1;
		char* isHeader2;
		int MinIter,MaxIter; //ot kakoi i do kakoi iteracii $iter
		int Nthread; //kolichesvo processov
		//mkdir("iter_$iter/results/predictions");
		//iter_$iter/results/training/expression iter_$iter/results/predictions
		//mkdir("iter_$iter/results/predictions");
		size_t ReadPortion=0; // how much strings for prediction to write to dmatrix
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
	
	{
		
		asprintf(&species1File, "%s", argv[1]);
		asprintf(&species2File, "%s", argv[2]);
		asprintf(&predictPairsFile, "%s", argv[3]);
		
		MinIter=atoi(argv[4]);
		MaxIter=atoi(argv[5]); 
		ReadPortion=atoi(argv[6]);
		Nthread=atoi(argv[7]);
		//ReadPortion=atoi(argv[8]); //какими порциями обрабатывать файл пар
				
		for( int i = 0; i < argc; ++i ) 
			printf( "argv[ %d ] = %s\n", i, argv[ i ] );
	}
	char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
   }

	char* modelNameWithDir;
	char* sNthread;
	asprintf(&sNthread,"%d",Nthread); 
	int amount_str=0;  //rows
	int len_str_max=0; //max columns for the case of not rectangle table

	FPrintCOut("result.log","1388\n");
	printf("countTableElemFromFile(species1File, &len_str_max, &amount_str)\n");
	countTableElemFromFile(species1File, &len_str_max, &amount_str);
	
	/////////////////////////////////////////////////
	//create array of char* for first name column size

	printf("Allocation for Sp1Stolbes\n");
	char** sp1Stopbec = 0;
	sp1Stopbec = (char**)malloc(sizeof(char*) * (amount_str));
	if (sp1Stopbec) printf("Allocation for Sp1Stolbes is done"); //else printf("Failed to allocate 1384");
	//array of amount_str columns and len_str_max rows
	//printf("Allocation for Sp1Data\n");
	int ** sp1Data = malloc(amount_str*sizeof(int*) + amount_str*(len_str_max)*sizeof(int));
	char * pc = sp1Data;
	pc += amount_str*sizeof(int*);
	int i=0;
	for (i=0; i<amount_str; i++)
	    sp1Data[i] = pc + i*len_str_max*sizeof(int);
	//free(sp1Data);
	if (sp1Data)
	{
		printf("Allocation for Sp1Data is done\n");
	} else printf("Failed to allocate 1398\n");

	//////////////////////////////////////////////////////////////////////////
	//read file to Sp1Stolbec and sp1Data/////////////////////////////////////
	//ReadKeyColAndIntTableFromFile("ATH_TMAP_TGR_star.txt", sp1Stopbec, sp1Data, &len_str_max, &amount_str);
	ReadKeyColAndIntTableFromFile(species1File, sp1Stopbec, sp1Data, &len_str_max, &amount_str);
	printf("Rows(amount_str)=%d Columns(len_str_max)=%d from %s\n",amount_str,len_str_max,species1File);

FPrintCOut("result.log","1432\n");

	//read file 2 ATH_TMAP_TGR_star.txt
	int amount_str2=0;  //rows
	int len_str_max2=0; //max columns for the case of not rectangle table

	//read file/////////////////////////////////////
	//file to count, a result: amount_str - rows, len_str_max - columns
	//countTableElemFromFile("AMB_TMAP_TGR_star.txt", &len_str_max2, &amount_str2);
	countTableElemFromFile(species2File, &len_str_max2, &amount_str2);
	
	/////////////////////////////////////////////////
	//create array of char* for first name column size

	printf("Allocation for Sp1Stolbes\n");
	char** sp1Stopbec2 = 0;
	sp1Stopbec2 = (char**)malloc(sizeof(char*) * (amount_str2));
	if (sp1Stopbec2) printf("Allocation for Sp1Stolbes is done"); else printf("Failed to allocate 1425");
	//array of amount_str columns and len_str_max rows
	int ** sp1Data2 = malloc(amount_str2*sizeof(int*) + amount_str2*(len_str_max2)*sizeof(int));
	char * pc2 = sp1Data2;
	pc2 += amount_str2*sizeof(int*);
	i=0;
	for (i=0; i<amount_str2; i++)
	    sp1Data2[i] = pc2 + i*len_str_max2*sizeof(int);

	if (sp1Data2)
	{
		printf("Allocation for Sp1Data is done\n");
	} else printf("Failed to allocate 1439\n");

	//////////////////////////////////////////////////////////////////////////
	//read file to Sp1Stolbec and sp1Data/////////////////////////////////////
	//len_str_max - real number of col, raw - amount_str
FPrintCOut("result.log","1513\n");
	t = time(NULL);
	tm = *localtime(&t);
	printf("Keys reading are started...\n");
	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	ReadKeyColAndIntTableFromFile(species2File, sp1Stopbec2, sp1Data2, &len_str_max2, &amount_str2);
	printf("Rows(amount_str)=%d Columns(len_str_max)=%d from %s\n",amount_str2,len_str_max2,species2File);
	
FPrintCOut("result.log","1521\n");	
	t = time(NULL);
	tm = *localtime(&t);
	printf("Keys reading are started...\n");
	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	printf("Rows(amount_str)=%d Columns(len_str_max)=%d from %s\n",amount_str2,len_str_max2,species2File);
	
	//test new structure
	StrColIntMtrx sp1Esp,sp2Esp;
	memset(&sp1Esp, 0, sizeof(StrColIntMtrx)); 
	memset(&sp2Esp, 0, sizeof(StrColIntMtrx));
	StrColIntMtrx *sp1Espr=&sp1Esp,*sp2Espr=&sp2Esp;
		
	sp1Espr=pStrSortedColIntMtrx(sp1Espr,sp1Stopbec,sp1Data,len_str_max,amount_str);
	printf("sp1->size_h=%d s->size_l=%d\n",sp1Espr->size_h,sp1Espr->size_l);
	//FPrintStrColIntMtrx("Sp1Data\n",sp1Espr);
	
	sp2Espr=pStrSortedColIntMtrx(sp2Espr,sp1Stopbec2,sp1Data2,len_str_max2,amount_str2);
	printf("sp2->size_h=%d s->size_l=%d\n",sp2Espr->size_h,sp2Espr->size_l);
	//FPrintStrColIntMtrx("Sp2Data\n",sp2Espr);
	
//try to free memory as fast as it possible
	for (int j=0;j<amount_str;j++)
		{
	   	  free(sp1Stopbec[j]);
	    }
	  free(sp1Stopbec);
	  free(sp1Data);

	   for (int j=0;j<amount_str2;j++)
	     {
	   	  free(sp1Stopbec2[j]);
	     }
	  free(sp1Stopbec2);
	  free(sp1Data2);

FPrintCOut("result.log","1543\n");


	TStCol gNamsToPredict;
	memset(&gNamsToPredict, 0, sizeof(gNamsToPredict));
	size_t amount_str7=0;  //rows
	int len_str_max7=0; //max columns for the case of not rectangle table	
	countTableElemFromFile(predictPairsFile, &len_str_max7, &amount_str7);
	size_t TotalRowsInFile=amount_str7;
	if (ReadPortion == 0) ReadPortion=TotalRowsInFile;
	
FPrintCOut("result.log","1555\n");

	//check if exist and create
	struct stat st = {0};	
	DMatrixHandle h_train,h_test;
	char labelsTrain='1';
		
	ArraySet* pTrainSetArr;
	ArraySet TrainSetArr;
	pTrainSetArr=&TrainSetArr;
	memset(pTrainSetArr,0,sizeof(ArraySet));
	
	size_t start=1;
	size_t end=ReadPortion;
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//MAIN CYCLE STARTS
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	printf("start=%d end=%d TotalRowsInFile=%zu ReadPortion=%zu\n",start,end,TotalRowsInFile,ReadPortion);
	
	while (start < TotalRowsInFile+1)
{
	printf("start=%zu end=%zu\n",start,end);
	//allocate memory for two string arrays
	//case the rest part of the file is less than Portion to read
	//if (end > TotalRowsInFile && start <= TotalRowsInFile) ReadPortion=TotalRowsInFile+1-start;
	
	gNamsToPredict.StrCol1 = (char**)malloc(sizeof(char*) * (ReadPortion));
	gNamsToPredict.StrCol2 = (char**)malloc(sizeof(char*) * (ReadPortion));
	
	if (!(gNamsToPredict.StrCol1) || !(gNamsToPredict.StrCol2)) printf("Failed to allocate 1488\n");
	
FPrintCOut("result.log","inside main cycle 1695\n");	
	
	amount_str7=ReadTwoColFromFileMtoN(predictPairsFile,gNamsToPredict.StrCol1,gNamsToPredict.StrCol2,&len_str_max7,&start,&end,sp1Espr,sp2Espr);
	if (amount_str7 <= 0)
	{
		//smth go wrong read from file zero strings
		FPrintCOut("result.log","smth go wrong ReadTwoColFromFileMtoN read from file amount_str7=0\n");
		continue;
	}
	if (ReadPortion > amount_str7)
	{
		gNamsToPredict.StrCol1 = (char**)realloc(gNamsToPredict.StrCol1, sizeof(char*) * (amount_str7));
		gNamsToPredict.StrCol2 = (char**)realloc(gNamsToPredict.StrCol2, sizeof(char*) * (amount_str7));
		FPrintCOut("result.log","1729 !!! Realocation!!!\n");
	}
	gNamsToPredict.size=amount_str7;
	
	printf("Keys reading are finished...\n");
	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	//check if there is garbage char like "\n" in the end of sp1Stopbec4
	//in future check for all possible garbage, such as spaces, \r etc
	//delete last /n
	for (int a=0;a<amount_str7;a++)
	{
		DeleteLastN(gNamsToPredict.StrCol2[a]);
		DeleteLastN(gNamsToPredict.StrCol1[a]);
	}
	
	//gNamsToPredict has two columns of names to predict wiht only pairs presence in sp1 and sp2 consistently
	printf("FillTrainSet forming is starting...\n");
	printf("TrainSetArray address of a = %p\n", (pTrainSetArr=MkArrayTrainSetFromIntMtrx(&gNamsToPredict, sp1Espr, sp2Espr, "1", pTrainSetArr)));
	printf("Set forming is Finished...\n");
	
		//convert to DMatrix
		printf("Start converting to DMatrix...\n");

		memset(&h_train,0,sizeof(DMatrixHandle));
		safe_xgboost(XGDMatrixCreateFromMat((float *)pTrainSetArr->IntMtrx[0],pTrainSetArr->raws,pTrainSetArr->cols,0,&h_train));
		printf("\ncreating DMatrixTrain\n");
		
		// load the labels
		safe_xgboost(XGDMatrixSetFloatInfo(h_train, "label", (float*)pTrainSetArr->Labels, pTrainSetArr->raws));
		BoosterHandle h_booster;
		DMatrixHandle eval_dmats[1] = {h_train};
		safe_xgboost(XGBoosterCreate(eval_dmats, 1, &h_booster));
		
		
		for (int iter=MinIter;iter <= MaxIter; iter++)
	{
		FPrintCOut("result.log","inside main cycle incice minIter_maxitet 1778\n");			
		asprintf(&modelDir, "iter_%d/results/training/expression",iter);
		printf("Model Dir %s\n",modelDir);
		asprintf(&PredDir, "iter_%d/results/predictions",iter);
		printf("Pred Dir %s\n",PredDir);
		
		//check if exist and create
		if (stat(PredDir, &st) == -1) 
			mkdir(PredDir, 0700);		
		
		asprintf(&modelNameWithDir,"%s/folds_all/model/model_1_1_1_1000.test",modelDir); 
		printf("%s\n",modelNameWithDir);
		safe_xgboost(XGBoosterLoadModel(h_booster,modelNameWithDir));	
		safe_xgboost(XGBoosterSetParam(h_booster,"nthread", sNthread));
		//safe_xgboost(XGBoosterSetParam(h_booster,"gpu_id", "0"));
		//safe_xgboost(XGBoosterSetParam(h_booster,"tree_method", "gpu_hist"));
		printf("%s threads\n",sNthread);
		// predict
		bst_ulong out_len = 0;
		const float* out_result = NULL;
		
FPrintCOut("result.log","inside main cycle incice minIter_maxitet 1805\n");					

		safe_xgboost(XGBoosterPredict(h_booster, h_train, 0, 0,&out_len, &out_result));

FPrintCOut("result.log","inside main cycle incice minIter_maxitet 1815\n");						
		  
		char* predictNameWithDir;
		asprintf(&predictNameWithDir,"%s/expression.other.predictions",PredDir); 
		printf("Predition %s\n",predictNameWithDir);
		FILE* mf=fopen(predictNameWithDir,"a");
		if (mf == NULL) return 0;
			//else printf ("open for write done\n");
		for (bst_ulong iii = 0; iii < out_len; ++iii) 
		{
			fprintf(mf,"%s\t%s\t%1.3f\n",gNamsToPredict.StrCol1[iii],gNamsToPredict.StrCol2[iii],out_result[iii]);
			//printf("%1.4f ", out_result[iii]);
		}
		fclose(mf);
		free(modelDir);
		free(PredDir);
		free(modelNameWithDir);
		free(predictNameWithDir);
	}

FPrintCOut("result.log","again outside main cycle incice minIter_maxitet 1841\n");						
	//free h_booster and h_train
		//clean from the previous iteration
	FreeArraySet(pTrainSetArr);
	FreeTStCol(&gNamsToPredict);
	memset(pTrainSetArr,0,sizeof(ArraySet));
	
	safe_xgboost(XGBoosterFree(h_booster));	
	safe_xgboost(XGDMatrixFree(h_train));

	FPrintCOut("result.log","Reading cycle iteration is done 1857\n");

	start=end+1;
	end=start+ReadPortion-1;
	//end while cycle
}

//free memory
	FreeStrColIntMtrx(sp2Espr);
	FreeStrColIntMtrx(sp1Espr);
FPrintCOut("result.log","1744\n");			
	printf("Free memory start...\n");
	free(species1File);
	free(species2File);
	free(predictPairsFile);
	free(sNthread);

	  return 0;
}
