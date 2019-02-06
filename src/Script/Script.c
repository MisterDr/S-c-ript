/*
 *  Script engine
 *  Version 1.0
 *  Created by Djenad Razic 
 */

// Comment this to produce IIS CGI, default is IIS CGI module
// #define APACHEMOD
// #define NGINXMOD
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bstrlib.h>
#include <sglib.h>

#include <libtcc.h>

// Include c files into the compiler
#include <bstrlib.c>

// TODO: Create apache mod
#ifdef APACHEMOD

#endif

// TODO: Create NGINX mod

#define HEADER 1
#define MAIN 2
#define CODE 3
#define HTML 4

#define STR_LBRACKET -2
#define STR_FOUNDBRACKET 0

#define FALSE 0
#define TRUE 1

typedef struct page{
	bstring val;
	int flag;  //Can be HEADER, MAIN, CODE, HTML
	struct page * next;	
	struct page * prev;		
} page;
	
typedef struct brck {
	int pos;
	int num;	
} brck;
	
page *ppage = NULL;
static bstring codeToCompile;

#define PAGE_COMPARATOR(e1, e2) (bstricmp(e1->val, e2->val))

SGLIB_DEFINE_DL_LIST_PROTOTYPES(page, PAGE_COMPARATOR, prev, next);
SGLIB_DEFINE_DL_LIST_FUNCTIONS(page, PAGE_COMPARATOR, prev, next);

/***********************************
*   Printify string                *
************************************/
bstring printify(bstring b, int flag)
{
	if (flag == HTML)
		bfindreplace(b, bfromcstr("\""), bfromcstr("\\\""), 0);
	
	bstring ret = bfromcstr("printf(\"");
	bconcat(ret, b);
	bconcat(ret, bfromcstr("\");"));
	return ret;
}

/*********************************************************
*   Find pre header macro starting with TCC_             *
**********************************************************/
int findPreHeaderMacro(bstring b, int pos)
{
	int tcc = binstr(b, pos, bfromcstr("TCC_"));	
	if (tcc == BSTR_ERR)
		return BSTR_ERR;
	
	pos = tcc + 4;	
	for (;pos < b->slen; pos++)
	{		
		if (!(((((int)b->data[pos]) > 64) && (((int)b->data[pos]) < 91)) || ((int)b->data[pos]) == 95))
			return pos;	
	}
}

/***********************************
*   Find Main function             *
************************************/
int findMain(bstring b, bstring header)
{
	//TODO: Find bracket if not in existing block and handle pre headers
	int mainExist = binstr(b, 0, bfromcstr("main("));
	if (mainExist == BSTR_ERR)
		return BSTR_ERR;
	
	if (header != NULL)
	{
		int posToInsert = binstr(b, mainExist, bfromcstr("{")) + 1;
		int preHeader = findPreHeaderMacro(b, posToInsert);
				
		if (preHeader != BSTR_ERR)
		{
			posToInsert = preHeader + 1;
			
			//Insert new line to avoid false string search
			binsert(b, posToInsert, bfromcstr("\n"),' ');
			posToInsert = posToInsert + 1;
		}
		
		binsert(b, posToInsert, header,' ');
	}
	
	return mainExist;	
}

/***********************************
*   Count brackets on given string *
************************************/
int countBrackets(bstring b, brck *brcount, int pos)
{				
	//TODO: Find matching braces and handle braces in the string
	for (; pos < b->slen; pos++)
	{
		if (b->data[pos] == '{')
		{
			brcount->num++;
		} 
		
		if (b->data[pos] == '}')
		{
			brcount->num--;
		}		

		//Found closing bracket
		if (brcount->num == 0)
		{
		  brcount->pos = pos;
		  return STR_FOUNDBRACKET;
		}
	}
	return STR_LBRACKET;
}

