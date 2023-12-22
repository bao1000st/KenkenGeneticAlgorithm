#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#define MAX_CANDIDATES 1050
#define MAX_LENGTH 81
#define MAX_NUMDIGITS 9
int NumDigits=9;



float float_rand( float min, float max )
{
    float scale = (float)rand() / (float) RAND_MAX; /* [0, 1.0] */
    scale = (float)rand() / (float) RAND_MAX;
    return min + scale * ( max - min );      /* [min, max] */
}

int int_rand(int min,int max)
{
	int r=(rand() %(max - min + 1)) + min;
	return r;
}

int min(int a,int b)
{
	if (a<b) return a;
	return b;
}

int max(int a,int b)
{
	if (a>b) return a;
	return b;
}


double gaussian_rand(double mu, double sigma)
{
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;
 
  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }
 
  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);
 
  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;
 
  call = !call;
 
  return (mu + sigma * (double) X1);
}

// Console Handling-----------------------------------------------------
void setcolor(int x)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),x);
}
void gotoxy(int x, int y)
{
    static HANDLE h = NULL;  
    if(!h)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = { x, y };  
    SetConsoleCursorPosition(h,c);
}

int formatX=32,formatY=10;//toa do de dinh dang
//-------------------------------------------------------------------------


// Cord (Toa do diem) ---------------------------------------------------------------

typedef struct{
	int x,y;
}Coord;

int Indexof(Coord C)
{
	return (C.x*NumDigits + C.y);
}

Coord Coordof(int vertex)
{
	Coord C;
	C.x=vertex/NumDigits;
	C.y=vertex%NumDigits;
	return C;
}
//------------------------------------------------------------------------------


// Boxlist  (DS Khoi trong kenken)----------------------------------------------------
typedef struct{
	int size;
	Coord* C;
	char Operator;
	int Result;
}Box;

typedef struct{
	int Box_number;
	Box B[MAX_LENGTH];
}BoxList; // rang buoc phep toan
//----------------------------------------------------------------------------------------------

// rang buoc ----------------------------------------------------------------------------
typedef struct{
	int data[MAX_NUMDIGITS*MAX_NUMDIGITS][MAX_NUMDIGITS*MAX_NUMDIGITS];
	int Coord_number; // so luong cac dinh (o) trong bang kenken
}Constrains; //rang buoc dong cot

void initConstrains(Constrains* Constr,int n)
{
	int i,j;
	for (i=0;i<n*n;i++)
	{
		for (j=0;j<n*n;j++)
		{
			Constr->data[i][j]=0;
		}
	}
	Constr->Coord_number=n*n;
}

void addConstrain(Constrains *Constr, Coord source, Coord target)
{
	int x=Indexof(source);
	int y=Indexof(target);
	Constr->data[x][y]=1;			
}

void spreadConstrains(Coord C, Constrains* Constr)
{	
	int i,j;
	// rang buoc cac o cung dong
	for (j=0;j<NumDigits;j++)
	{
		if (j!=C.y)
		{
			Coord Target={C.x,j};
			addConstrain(Constr,C,Target);
		}
	}
	// rang buoc cac o cung cot
	for (i=0;i<NumDigits;i++)
	{
		if (i!=C.x)
		{
			Coord Target={i,C.y};
			addConstrain(Constr,C,Target);
		}
	}
	
}
//-----------------------------------------------------------------------




// KenKen (bang kenken)-----------------------------------------------------
typedef struct{
	int Box_Map[MAX_NUMDIGITS][MAX_NUMDIGITS]; //nham de in mau cho khoi va phuc vu cho rang buoc BoxList
	int Given[MAX_NUMDIGITS][MAX_NUMDIGITS];
	BoxList ListofBoxes; // rang buoc theo khoi
	Constrains Constr; // rang buoc theo cot, dong
}KenKen;


void init_KenKen(KenKen *KK)
{
	int i,j;
	initConstrains(&KK->Constr,NumDigits);
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			KK->Given[i][j]=0;
			Coord C={i,j};
			spreadConstrains(C,&KK->Constr);
		}
	}
}


