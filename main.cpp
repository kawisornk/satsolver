#include "sat.h"
#include "preproc.h"
#include "tseitin.h"
int ts=0;
char *filename;

void param(char *c)
{
	if(c==NULL)
		return;
	else if(strcmp(c, "ts")==0)
		ts=1;
	else if(strcmp(c, "wl")==0)
		wl=true;
	else if(strcmp(c, "cl")==0)
		cl=true;
	else if(strcmp(c, "interac")==0)
		interactive=true;
	else if(strcmp(c, "moms")==0)
		moms=true;
	else if(strcmp(c, "moms")==0)
		dlis=true;
	else
	{
		filename = (char*) malloc(sizeof(char)*strlen(c));
		strcpy(filename, c);
	}
	return;
}

void read(char* s)
{
	char *c;
	c=strtok(s,"- ");
	while(c!=NULL)
	{
		param(c);
		c=strtok(NULL,"- ");
	}
	return;	
}

int main(int argc, char** argv)
{	
	for(int temp1=0;temp1<1000;temp1++){
		for(int temp2=0;temp2<100;temp2++){
			mat[temp1][temp2]=0;
		}
	}
	int i=0, x, ncl;
	filename=NULL;
	for(i=1;i<argc;i++)
		read(argv[i]);		//read parameters ts wl cl interac and filename
	preproc(filename);
	tseitin(ts);
	FILE* fin = fopen("input.cnf","r");
	V=0;
	char c, s[4];
	c=fgetc(fin);
	i=0;
	while(c!=EOF)
	{
		switch(c)
		{
			case 'p':
			fscanf(fin, "%s",s);
			if(strcmp(s,"cnf")) printf("Error reading \"en-tete\"\n");
			else fscanf(fin, "%d %d", &V, &ncl);
			break;
			
			case 'c':
			while(fgetc(fin) != '\n');
			break;

			case '-':
			case '0': 
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			ungetc(c,fin);
			i++;
			break;

			case ' ':
			case '\n':
			case '\t':
			break;

			default:
			printf("line reading error\n");
			break;			
		}
		if(isdigit(c)!=0 || c=='-')
		{
			fscanf(fin,"%d",&x);
			while(x!=0)
			{	if(mat[i][ab(x)]!=0)mat[i][0]=1;
				mat[i][ab(x)]=(x>0)?1:-1;
				fscanf(fin, "%d",&x);
			}
			if(mat[i][0]==1)i--;
		}
		c=fgetc(fin);
	}
	if(i>ncl) printf("Ce fichiers contient %d clauses mais pas %d\n", i, ncl);
	nbclause = i;
	
	for(i=1;i<=V;i++){
		valuation[i]=0;
		date[i]=-1;
	}
    /*
	for(i=1;i<=nbclause;i++){
		for(int j=1;j<=V;j++){
			if(mat[i][j]==-1)cout << mat[i][j] << " ";
			else { cout << " " << mat[i][j] << " "; }
		}
		cout << endl;
	}
*/
	currentlevel = 0;
	/* watched literals initialisation */
	int b;
	for(i=1;i<=nbclause;i++)
	{
		b=0;
		for(int j=1;j<=V;j++)
		{
			if(mat[i][j]!=0 && valuation[j]!=-mat[i][j])
			{
				if(b==0)
				{
					wlit[i][0]=j;
					b++;
				}
				else if(b==1)
				{
					wlit[i][1]=j;
					b++;
				}
				else
					break;
			}
		}
	}
	/* wli */
	if(cl)solvecl(0);
	else{ ans = dfs(1);}
	if(ans)
	{
		V=compute_lvar();
		printf("s SATISFIABLE\n");
		for(int it=1;it<=V;it++){
			if(valuation[it]==-1)cout << -it << " ";
			else cout << it << " ";
		}	
		cout << 0 << endl;
	}
	else
		printf("s UNSATISFIABLE\n");
	

	return 0;
}