/***********************************
*   Append end of file to the main *
************************************/
void appendEndToMain(bstring b)
{
	page *l;
	int opening = -1, mainStarted = FALSE, state = 0, pos = 0;
	brck brcount;	
	brcount.num = 0;
	
	for(l=sglib_page_get_first(ppage); l!=NULL; l=l->next)
	{
		if (l->flag == MAIN || ((mainStarted == TRUE) && l->flag == CODE))
		{
			//Find main and following bracket after main if exist
			if ((opening == -1) && (l->flag == MAIN))
			{
				int mainExist = binstr(l->val, 0, bfromcstr("main("));
				mainExist = binstr(l->val, mainExist, bfromcstr("{"));
				if (mainExist != BSTR_ERR)
					pos = mainExist;
			}
			
			state = countBrackets(l->val, &brcount, pos);
			
			if (state == STR_LBRACKET)
				opening = 1;
			else if (state == STR_FOUNDBRACKET)
				binsert(l->val,  brcount.pos, b,' ');
									
			pos = 0;
			mainStarted = TRUE;
		}
	}
}

/***********************************
*   Add item to the page           *
************************************/
void addPageItem(bstring itm, int flag)
{
	page *l;
	l = malloc(sizeof(page));
	l->val = itm;
	l->flag = flag;	
	sglib_page_add(&ppage, l);
}

/***********************************
*   Parse HTML and printify it     *
************************************/
int parseHtml(bstring fileContent) 
{
	struct sglib_page_iterator  it;
	page *l;	
	bstring header, body;		
	int currentPos, oldPos, codeType, bodyEnd = 0;	
	int mainFound = 0;
	int headerPos = binstr(fileContent, 0, bfromcstr("<$"));
		
	//Add empty element to list to behave correctly O_o
	addPageItem(bfromcstr(""), HTML);
	
	if (headerPos == BSTR_ERR)
		return 0;
	
	//Generate header if exists 
	if (headerPos !=0)
		header = printify(bmidstr(fileContent, 0,headerPos), HTML);
	else 	
		header = NULL;
	
	currentPos = binstr(fileContent, headerPos, bfromcstr("$>"));
	bodyEnd = currentPos - (headerPos + 2);
	
	body = bmidstr(fileContent, headerPos + 2, bodyEnd);
	
	if (findMain(body, header) != BSTR_ERR)
	{
		mainFound = 1;
		addPageItem(body, MAIN);
	} 
	else 
	{			
		addPageItem(body, CODE);	
	}	

	oldPos = currentPos+2;
	
	for (currentPos = binstr(fileContent, currentPos+2, bfromcstr("<$")); currentPos != BSTR_ERR; currentPos = binstr(fileContent, currentPos+2, bfromcstr("<$")))	
	{		
		if (oldPos != currentPos) 
		{
			//TODO: Check for size of html
			bstring htmlPart = printify(bmidstr(fileContent, oldPos, currentPos - oldPos), HTML);
			addPageItem(htmlPart, HTML);
			
			oldPos = currentPos+2;
			currentPos = binstr(fileContent, currentPos, bfromcstr("$>"));			
			
			bstring codePart = bmidstr(fileContent, oldPos, currentPos - oldPos);
									
			oldPos = currentPos+2;
			
			if ((mainFound == 0) && (findMain(codePart, header) != BSTR_ERR))
			{
				mainFound = 1;
				addPageItem(codePart, MAIN);						
			}			
			else 
			{
				addPageItem(codePart, CODE);						
			}	
		} 
		else 
		{
			currentPos = binstr(fileContent, currentPos, bfromcstr("$>"));
			bstring codePart = bmidstr(fileContent, oldPos, currentPos - oldPos);
					
			if ((mainFound == 0) && (findMain(codePart, header) != BSTR_ERR))
			{
				mainFound = 1;
				addPageItem(codePart, MAIN);	
			}			
			else 
			{
				addPageItem(codePart, CODE);						
			}	
			
			oldPos = currentPos+2;
		}	
	}
	
	//Find and append end of the html file at the main function
	if (oldPos != blength(fileContent)) 
	{
		bstring htmlPart = printify(bmidstr(fileContent, oldPos, blength(fileContent) - oldPos), HTML);
		appendEndToMain(htmlPart);
	}
	
	//Must init bstring otherwise crashes
	codeToCompile = bfromcstr("");
	
	//Construct string to compile
	for(l=sglib_page_get_first(ppage); l!=NULL; l=l->next)
		bconcat(codeToCompile,l->val);
	
	for(l=sglib_page_it_init(&it,ppage); l!=NULL; l=sglib_page_it_next(&it)) {
		bdestroy(l->val);
		free(l);
	}
	
	bdestroy(header);
	bdestroy(body);
}