void read_KenKen(KenKen* KK,char* input)
{
	
	FILE* f=fopen(input,"r");
	
	
	fscanf(f,"%d%d",&NumDigits,&KK->ListofBoxes.Box_number); // doc so luong khoi trong bang
	int i;
	for (i=0;i<KK->ListofBoxes.Box_number;i++)
	{
		fscanf(f,"%d\n",&KK->ListofBoxes.B[i].size); // doc so luong o trong tung khoi
		KK->ListofBoxes.B[i].C=(Coord*)malloc(sizeof(Coord)*KK->ListofBoxes.B[i].size);
		int j;
		for (j=0;j<KK->ListofBoxes.B[i].size;j++)
		{
			fscanf(f,"%d%d\n",&KK->ListofBoxes.B[i].C[j].x,&KK->ListofBoxes.B[i].C[j].y);
			KK->Box_Map[ KK->ListofBoxes.B[i].C[j].x ][ KK->ListofBoxes.B[i].C[j].y ]= i; // ghi chi so khoi len boxmap
		}
		// phai de \n o cac lenh fscanf phia truoc, de ko bi doc ngay space character-------------
		KK->ListofBoxes.B[i].Operator=fgetc(f);
		fscanf(f,"%d",&KK->ListofBoxes.B[i].Result);
	}
	int j;
	init_KenKen(KK);
	
	fclose(f);
}



void print_KenKen_info(KenKen KK)
{
	int i,j;
	printf("KenKen (%dx%d):\n",NumDigits,NumDigits);
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			printf(" %3d ",KK.Box_Map[i][j]);
		}
		printf("\n");
	}
	
	printf("\n%d\n",KK.ListofBoxes.Box_number);
	
	for (i=0;i<KK.ListofBoxes.Box_number;i++)
	{
		for (j=0;j<KK.ListofBoxes.B[i].size;j++)
		{
			printf("(%d,%d) ",KK.ListofBoxes.B[i].C[j].x,KK.ListofBoxes.B[i].C[j].y);
		}
		printf("%c %d\n",KK.ListofBoxes.B[i].Operator, KK.ListofBoxes.B[i].Result);
	}
	
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			printf("%d ",KK.Given[i][j]);
		}
		printf("\n");
	}
	
	for (i=0;i<NumDigits*NumDigits;i++)
	{
		for (j=0;j<NumDigits*NumDigits;j++)
		{
			printf("%d ",KK.Constr.data[i][j]);	
		}	
		printf("\n");
	}
	
}

int isthefirst(Coord C,Box B)
{
	if (B.C[0].x==C.x && B.C[0].y==C.y) return 1;
	return 0;
}


void printCell(int x,int y, Coord C,KenKen KK)
{
	int box_index=KK.Box_Map[C.x][C.y];
	Box B=KK.ListofBoxes.B[ box_index ];
	// dong dau tien
	if (isthefirst(C,B))
	{
		gotoxy(x,y); 
		if ( B.Result/10 != 0 )
		{
			if (B.Result/100!=0) printf("%d%c",B.Result,B.Operator);
			else printf("%d%c%c",B.Result,B.Operator,205);
		}
		else printf("%d%c%c%c",B.Result,B.Operator,205,205);	
		printf("%c",187);
			
	}
	else 
	{	gotoxy(x,y);
		if  (C.x-1<0 || KK.Box_Map[C.x-1][C.y]!=box_index) printf("%c%c%c%c%c",201,205,205,205,187);
		else printf("%c%c%c%c%c",186,196,196,196,186);
	}
	
	
    
    // dong thu hai
    gotoxy(x,y+1);
    if (C.y-1<0 || KK.Box_Map[C.x][C.y-1]!=box_index) printf("%c",186);
    else printf("%c",179);
	printf("  %d",KK.Given[C.x][C.y]);	
    if (C.y+1>NumDigits-1 || KK.Box_Map[C.x][C.y+1]!=box_index) printf("%c",186);
    else printf("%c",179);
	
	
	// dong thu ba
    gotoxy(x,y+2);	
	if (C.x+1>NumDigits-1 || KK.Box_Map[C.x+1][C.y]!=box_index) printf("%c%c%c%c%c",200,205,205,205,188);
	else printf("%c%c%c%c%c",186,196,196,196,186);


}




