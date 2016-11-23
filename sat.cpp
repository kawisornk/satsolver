#include<stdio.h>
#include<vector>
#include<string.h>
#include<ctype.h>
#include<iostream>
#include "sat.h"
#include<math.h>
#include<queue>
#include<map>
#include<fstream>
using namespace std;

#define forv(i,v,type) for (vector<type>::iterator i = v.begin(); i != v.end(); ++i)
#define forn(i,n) for(int i=1;i<=n;i++)

int date[1000]={0};
int V;// number of variable
int valuation[1000];
int mat[1000][1000]={{0}};
int nbclause;
int dateclause[1000]={0};//level of decision that a value of a clause is determined
int currentlevel=0;
int glevel;
bool interactive=false;
bool cl=false;
bool ans = false;
bool dlis = false;
int visit[1000]={0};
int reason[1000]={0}; // a clause that i is deduced
bool moms = false;
int wlit[1000][2];
bool wl = false;
// TRUE = 1, FALSE = -1, UNDEFINED = 0

void checknnsure()
{
	int w1, w2, i, j, b=2, c=2;
	forn(i,nbclause)
	{
		w1=wlit[i][0];
		w2=wlit[i][1];
		if(valuation[w1]==-mat[i][w1]&&date[w1]==currentlevel)
		{
			b=0;
			for(j=1;j<=V;j++)
			{
				if(b==0 && j!=w2 && mat[i][j]!=0 && valuation[j]!=-mat[i][j])
				{
					wlit[i][0]=j;
					b=1;
				}
				if(b==1)
					break;
			}
			if(b==0)
			{
				if(valuation[w2]==0)
				{
					valuation[w2]=mat[i][w2];
					date[w2]=currentlevel;
					checknnsure();
					return;
				}
			}
			if(valuation[w2]==-mat[i][w2]&&date[w1]==currentlevel)
			{
				c=0;
				for(j=1;j<=V;j++)
				{
					if(c==0 && j!=w1 && mat[i][j]!=0 && valuation[j]!=-mat[i][j])
					{
						wlit[i][1]=j;
						c=1;
					}
					if(c==1)
						break;
				}
				if(c==0)
				{
					valuation[w1]=mat[i][w1];
					date[w1]=currentlevel;
					checknnsure();
					return;
				}
				else
					continue;
			}
		}
		if(b==0 && c==0)
			return;
	}
	if(b==2 && c==2)
		return;
	else checknnsure();
		return;		
}

int ab(int x){
	return (x>0)?x:-x;
}

//return a value of variable i
int valv(int i){
	if(i>0)return valuation[i];
	else if(i==0)return 0;
	return -valuation[-i];
}
//return a value of a clause i
int valc(int i){
	if(mat[i][0]==1)return 1;
	else if(mat[i][0]==-1)return -1;
	int sum=0;
	forn(j,V){	if(mat[i][j]==0)continue;
				if(valv(mat[i][j]*j)==1){
										mat[i][0]=1;
										dateclause[i]=currentlevel;
										return 1;
										}
				if(valv(mat[i][j]*j)==0)sum=1;
			 }
	if(sum!=1){ mat[i][0]=-1;
				dateclause[i]=currentlevel;
			  }
	return (sum==1)?0:-1;
}

int marrier(int i,int j){
	
	nbclause++;
	mat[nbclause][V+1]=0;
	forn(k,V){
		mat[nbclause][k]=mat[i][k]+mat[j][k];
		if(mat[nbclause][k]>0){mat[nbclause][k] = 1;mat[nbclause][V+1]++;}
		if(mat[nbclause][k]<0){mat[nbclause][k] = -1;mat[nbclause][V+1]++;}
		//mat[i][k] = 0;
		//mat[j][k] = 0;
	}
	//remove clause i and j
	//mat[i][0]=1;
	//mat[j][0]=1;
	return nbclause;
}

void clauselearning(int i){
	int minlevel=0,n=0,m=0;
	forn(j,V){
	if(mat[i][j]!=0&&date[j]==currentlevel){n++;m++;}
	if(mat[i][j]!=0&&date[j]!=currentlevel){m++;
		minlevel = (minlevel>date[j])?minlevel:date[j];
											}
	}
	if(m==1){
	forn(j,V){ if(mat[i][j]!=0){ if(interactive)cout << "The clause derived is " << mat[i][j]*j << endl;
								 currentlevel--;
								 return;
								}
			}
	}
	if(n==1){
		if(interactive){
			cout << "The clause derived is ";
			forn(j,V){
			if(mat[i][j]!=0){ cout << mat[i][j]*j << " ";}
		}
		cout << endl;
	}
	currentlevel=minlevel;
	return;
	}
	forn(j,V){
	if(mat[i][j]!=0&&valv(mat[i][j]*j)==-1&&date[j]==currentlevel&&reason[j]!=0){
		clauselearning(marrier(i,reason[j]));
		return;
		}							                    
	}
	return;
}
//check if we satisfy all clause
int satisfiable(){
	forn(i,nbclause){
		int x = valc(i);
		if(x==0)return 0;
		if(x==-1){	glevel=currentlevel;
					if(cl)clauselearning(i);
					if(interactive&&cl){
					char choice;
					cout << "Conflict found " << endl;
					cout << "Type g to create a conflict graph\n";
					cout << "Type c to continue until next conflict graph\n";
					cout << "Type t to continue until the end\n";
					cin >> choice;
					switch(choice){
						case 'g':
							creategraph(i);
							break;
						case 'c':
							break;
						case 't':
							interactive=false;
							break;
						default:
							break;
					}
			}
					return -1;
				 }
	}
	return 1;
}
//return a number of undefined variable in clause i
int nbundef(int i){
	if(mat[i][0]==1)return 0;
	int sum=0;
	forn(j,V){
		if(mat[i][j]!=0 && valuation[j]==0)sum++;
	}
	return sum;
}


