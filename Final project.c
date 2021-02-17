/*
Mercurio Alessandro
Codice persona 10614632
Matricola 907992

Final project of algorithms and data structures

Politecnico di Milano
2019/2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1025
#define MAXMEMUSAGE 1500000000

//Container for all text strings in input
char *StringContainer;

//Base struct to contain the text as it follows to be updated
typedef char *String;
typedef struct {
	unsigned int Length;
	String *LinesVector;
} ArrayOfLines;

//Global megastructure to contain all configurations
unsigned int Total;
ArrayOfLines *ArrayOfArrays=NULL;
ArrayOfLines *Current=NULL;
ArrayOfLines *Previous=NULL;

//Emergency structs to operate well at memory limit
typedef struct El {
	struct El* Prev;
	struct El* Next;
	char* Text;
} ListEl;
typedef ListEl* List;

typedef struct {
	unsigned int Allocated;
	unsigned int Used;
	List* Vector;
} EmergencyArray;
EmergencyArray Lines;

/////////////////////////////////////////////////////////////////////////

char read_line[MAX];					//temporary string for text reading
char *tmp; 								//temporary string for text memory allocation
unsigned int CurrentPosition=0;			//current index in the array of arrays
unsigned int FutureCurrent=0;			//jump position when undo or redo
double UndoRedoCount;					//counter to calculate how many undo/redo to do
unsigned int MemoryTracker=0;			//variable to keep track of memory usage and prevent over-saving

//FUNCTIONS
void Change(unsigned int ind1, unsigned int ind2);
void Delete(unsigned int ind1, unsigned int ind2);
void Print(unsigned int ind1, unsigned int ind2);
void EmergencyChange(unsigned int ind1, unsigned int ind2); //memory limit function
void EmergencyPrint(unsigned int ind1, unsigned int ind2);	//memory limit function

//////////////////////////////////////////////////////////////////////////

//MAIN CODE
int main() //reading commands and calling functions to execute each
{
	//useful indexes
	unsigned int ind1, ind2, index, subindex;
	
	List Temp;
	Lines.Allocated=0;
	Lines.Used=0;
	Lines.Vector=NULL;
	
	//useful char type variables
	char operation;
	char *end="q\n", *p;
	char read_command[MAX];
	
	Total=0;
	StringContainer=malloc(1000000000*sizeof(char)); //super-malloc for all text strings
	
	//////////////////////////////////////////////////////////////////////
	
	fgets(read_command, MAX, stdin);
	while(0!=strcmp(read_command,end)) //execute for each command until you read the q command
	{
		p=read_command;
		ind1=strtol(p, &p, 10); //first index to operate with
		if(*p==',')
		{
			p++;
			ind2=strtol(p, &p, 10); //second index to operate with
		}
		operation=*p; //operation to execute
		
		///////////////////////////////////////////////////////////////////////////
	
		//CHANGE
		if(operation=='c')
		{
			if(MemoryTracker<MAXMEMUSAGE) //memory limit is not broken 
			{
				if(UndoRedoCount!=0) //you need to do some undo or redo
				{
					CurrentPosition=FutureCurrent;
					UndoRedoCount=0;
				}
			
				//Creating new configuration
				if(CurrentPosition==Total)
				{
					Total++;
					ArrayOfArrays=realloc(ArrayOfArrays, Total*sizeof(ArrayOfLines));
					MemoryTracker=MemoryTracker+Total*sizeof(ArrayOfLines);
				}
				else if(CurrentPosition<Total)
				{
					Total=CurrentPosition;
					Total++;
				}
			
				CurrentPosition++;
				FutureCurrent=CurrentPosition;
			
				if(CurrentPosition>=1)
				{
					Current=&(ArrayOfArrays[CurrentPosition-1]);
				}
				else
				{
					Current=NULL;
				}
				if(CurrentPosition>=2)
				{
					Previous=&(ArrayOfArrays[CurrentPosition-2]);
				}
				else
				{
					Previous=NULL;
				}
			
				Change(ind1,ind2);
			}
			
			else //reached memory limit - need to use a lighter structure to continue writing
			{
				if(Lines.Allocated==0)
				{
					Lines.Vector=malloc(ArrayOfArrays[Total-1].Length*sizeof(List));
					for(index=0; index<ArrayOfArrays[Total-1].Length; index++)
					{
						Temp=malloc(sizeof(ListEl));
						Temp->Prev=NULL;
						Temp->Next=NULL;
						Temp->Text = ArrayOfArrays[Total-1].LinesVector[index];
						Lines.Vector[index]=Temp;
					}
					Lines.Allocated=ArrayOfArrays[Total-1].Length;
					Lines.Used=Lines.Allocated;
				}
				EmergencyChange(ind1, ind2);
			}
		}
		
		///////////////////////////////////////////////////////////////////
		
		//DELETE
		else if(operation=='d')
		{	
			MemoryTracker=0;
			
			if(UndoRedoCount!=0) //you need to do some undo or redo
			{
				CurrentPosition=FutureCurrent;
				UndoRedoCount=0;
			}
			
			//Creating new configuration
			if(CurrentPosition==Total)
			{
				Total++;
				ArrayOfArrays=realloc(ArrayOfArrays, Total*sizeof(ArrayOfLines));
			}
			else if(CurrentPosition<Total)
			{
				Total=CurrentPosition;
				Total++;
			}
	
			CurrentPosition++;
			FutureCurrent=CurrentPosition;
			
			if(CurrentPosition>=1)
			{
				Current=&(ArrayOfArrays[CurrentPosition-1]);
			}
			else
			{
				Current=NULL;
			}
			if(CurrentPosition>=2)
			{
				Previous=&(ArrayOfArrays[CurrentPosition-2]);
				
				if((Previous->Length>0)&&(ind1<=Previous->Length)&&(ind2!=0))
				{
					if(ind1==0) //zero line cannot be deleted
					{
						ind1=1;
					}
					if(ind2>Previous->Length) //not existing lines cannot be deleted
					{	
						ind2=Previous->Length;
					}
					Delete(ind1,ind2);
				}
				else //non-valid delete
				{
					memcpy(Current, Previous, sizeof(ArrayOfLines));
				}
			}
			else //this delete is executed without an existing text
			{
				Current->Length=0;
				Current->LinesVector=NULL;
			}
		}
		
		/////////////////////////////////////////////////////////////////////
		
		//PRINT
		else if(operation=='p')
		{
			if(MemoryTracker<MAXMEMUSAGE) //memory limit is not broken 
			{		
				if(UndoRedoCount!=0) //you need to do some redo
				{
					CurrentPosition=FutureCurrent;
					UndoRedoCount=0;
				}
				Print(ind1,ind2);
			}
			
			else
			{
				EmergencyPrint(ind1, ind2);
			}
		}
		
		//UNDO
		else if(operation=='u') //counting the undo to do before an operation
		{
			MemoryTracker=0;
			
			if(ind1>FutureCurrent)
			{
				ind1=FutureCurrent;
			}
			UndoRedoCount=UndoRedoCount-ind1;
			FutureCurrent=CurrentPosition+UndoRedoCount;
		}
		
		//REDO
		else if(operation=='r') //counting the redo to do befora an operation
		{
			MemoryTracker=0;
			
			if(ind1>(Total-FutureCurrent))
			{
				ind1=Total-FutureCurrent;
			}
			UndoRedoCount=UndoRedoCount+ind1;
			FutureCurrent=CurrentPosition+UndoRedoCount;
		}
		
		////////////////////////////////////////////////////////////////////
		
		fgets(read_command, MAX, stdin); //waiting new command
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void Change(unsigned int ind1, unsigned int ind2)
{
	unsigned int index, length, NewLength;
	
	if(Previous!=NULL) //there is a previous text that is being changed
	{
		NewLength=Previous->Length;
		if(ind2>NewLength)
		{
			NewLength=ind2;
		}
		Current->LinesVector=NULL;
		Current->LinesVector=realloc(Current->LinesVector, NewLength*sizeof(String));
		Current->Length=NewLength;
	
		for(index=1; index<=NewLength; index++)
		{
			if((index<ind1)||(index>ind2))
			{
				Current->LinesVector[index-1]=Previous->LinesVector[index-1];
			}
			else
			{
				fgets(read_line, MAX, stdin);
				length=strlen(read_line)+1;
				memcpy(StringContainer, read_line, length); //using the string container for all lines
				Current->LinesVector[index-1]=StringContainer;
				StringContainer=&StringContainer[length];
			}
		}
	}
	else //there is no previous text, the program is starting writing
	{
		Current->Length=ind2;
		Current->LinesVector=NULL;
		Current->LinesVector=realloc(Current->LinesVector, ind2*sizeof(String));
		for(index=1; index<=ind2; index++)
		{
			fgets(read_line, MAX, stdin);
			length=strlen(read_line)+1;
			memcpy(StringContainer, read_line, length); //using the string container for all lines
			Current->LinesVector[index-1]=StringContainer;
			StringContainer=&StringContainer[length];
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void Print(unsigned int ind1, unsigned int ind2)
{
	unsigned int index;
	
	if(CurrentPosition>0)
	{
		Current=&(ArrayOfArrays[CurrentPosition-1]);
		if(CurrentPosition>=2)
		{
			Previous=&(ArrayOfArrays[CurrentPosition-2]);
		}
		else
		{
		Previous=NULL;
		}
		
		if(ind1==0) //the zero line never exists
		{
			printf(".\n");
			ind1=1;
		}
		if(ind2!=0)
		{
			for(index=ind1; (index<=ind2)&&(index<=Current->Length); index++) //printing existing lines
			{
				printf("%s",Current->LinesVector[index-1]);
			}
			if(Current->Length<ind2)
			{
				for(index=Current->Length+1; index<=ind2; index++) //printing a point for each invalid line
				{	
					if(index>=ind1)
					{
						printf(".\n");
					}
				}
			}
		}
	}
	else
	{
		for(index=ind1; index<=ind2; index++) //the text is empty
		{	
			printf(".\n");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void Delete(unsigned int ind1, unsigned int ind2)
{
	unsigned int size, index, newindex, length, NewLength;
	
	size=ind2-ind1+1;
	if(Previous!=NULL) //there is a previous text to delete from
	{
		if(ind2>Previous->Length)
		{
			ind2=Previous->Length;
		}
		NewLength=Previous->Length-size;
		Current->LinesVector=NULL;
		Current->LinesVector=realloc(Current->LinesVector, NewLength*sizeof(String));
		Current->Length=NewLength;
		
		newindex=ind1;
		for(index=1; index<=Previous->Length; index++)
		{
			if(index<ind1)
			{
				Current->LinesVector[index-1]=Previous->LinesVector[index-1];
			}
			else if(index>ind2)
			{
				Current->LinesVector[newindex-1]=Previous->LinesVector[index-1];
				newindex++;
			}
		}
	}
	else //no delete can be performed on an empty text
	{
		Total=0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void EmergencyChange(unsigned int ind1, unsigned int ind2) //changing lines with a lighter structure
{
	unsigned int index, length;
	List* Temporary;
	List Temp;
	
	if(Lines.Allocated<ind2) //allocating new space
	{
		Temporary=realloc(Lines.Vector, ind2*sizeof(List));
		if(Temporary!=NULL)
		{
			Lines.Vector=Temporary;
			Lines.Allocated=ind2;
		}	
	}
	
	for(index=ind1; index<=ind2; index++)
	{
		if(index>Lines.Used)
		{
			Temp=malloc(sizeof(ListEl));
			Temp->Prev=NULL;
			Temp->Next=NULL;
			fgets(read_line, MAX+1, stdin);
			length=strlen(read_line)+1;
			tmp=malloc(sizeof(char)*length);
			memcpy(tmp, read_line, length);
			Temp->Text = tmp;
			Lines.Vector[index-1]=Temp;
			Lines.Used=index;
		}
		else //chainging already existing lines
		{
			Temp=malloc(sizeof(ListEl));
			Temp->Prev=Lines.Vector[index-1];
			Temp->Next=NULL;
			fgets(read_line, MAX+1, stdin);
			length=strlen(read_line)+1;
			tmp=malloc(sizeof(char)*length);
			memcpy(tmp, read_line, length);
			Temp->Text = tmp;
			Lines.Vector[index-1]->Next=Temp;
			Lines.Vector[index-1]=Temp;
		}
	}
}

////////////////////////////////////////////////////////////////////////

void EmergencyPrint(unsigned int ind1, unsigned int ind2) //printing lines from the lighter structure
{
	unsigned int index;
	if(Lines.Vector!=NULL)
	{
		if(ind1==0)
		{
			printf(".\n");
			ind1=1;
		}
		if(ind2!=0)
		{
			for(index=ind1; (index<=ind2)&&(index<=Lines.Used); index++)
			{
				printf("%s",Lines.Vector[index-1]->Text);
			}
			if(Lines.Used<ind2)
			{
				for(index=Lines.Used+1; index<=ind2; index++)
				{
					if(index>=ind1)
					{
						printf(".\n");
					}
				}
			}
		}
	}
	else
	{
		for(index=ind1; index<=ind2; index++)
		{
			printf(".\n");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