void print_KenKen(KenKen KK)
{
	setcolor(2);
	gotoxy(0,0);
	printf("                                           __                      .___      __\n");          
    setcolor(3);printf("                                          |  | __ ____   ____    __| _/____ |  | ____ __\n"); 
    setcolor(4);printf("                                          |  |/ // __ \\ /    \\  / __ |/  _ \\|  |/ /  |  \\\n");
    setcolor(5);printf("                                          |    <\\  ___/|   |  \\/ /_/ (  <_> )    <|  |  /\n");
    setcolor(6);printf("                                          |__|_ \\\\___  >___|  /\\____ |\\____/|__|_ \\____/\n"); 
    setcolor(7);printf("                                               \\/    \\/     \\/      \\/           \\/      \n");
	

	int i,j;
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			setcolor(1+KK.Box_Map[i][j]%10);
			Coord C={i,j};
			printCell(formatX,formatY,C,KK);
			formatX+=5;
			if (j==NumDigits-1) {
				formatY+=3;
				formatX-=5*NumDigits;	
			}
		}
	}
	setcolor(15);
}

//---------------------------------------------------------------------------







//List  (mien gia tri)-----------------------------------------------------
typedef struct{
	int E[MAX_NUMDIGITS+1];
	int size;
}List;


List init_List(List* L)
{
	L->size=0;
}

void append_List(List* L,int x)
{
	L->E[L->size++]=x;
}

void delete_List(List* L,int x)
{
	int i,j;
	for (i=0;i<L->size;i++)
	{
		if (L->E[i]==x)
		{
			for (j=i;j<L->size-1;j++)
			{
				L->E[j]=L->E[j+1];
			}
		}
	}
	L->size--;
}

int haveZero(List L)
{
	int i;
	for (i=0;i<L.size;i++)
	{
		if (L.E[i]==0) return 1;
	}
	return 0;
}

int findX(List L, int X)
{
	int i;
	for (i=0;i<L.size;i++)
	{
		if (L.E[i]==X) return i;
	}
	return -1;
}

int findSameElement(List L, List Remaining)
{
	int i,j;
	for (i=0;i<L.size;i++)
	{
		for (j=0;j<Remaining.size;j++)
		{
			if (L.E[i]==Remaining.E[j])
			{
				return i;
			}
		}
	}
	return -1;
}

List get_AvailableValues(Coord C,KenKen KK)
{
	int i;
	int Values[NumDigits+1];
	// Buoc 1: Khoi tao gia tri thoa ban dau, dua tren rang buoc phep toan
	char Operator=KK.ListofBoxes.B[  KK.Box_Map[C.x][C.y] ].Operator;
	int Result=KK.ListofBoxes.B[ KK.Box_Map[C.x][C.y] ].Result;
	int SizeofBox=KK.ListofBoxes.B[KK.Box_Map[C.x][C.y]].size;
	
	for (i=1;i<=NumDigits;i++) Values[i]=0;
	
	if (Operator=='=') Values[ Result ]=1; //single square
	else
	if (Operator=='/')
	{
		Values[1]=1;
		Values[Result]=1;
		
		int j;
		for (i=NumDigits;i>(NumDigits)/2;i--)
		{
			for (j=(NumDigits)/2;j>=1;j--)
			{
				if (i/j==Result && i%j==0)
				{
					Values[i]=1;
					Values[j]=1;
				}
			}
		}
	}
	else
	if (Operator== '-' && SizeofBox==2)
	{
		for (i=1;i<=NumDigits;i++)
		{
			if (i+Result > NumDigits) break;
			Values[i]=1;
			Values[i+Result]=1;
		}
	}
	else if (Operator=='+')
	{
		if (SizeofBox==2)
		{
			int i,j;
			for (i=1;i<=MAX_NUMDIGITS;i++)
			{
				for (j=1;j<=MAX_NUMDIGITS;j++)
				{
					if (i!=j && i+j==Result)
					{
						Values[i]=1;
						Values[j]=1;
					}
				}
			}
		}
		else
		{
			for (i=1;i<=NumDigits;i++) Values[i]=1;		
		}
	
	}
	else if (Operator=='x')
	{
		if (SizeofBox==2)
		{
			int i,j;
			for (i=1;i<=MAX_NUMDIGITS;i++)
			{
				for (j=1;j<=MAX_NUMDIGITS;j++)
				{
					if (i!=j && i*j==Result)
					{
						Values[i]=1;
						Values[j]=1;
					}
				}
			}
		}
		else
		{
			for (i=1;i<=NumDigits;i++) Values[i]=1;		
		}
	}
	// chua co quy luat cua phep cong va phep nhan--------------------------------------
	
	//Buoc 2: Giam mien gia tri dua vao rang buoc dong va cot	
	int vertex=Indexof(C);
	for (i=0;i<KK.Constr.Coord_number;i++)
	{
		if (KK.Constr.data[vertex][i]==1)
		{
			Coord Temp=Coordof(i);
			Values[ KK.Given[Temp.x][Temp.y] ]=0;
		}
	}
		
	List L; init_List(&L);
	for (i=1;i<=NumDigits;i++)
	{
		if (Values[i]==1) 
		{
			append_List(&L,i);
	
		}
	}

	
	return L;
}

