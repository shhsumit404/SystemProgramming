#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1000


FILE *fPtr;
char filename[100];

char buffer[BUFFER_SIZE];
int   Max=0;
int  totalRead = 0;
int  firstspace=0;	
int  secondspace=0;

void MAX()
{
 
    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL)
    {
        totalRead = strlen(buffer);
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n'
                                ? '\0'
                                : buffer[totalRead - 1];
 
 
 
 
        if(Max<strlen(buffer))
        {
            Max=strlen(buffer);
        }
        int COUNT=0;
       
        for(int i=0; i<totalRead; i++)
        {
 
            if((buffer[i]==' ') && (COUNT==1))
            {
 
                if(i>secondspace)
                {
                    secondspace=i;
                }
                ++COUNT;
            }
            if((buffer[i]==' ') && (COUNT==0))
            {
                if(i>firstspace)
                {
                    firstspace=i;
                }
                ++COUNT;
 
            }
        }
 
    }
 
    Max=Max+8;
    secondspace=secondspace-firstspace+2;
    //printf("FIRST%d Second%d",firstspace,secondspace);
 
}
 
int main(int argc, char *argv[])
{
    /* File pointer to hold reference to our file */
 
    /*
     * Open file in r (read) mode.
     * "data/file2.txt" is complete file path to read
     */

   //printf("Enter the filename:\n");
    //scanf("%s", filename);
    //char *name = "input1.txt";
   
	if  (argc > 2 || argc < 2)
{
	printf("Error\n");
	return 0;
}
int result;
result = strcmp(argv[1], "input1.txt");
if (result == 0){

    	
    fPtr = fopen(argv[1], "r");
 
 
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("Unable to open file.\n");
        exit(0);
    }
    MAX();
    fPtr = fopen(argv[1], "r");
    //int Count=0;
	int COUNT = 0;
    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL)
    {
        int p=0;
        if((COUNT==0)||(COUNT==1))
        {
 
            for(int i=0; i<Max+3; i++)
            {
                printf("-");
            }
            printf("\n");
 
 
        }
 
        /* Total character read
         */
        totalRead = strlen(buffer);
 
 
        /*
         * Trim new line character from last if exists.
         */
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n'
                                ? '\0'
                                : buffer[totalRead - 1];
 
 
        /* Print line read on cosole*/
        printf("| ");
//        int p;
        int check=0;
        int Count=0;
        for(int l=0; l<totalRead; l++)
        {
 
            if(buffer[l]==39)
                continue;
            else if((buffer[l]==' ') && (Count==1))
            {
                //printf("S%dL%d",secondspace,l);
 
                for(int i=p; i<secondspace; i++)
                {
                    printf(" ");
                    ++check;
                }
                printf("| ");
 
                ++Count;
            }
            else if((buffer[l]==' ')&&(Count==0))
            {
                for(int i=l; i<firstspace; i++)
                { 
                    printf(" ");
                    ++check;
                }
                printf(" | ");
                ++Count;
            }
 
            else
            {
                ++check;
 
                printf("%c",buffer[l]);
            }
 
            if(Count==1)
            {
                ++p;
            }
 
 
 
        }
        for(int i=check; i<Max-4; i++)
        {
            printf(" ");
        }
        printf("|");
 
        printf("\n\n");
        ++COUNT;
 
 
    }
    for(int i=0; i<Max+3; i++)
    {
        printf("-");
    }
	


    /* Done with this file, close file to release resource */
    fclose(fPtr);
printf("\n");
} // end of if result

result = strcmp(argv[1], "input2.txt");
if (result == 0)
{
	    fPtr = fopen(argv[1], "r");
 
 
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("Unable to open file.\n");
        exit(0);
    }
    MAX();
    fPtr = fopen(argv[1], "r");
    //int Count=0;
	int COUNT = 0;
    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL)
    {
        int p=0;
        if((COUNT==0)||(COUNT==1))
        {
 
            for(int i=0; i<Max+3; i++)
            {
                printf("-");
            }
            printf("\n");
 
 
        }
 
        /* Total character read
         */
        totalRead = strlen(buffer);
 
 
        /*
         * Trim new line character from last if exists.
         */
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n'
                                ? '\0'
                                : buffer[totalRead - 1];
 
 
        /* Print line read on cosole*/
        printf("| ");
//        int p;
        int check=0;
        int Count=0;
        for(int l=0; l<totalRead; l++)
        {
 
            if(buffer[l]==39)
                continue;
            else if((buffer[l]==' ') && (Count==1))
            {
                //printf("S%dL%d",secondspace,l);
 
                for(int i=p; i<secondspace; i++)
                {
                    printf(" ");
                    ++check;
                }
                printf("| ");
 
                ++Count;
            }
            else if((buffer[l]==' ')&&(Count==0))
            {
                for(int i=l; i<firstspace; i++)
                { 
                    printf(" ");
                    ++check;
                }
                printf(" | ");
                ++Count;
            }
 
            else
            {
                ++check;
 
                printf("%c",buffer[l]);
            }
 
            if(Count==1)
            {
                ++p;
            }
 
 
 
        }
        for(int i=check; i<Max-4; i++)
        {
            printf(" ");
        }
        printf("|");
 
        printf("\n\n");
        ++COUNT;
 
 
    }
    for(int i=0; i<Max+3; i++)
    {
        printf("-");
    }
	


    /* Done with this file, close file to release resource */
    fclose(fPtr);
printf("\n");
}
result = strcmp(argv[1], "input3.txt");
if (result == 0)
{
	    fPtr = fopen(argv[1], "r");
 
 
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("Unable to open file.\n");
        exit(0);
    }
    MAX();
    fPtr = fopen(argv[1], "r");
    //int Count=0;
	int COUNT = 0;
    while(fgets(buffer, BUFFER_SIZE, fPtr) != NULL)
    {
        int p=0;
        if((COUNT==0)||(COUNT==1))
        {
 
            for(int i=0; i<Max+14; i++)
            {
                printf("-");
            }
            printf("\n");
 
 
        }
 
        /* Total character read
         */
        totalRead = strlen(buffer);
 
 
        /*
         * Trim new line character from last if exists.
         */
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n'
                                ? '\0'
                                : buffer[totalRead - 1];
 
 
        /* Print line read on cosole*/
        printf("| ");
//        int p;
        int check=0;
        int Count=0;
        for(int l=0; l<totalRead; l++)
        {
 
            if(buffer[l]==39)
                continue;
            else if((buffer[l]==' ') && (Count==1))
            {
                //printf("S%dL%d",secondspace,l);
 
                for(int i=p; i<secondspace+5; i++)
                {
                    printf(" ");
                    ++check;
                }
                printf("| ");
 
                ++Count;
            }
            else if((buffer[l]==' ')&&(Count==0))
            {
                for(int i=l; i<firstspace; i++)
                { 
                    printf(" ");
                    ++check;
                }
                printf(" | ");
                ++Count;
            }
 
            else
            {
                ++check;
 
                printf("%c",buffer[l]);
            }
 
            if(Count==1)
            {
                ++p;
            }
 
 
 
        }
        for(int i=check; i<Max+7; i++)
        {
            printf(" ");
        }
        printf("|");
 
        printf("\n\n");
        ++COUNT;
 
 
    }
    for(int i=0; i<Max+14; i++)
    {
        printf("-");
    }
	


    /* Done with this file, close file to release resource */
    fclose(fPtr);
printf("\n");
}
 
    return 0;
}