int unit_prop(){
	forn(i,nbclause){
		int x = valc(i);
		if(x==0&&nbundef(i)==1){
				forn(j,V){
						if(mat[i][j]!=0&&valuation[j]==0){ 
												  valuation[j] = mat[i][j];
												  date[j] = currentlevel;
												  reason[j] = i;
												  return mat[i][j]*j;
												}
						 }
				}
	}
return 0;
}

int unique_pol(){
	forn(j,V){
		if(valuation[j]==0){
			int x=0,x_bar=0;
			forn(i,nbclause){
				if(valc(i)==0){
					if(mat[i][j]==1)x=1;
					else if(mat[i][j]==-1)x_bar=1;
				}
			}
			if(x==1&&x_bar==0){ valuation[j]=1;date[j]=currentlevel;return j;}
			else if(x==0&&x_bar==1){ valuation[j]=-1;date[j]=currentlevel;return j;}
		}
	}
return 0;
}

int notfull_mom(){
	int minvar = 1000;
	//find the min var among clause undef
	forn(i,nbclause){
		if(valc(i)==0){
			minvar=min(minvar,mat[i][V+1]);
		}
	}
	int currentmax=0,index=0;
	forn(i,V){
		if(valuation[i]==0){
			int k=0;
			forn(j,nbclause){
				if(valc(j)==0&&mat[j][V+1]==minvar)k++;
			}
			if(k>currentmax){
				index = i;
				currentmax = k;
			}
		}
	}
	return index;
}
int notfull_dlis(){
	int maxscore=0,index=0;
	forn(j,V){
		if(valuation[j]==0){
			int score1=0,score2=0;
			forn(i,nbclause){
				if(valc(i)==0&&mat[i][j]!=0){
					if(mat[i][j]==1)score1++;
					else score2++;
				}
			}
			score1 = max(score1,score2);
			if(score1>maxscore){ maxscore=score1;index=j;}
		}
	}
	return index;
}
int notfull(){
	if(moms){   return notfull_mom();
			}
	else if(dlis){ return notfull_dlis();}
	else{
	forn(i,V){
		if(valuation[i]==0)return i;
	}
	return 0;
	}
}


void backtrack(){
	forn(j,V){
		if(date[j]>=currentlevel){
			date[j]=0;
			valuation[j]=0;
			reason[j]=0;
		}
	}
	forn(j,nbclause){
		if(dateclause[j]>=currentlevel){ dateclause[j]=-1;mat[j][0]=0;}
	}
}

void creategraph(int i){
	ofstream fout ("igraph.dot");
	fout << "digraph G {" << endl;
	fout << "size =\"10,10\";" << endl;
	forn(j,V){
		if(date[j]==glevel)fout << valuation[j]*j << "[style=filled,color=cadetblue1];" << endl;
	}
	forn(j,V){
		if(mat[nbclause][j]!=0&&date[j]==glevel)fout << valuation[j]*j << "[style=filled,color=yellow];" << endl;
	}
	fout << "Conflict[style=filled,color=red]" << endl;
	fout << "node [style=filled,color=violet]" << endl;
	forn(j,V){
		if(mat[i][j]!=0)fout << valuation[j]*j << "->" << "Conflict" << endl;
	}
	forn(j,V){
		if(date[j]==glevel&&reason[j]!=0){
			forn(k,V){
				if(mat[reason[j]][k]!=0&&k!=j){
					fout << valuation[k]*k << "->" << valuation[j]*j << endl;
				}
			}
		}
	}
	fout << "}";
}

void solvecl(int level){
	int i;
	if(level==0){do{ i=unit_prop();
					}while(i!=0);	
				 int state = satisfiable();
				 if(state==-1){ ans=false; }
				 else if(state==1){ ans=true;}
				 else { solvecl(1);}
				 return;
				}
	int newv = notfull();
	if(newv){  
			   valuation[newv] = 1;
			   date[newv] = level;
			   currentlevel = level;
			   do{ i=unit_prop();
				}while(i!=0);	
	}
	int state = satisfiable();
	if(state==1){
		ans=true;
		return;
				}
	else if(state==0){
		solvecl(level+1);
		return;
	}
	else{
		backtrack();
		solvecl(currentlevel);
		return;
	}
}
bool dfs(int level)
{
    int  newv = notfull();
 
    if(newv>0)
    {       
            //left child
            valuation[newv]=1;
            date[newv]=level;
            currentlevel=level;
             
            int i;
	if(wl)
		checknnsure();
	else
	{	
            do{ i=unit_prop();
            }while(i!=0); 
            if(!cl&&!wl){  
            do{ i=unique_pol();
            }while(i!=0);
            }
	} 
            int state = satisfiable();
            if(state==1 || (state==0&&dfs(level+1)==1))
                return true;
            //conflict found
            currentlevel=level;
            if(currentlevel!=level)return false;
            backtrack();
            //right child
            valuation[newv]=-1;
            date[newv]=level;
            currentlevel=level;
            //cout << "At level (right)" << currentlevel<< endl;
            //cout << "The variable bet is " << newv << endl;
            if(wl)
		checknnsure();
	else
	{	
            do{ i=unit_prop();
            }while(i!=0); 
            if(!cl&&!wl){  
            do{ i=unique_pol();
            }while(i!=0);
            }
	} 
            state = satisfiable();
            if(state==1 || (state==0&&dfs(level+1)==1))
                return true;
            //conflict found
            currentlevel=level;
            if(currentlevel!=level)return false;
            backtrack();
    } 
    else{
        int state = satisfiable();
        return (state==1);
        }
}