// solve kenken with contraints--------------------------------------------
#define INF 999999



Coord getNextMinDomainCell(KenKen KK)
{
	int min_value=INF;
	
	int i,j;
	Coord MinCell={0,0};
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			if (KK.Given[i][j]==0)
			{
				Coord C={i,j};
				
				int length=get_AvailableValues(C,KK).size;
				if (length < min_value)
				{
					MinCell=C;
					min_value=length;
				}	
			}

		}
	}
	return MinCell;
}

int isFilledKenKen(KenKen KK)
{
	int i,j;
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			if (KK.Given[i][j]==0) return 0;
		}
	}
	return 1;
}


int isFilledBox(int Box_index, KenKen KK)
{
	int i;
	for (i=0;i<KK.ListofBoxes.B[Box_index].size;i++)
	{
		Coord C=KK.ListofBoxes.B[Box_index].C[i];
		if (KK.Given[C.x][C.y]==0) return 0;
	}
	return 1;
}

int check_Boxes_ofKenKen(int Box_index,KenKen KK)
{
	if (isFilledBox(Box_index,KK))
	{
		char Operator= KK.ListofBoxes.B[Box_index].Operator;
		int Con_Value= KK.ListofBoxes.B[Box_index].Result;
		int S;
		if (Operator=='x' || Operator=='/') S=1;
		if (Operator=='-' || Operator=='+' || Operator=='=') S=0;
		int i;
		for (i=0;i<KK.ListofBoxes.B[Box_index].size;i++)
		{
			Coord C=KK.ListofBoxes.B[Box_index].C[i];
			if (Operator=='x') S*= KK.Given[C.x][C.y];
			if (Operator=='/') S= max(S,KK.Given[C.x][C.y])/ min (S,KK.Given[C.x][C.y]);
			if (Operator=='=') S=KK.Given[C.x][C.y];
			if (Operator=='+') S+=KK.Given[C.x][C.y];
			if (Operator=='-') S=abs(S-KK.Given[C.x][C.y]);
			
			
		}
		if (S==Con_Value) return 1;
		else return 0;
	}		
	return 1;
}

int exploredCounter=0;
int KenKenBackTracking(KenKen* KK,KenKen *Backup)
{	
	if (isFilledKenKen(*KK)) return 1;
	int i;
	Coord C=getNextMinDomainCell(*KK);
	List L=get_AvailableValues(C,*KK);
	//printf("%d_%d\n",C.x,C.y);	
	if (L.size==0) 
	{
		return 0;
	}
	for (i=L.size-1;i>=0;i--)
	{
		KK->Given[C.x][C.y]=L.E[i];
		exploredCounter++;
		int Box_index=KK->Box_Map[C.x][C.y];
		if (check_Boxes_ofKenKen(Box_index,*KK))
		{
			if (KenKenBackTracking(KK,Backup)) 
			{
				return 1;	
			}
		} 
		KK->Given[C.x][C.y]=0;
		*Backup=*KK;	
		
	}
	
	return 0;
}