/*******************************************
*   Print error                            *      
*   (TODO: !duh, handle messaging better)  *
********************************************/
void printError(char *str)
{
	printf("%s","HTTP/1.0 200 OK\nContent-Type:text/html;charset=iso-8859-1\n\n");
	printf("%s", str);
}

/***********************************
*   Get file size                  *
************************************/
int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //Go back to where we were
    return sz;
}

/******************************************************
*   Read file and allocate mem according to its size  *
*******************************************************/
bstring readFile(FILE *fp) 
{
  int n, np;
  char *b;
  int r;
  bstring file;  
  int sz = fsize(fp);
 
  b = calloc(sizeof(char), sz+20);
  fread(b, 1, sz, fp);
  
  file = bfromcstr(b);  
  
  return file;
}

/******************************************************
*   Write debug data to the file                      *
*******************************************************/
void writeDebugData(char *file, bstring *data)
{
	FILE *f = fopen(file, "w");
	fprintf(f, "%s",codeToCompile->data);
	fclose(f);
}


/******************************************************
*   Read file and process it                          *
*******************************************************/
int main(int argc, char **argv)
{
	char *lenstr;
	int ch;
	char *fileContent;
		
	//Get file to process
	lenstr = getenv("PATH_TRANSLATED");
	
	//DEBUG VAR
	//char lenstr[] = "E:\\GeckoHtml\\tcc\\simpleMongoConn.tcc";
	
	FILE *f = fopen(lenstr, "rb");
	
	if (f == NULL)
	{
		printError("Cannot open file!\n\n");
		return 0;
	}
	
	// Read and parse TCC file
	bstring b = readFile(f);	
	fclose(f);	
	
	//Parse HTML and process code
	parseHtml(b);	
	
	//DEBUG text to compile
	//printf("%s", codeToCompile->data);
	//writeDebugData("debug.txt", &codeToCompile);
	//return;
	
	//Compile processed file
    TCCState *s;
    void (*runmain)(int,char**);
    void *mem;
    int size;

    s = tcc_new();
    if (!s) {
        fprintf(stderr, "Could not create TCC state\n");
        exit(1);
    }
	
    //If tcclib.h and libtcc1.a are not installed, where can we find them?
    if (argc == 2 && !memcmp(argv[1], "lib_path=",9))
        tcc_set_lib_path(s, argv[1]+9);

#if WIN32_LEAN_AND_MEAN
	tcc_set_lib_path_w32(s);
#endif

    //MUST BE CALLED before any compilation
    //tcc_output_file(s, "testOutput.exe");
	//tcc_set_output_type(s, TCC_OUTPUT_EXE);
	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
	
	//Well, we are little modern no?
	//tcc_add_library(s,"advapi32");
	tcc_add_library(s,"WS2_32");
    
	//Hush, no need to blush TODO: Handle better compiling
	if (tcc_compile_string(s, codeToCompile->data) == -1)
        return 1;	
	
    //Get needed size of the code
    size = tcc_relocate(s, NULL);
    if (size == -1)
        return 1;
	
	//Allocate memory and copy the code into it
    mem = malloc(size);
	
	//And get the TCC memory too
    tcc_relocate(s, mem);
	
    //Get entry symbol
    runmain = tcc_get_symbol(s, "main");
    if (!runmain)
        return 1;

    //Run the code
    runmain(argc, argv);

    //Delete the state
	//Cannot clean before running the program because of it will unload dlls, it obstruct winsock especially
    tcc_delete(s);
	
	//See ya
    free(mem);
	bdestroy(codeToCompile);
	
    return 0;
}