void solve_KenKen_with_constraints(KenKen *KK,KenKen *Backup)
{
	// giai bang giai thuat rang buoc
	*Backup=*KK;
	if (KenKenBackTracking(KK,Backup))
	{	
		//printf("\nCAN BE SOLVED WITH CONSTRAINTS ALGORITHM. EXPLORED %d STATES\n",exploredCounter);			
	}
	else
	{	
		//printf("\nCANNOT BE SOLVED WITH CONSTRAINT ALGORITHM\n");
	}
	int i,j;
	
	// xu ly backup
	for (i=0;i<Backup->ListofBoxes.Box_number;i++)
	{
		if (!isFilledBox(i,*Backup))
		{
			
			for (j=0;j<Backup->ListofBoxes.B[i].size;j++)
			{
				Coord C=Backup->ListofBoxes.B[i].C[j];
				Backup->Given[C.x][C.y]=0;
			}
		}
	}
	
	int count=0;
	if (NumDigits!=9)
	{
		while (count!=NumDigits/2+2)
		{
		
			int r=int_rand(0,NumDigits-1);
			for (i=0;i<NumDigits;i++)
			{
				Backup->Given[r][i]=0;
			}	
			count++;
		}		
	}
	
}






// candidate (ca the)--------------------------------------------------------------------
 
 typedef struct{
	int Values[MAX_NUMDIGITS][MAX_NUMDIGITS];
	double Fitness;
}Candidate;


void init_Candidate(Candidate* Can)
{
	int i,j;
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			Can->Values[i][j]=0;
		}
	}
	Can->Fitness=0;
}



List available_Values(int row,int col,Candidate C,KenKen KK)
{
	int i;
	
	int Values[NumDigits+1]; // 1->NumDigits, khong su dung 0
	
	for (i=1;i<NumDigits+1;i++) // khoi tao ban dau
	{
		Values[i]=1;
	}
	
	for (i=0;i<NumDigits;i++) // giam mien gia tri
	{
		if (col!=i) Values[ C.Values[row][i] ]=0;
		
		if (KK.Given[i][col]!=0 && row!=i) Values[ KK.Given[i][col] ]=0;
		if (KK.Given[row][i]!=0 && col!=i) Values[ KK.Given[row][i] ]=0;
	}
	
	
	
	List L; init_List(&L);
	
	for (i=1;i<NumDigits+1;i++)
	{
		if (Values[i]==1) append_List(&L,i);
	}
	return L;
}

void print_Candidate(Candidate Can)
{
	int i,j;
	for (i=0;i<NumDigits;i++)
	{
		for (j=0;j<NumDigits;j++)
		{
			printf("%d ",Can.Values[i][j]);
		}
		printf("\n");
	}
	printf("FITNESS = %.3f\n",Can.Fitness);
}

void update_Fitness(Candidate* Can, KenKen KK)
{
	int i,j;
	double boxsum=0.0;
	
	for (i=0;i<KK.ListofBoxes.Box_number;i++)
	{
		char Operator= KK.ListofBoxes.B[i].Operator;
		int Con_Value= KK.ListofBoxes.B[i].Result;
		int S=0;
		if (Operator=='x' || Operator=='/') S=1;
		if (Operator=='-' || Operator=='+' || Operator=='=') S=0;
		int j;
		for (j=0;j<KK.ListofBoxes.B[i].size;j++)
		{
			Coord C=KK.ListofBoxes.B[i].C[j];
			if (Operator=='x') S*= Can->Values[C.x][C.y];
			if (Operator=='/') S= max(S,Can->Values[C.x][C.y])/ min(S,Can->Values[C.x][C.y]);
			if (Operator=='=') S=Can->Values[C.x][C.y];
			if (Operator=='+') S+=Can->Values[C.x][C.y];
			if (Operator=='-') S=abs(S - Can->Values[C.x][C.y]);
			
		}
		if (S==Con_Value) boxsum=boxsum+1.0;
	}
	
	int columncount[NumDigits];
	for (i=0;i<NumDigits;i++) columncount[i]=0;
	double columnsum=0.0;
	for (i=0;i<NumDigits;i++)
	{
		
		for (j=0;j<NumDigits;j++)
		{
			columncount[Can->Values[j][i]-1]+=1;
		}
		int nonzero=0;
		for (j=0;j<NumDigits;j++)
		{
			if (columncount[j]!=0) nonzero+=1;
		}
		columnsum=columnsum+((1.0*nonzero)/(1.0*NumDigits));
		for (j=0;j<NumDigits;j++) columncount[j]=0;
	}
	
	boxsum=boxsum/KK.ListofBoxes.Box_number;
	columnsum=columnsum/NumDigits;
	
	
	
	if (((int)boxsum==1) && ((int)columnsum)==1)
	{
		Can->Fitness=1.0;	
	}
	else 
	{
		Can->Fitness= (boxsum)*(columnsum);
	}
}




int is_colDuplicate(int value,int col,KenKen KK)
{
	int i;
	for (i=0;i<NumDigits;i++)
	{
		if (KK.Given[i][col]==value) return 1;	
	}	
	return 0;
}

int is_validRow(int row,KenKen KK)
{
	int i,count=0;
	for (i=0;i<NumDigits;i++)
	{
		if (KK.Given[row][i]==0) count++;
	}
	if (count>=3) return 1;
	else return 0;
}


int mutate(double mutationRate, Candidate* Can, KenKen KK){
	int r=float_rand(0,1.0);
	int sucess=0;
	if (r < mutationRate){
		while (!sucess){
			int row=int_rand(0, NumDigits-1);
			while (!is_validRow(row,KK))
			{
				row=int_rand(0,NumDigits-1);
			}
			
	
			int C1=int_rand(0, NumDigits-1);
			int C2=int_rand(0, NumDigits-1);
			
			while (C1==C2) 
			{
				//printf("%d %d %d\n",row,C1,C2);		
				C1=int_rand(0, NumDigits-1);
				C2=int_rand(0, NumDigits-1);
			}
			if(KK.Given[row][C1]==0 && KK.Given[row][C2]==0 &&
			!is_colDuplicate(Can->Values[row][C2],C1,KK) && !is_colDuplicate(Can->Values[row][C1],C2,KK))
			{
				
				int temp=Can->Values[row][C1];
				Can->Values[row][C1]=Can->Values[row][C2];
				Can->Values[row][C2]=temp;	
				sucess=1;	
			}
		}
	}
	return sucess;
}

void crossoverRows(int crossoverpoint, Candidate* Child1, Candidate* Child2)
{
	//
	List childrow1, childrow2, remaining, row1, row2;
	init_List(&childrow1); init_List(&childrow2); init_List(&remaining);
	init_List(&row1); init_List(&row2);
	int i;
	for (i=0;i<NumDigits;i++)
	{
		append_List(&row1,Child1->Values[crossoverpoint][i]);
		append_List(&row2,Child2->Values[crossoverpoint][i]);
		append_List(&childrow1,0);
		append_List(&childrow2,0);
	}
	for (i=1;i<=NumDigits;i++)
	{
		append_List(&remaining,i);
	}
	//
	int cycle=0;
	while (haveZero(childrow1) && haveZero(childrow2))
	{
		int index=findSameElement(row1,remaining);
		int start=Child1->Values[crossoverpoint][index];
		int next=Child2->Values[crossoverpoint][index];
		delete_List(&remaining,start);
		if (cycle%2==0)
		{
			childrow1.E[index]=start;
			childrow2.E[index]=next;
			while (start!=next)
			{
				index=findX(row1,next);
				childrow1.E[index]=Child1->Values[crossoverpoint][index];
				delete_List(&remaining, Child1->Values[crossoverpoint][index] );
				next=Child2->Values[crossoverpoint][index];
				childrow2.E[index]=next;	
			}			
		}
		else
		{
			childrow1.E[index]=next;
			childrow2.E[index]=start;
			while (start!=next)
			{
					
				index=findX(row1,next);
				childrow2.E[index]=Child1->Values[crossoverpoint][index];
				delete_List(&remaining, Child1->Values[crossoverpoint][index] );
				next=Child2->Values[crossoverpoint][index];
				childrow1.E[index]=next;	
			}
					
		}
		cycle++;
	}
	//
	for (i=0;i<NumDigits;i++)
	{
		Child1->Values[crossoverpoint][i]=childrow1.E[i];
		Child2->Values[crossoverpoint][i]=childrow2.E[i];
	}
}

//-------------------------------------------------------------





//Population------------------------------------------------------
typedef struct{
	Candidate Can[MAX_CANDIDATES];
	int size;
}Population;


void init_Population(Population* P)
{
	P->size=0;
}

void append_Population(Population *P,Candidate C)
{
 	P->Can[P->size++]=C;
}


void sort_Population(Population *P)
{
	int i,j;
	for (i=0;i<P->size-1;i++)
	{
		for (j=i+1;j<P->size;j++)
		{
			if (P->Can[i].Fitness < P->Can[j].Fitness)
			{
				Candidate Temp=P->Can[i];
				P->Can[i]=P->Can[j];
				P->Can[j]=Temp;
			}
		}
	}
}


void seed_Population(Population *P, int number_of_Candidate, KenKen KK)
{	
	init_Population(P);
	int p;
	for (p=0;p<number_of_Candidate;p++)
	{
		// xai ham Available_Values, neu size==0, thi khoi tao random lai dong do(i--) )
		int isValid=1;
		
		Candidate Can;  init_Candidate(&Can); // khoi tao ca the
		// random gia tri cho ca the
		int i,j;
		for (i=0;i<NumDigits;i++)
		{
			for (j=0;j<NumDigits;j++)
			{
				if (KK.Given[i][j]!=0)
				{
					Can.Values[i][j]=KK.Given[i][j];
				}
				else
				{
					List Values=available_Values(i,j,Can,KK);
				
					if (Values.size==0)
					{
						isValid=0;
						break;
					}		
					else
					{
						 int index = int_rand(0,Values.size-1);
						 Can.Values[i][j]=Values.E[index];
					}
				}
			}
			if (isValid==0)
			{
				p--; break;
			}
		}
		if (isValid==1)
		{
			update_Fitness(&Can,KK);
			append_Population(P,Can);
		} 	
	}
}




void print_Population(Population P)
{
	int i;
	for (i=0;i<P.size;i++)
	{
		printf("_____CANDIDATE %d_____\n",i+1);
		print_Candidate(P.Can[i]);
	}
}

Candidate compete(Population P)
{
	Candidate C1=P.Can[int_rand(0,P.size-1)];
	Candidate C2=P.Can[int_rand(0,P.size-1)];
	
	Candidate Fittest, Weakest;
	if (C1.Fitness > C2.Fitness)
	{
		Fittest=C1;
		Weakest=C2;
	}
	else
	{
		Fittest=C2;
		Weakest=C1;
	}
	
	double selectionRate=0.85;
	double ran=float_rand(0,1.0);
	
	if (ran < selectionRate)	return Fittest;
	else return Weakest;
}


Population crossover(Candidate Parent1, Candidate Parent2, double crossoverRate,KenKen KK)
{	
	Candidate Child1=Parent1;
	Candidate Child2=Parent2; 
	

	double r=float_rand(0,1.0);
	if (r<crossoverRate)
	{
		
		int crossoverRange1=int_rand(0,NumDigits-1);
		int crossoverRange2=int_rand(1,NumDigits);
		
		while (crossoverRange1==crossoverRange2)
		{
			crossoverRange1=int_rand(0,NumDigits-1);
			crossoverRange2=int_rand(1,NumDigits);			
		}
		
		if (crossoverRange1 > crossoverRange2)
		{
			int temp=crossoverRange1;
			crossoverRange1=crossoverRange2;
			crossoverRange2=temp;
		}
		int i;
		for (i=crossoverRange1;i<crossoverRange2;i++)
		{
			crossoverRows(i,&Child1,&Child2);				
		}
			
			
	}
	Population P; init_Population(&P);
	append_Population(&P,Child1);
	append_Population(&P,Child2);
	return P;
}
//--------------------------------------------------------------------------------------------



Candidate solve_KenKen(KenKen KK)
{
	int Num_Candidates=1000;
	int Num_Elites=0.05*Num_Candidates;
	int Num_Generations=1000;
	int Num_Mutations=0;	
	double phi=0, sigma=1, mutationRate=0.06;	
	Population P; init_Population(&P);
	seed_Population(&P,Num_Candidates,KK);	
	int i,stale=0;
	for (i=0;i<Num_Generations;i++)
	{
		printf("_Generation %d_\n",i);		
		double best_Fitness=0.0;
		sort_Population(&P);
		if (P.Can[0].Fitness==1) return P.Can[0];
		if (P.Can[0].Fitness > best_Fitness) best_Fitness=P.Can[0].Fitness;
		printf("_Best Fitness: %.3f_\n",best_Fitness);		
		Population Next_P; init_Population(&Next_P); 	
		Population Elites; init_Population(&Elites);
		int k;
		for (k=0;k<Num_Elites;k++)
		{
			append_Population(&Elites,P.Can[k]);
		}
		int count;
		for (count=Num_Elites;count<Num_Candidates;count+=2)
		{			
			Candidate Parent1=compete(P);
			Candidate Parent2=compete(P);			
			Population Result; init_Population(&Result);
			Result=crossover(Parent1,Parent2,1.0,KK);
			double old_Fitness=Result.Can[0].Fitness;
			int success=mutate(mutationRate,&Result.Can[0],KK);
			update_Fitness(&Result.Can[0],KK);
			if (success)
			{
				Num_Mutations+=1;
				if (Result.Can[0].Fitness > old_Fitness) phi=phi+1.0;
			}			
			old_Fitness=Result.Can[1].Fitness;
			success=mutate(mutationRate,&Result.Can[1],KK);
			update_Fitness(&Result.Can[1],KK);
			if (success)
			{
				Num_Mutations+=1;
				if (Result.Can[1].Fitness > old_Fitness) phi=phi+1.0;
			}
			append_Population(&Next_P,Result.Can[0]); 
			append_Population(&Next_P,Result.Can[1]);
			
		}		
		for (k=0;k<Elites.size;k++) append_Population(&Next_P,Elites.Can[k]);		
		P=Next_P;
		for (k=0;k<P.size;k++) update_Fitness(&P.Can[k],KK);		
		if (Num_Mutations==0)
		{
			phi=0;
		}
		else
		{
			phi=phi/(1.0*Num_Mutations);
		}
		if (phi>0.2)
		{
			sigma=sigma/0.998;
		}
		else
		{
			sigma=sigma*0.998;
		}
		mutationRate=gaussian_rand(0.0,sigma);		
		sort_Population(&P);		
		if (P.Can[0].Fitness != P.Can[1].Fitness) stale=0;
		else stale+=1;
		if (stale>=100)
		{
			printf("Stale Population. Re-seeding...\n");
			seed_Population(&P,Num_Candidates,KK);
			stale=0;
			sigma=1;
			phi=0;
			Num_Mutations=0;
			mutationRate=0.06;
		}			
	}
}

//-------------------------------------------------------------------------------------------
int main()
{
	srand(time(0));
	KenKen KK; read_KenKen(&KK,"INPUTS/KenKen5_easy.txt");
	KenKen Backup;
	print_KenKen(KK);
	printf("\n");
	solve_KenKen_with_constraints(&KK,&Backup);
	print_KenKen_info(Backup);
	Candidate KQ=solve_KenKen(Backup);
	print_Candidate(KQ);	
	return 0;
}

