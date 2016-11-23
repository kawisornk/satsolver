#include<stdio.h>
#include<stdlib.h>
#include<vector>
#define EXCLAMATION 32767
#define ANDOPERATOR 32766
#define OROPERATOR 32765

struct expr
{
	int op;
	struct expr *e1;
	struct expr *e2;
};

void rfree(struct expr* e)
{
	if(e==NULL)
		return;
	if(e->e1==NULL)
	{
		if(e->e2!=NULL)
			rfree(e->e2);
	}
	else
	{
		if(e->e2!=NULL)
			rfree(e->e2);
		rfree(e->e1);
	}
	free(e);
	return;
}

void affiche(struct expr* e)
{
	switch(e->op)
	{
		case ANDOPERATOR:
		printf("( ");
		affiche(e->e1);
		printf("/\\ ");
		affiche(e->e2);
		printf(") ");
		break;

		case OROPERATOR:
		printf("( ");
		affiche(e->e1);
		printf("\\/ ");
		affiche(e->e2);
		printf(") ");
		break;

		case EXCLAMATION:
		printf("!");
		affiche(e->e1);
		break;

		default:
		printf("%d ",e->op);		
	}
	return;
}

void outp(FILE *fout, struct expr * e)
{
	if(e==NULL)
		return;
	if(e->e1 == NULL)
	{
		if(e->e2 == NULL)
		{
			fprintf(fout, "%d ",e->op);
		}
		else
		{
			e->e1=e->e2;
			outp(fout,e);
		}
	}
	else
	{
		if(e->e2 == NULL)
		{
			fprintf(fout, "-%d ",e->e1->op);
		}
		else
		{
			if(e->op == ANDOPERATOR)
			{
				outp(fout,e->e1);
				fprintf(fout,"0 \n");
				outp(fout,e->e2);
			}
			else
			{
				outp(fout,e->e1);
				outp(fout,e->e2);
			}
		}
	}
	return;
}

int lignes (struct expr* e)
{
	if(e==NULL)
		return 0;
	if(e->e1==NULL)
	{
		return 1;
	}
	else
	{
		if(e->e2==NULL)
		{
			return 1;
		}
		else
		{
			if(e->op==ANDOPERATOR)
				return (lignes(e->e1) + lignes(e->e2));
			else
				return 1;
		}
	}
}

void push_nots(struct expr** ep, int p)
{
	struct expr* e = *ep, *pp;
	if(e==NULL)
		return;
	if(p==0)
	{
		if(e->op == EXCLAMATION)
		{
			if(e->e1->e1==NULL)
				return;
			push_nots(&(e->e1),1);
			pp = e->e1;
			free(*ep);
			*ep = pp;
		}
		else
		{
			push_nots(&(e->e1),0);
			push_nots(&(e->e2),0);
		}
		return;
	}
	if(e->e1 == NULL)
	{
		if(e->e2 == NULL)
		{
			e->e1 = (struct expr*) malloc(sizeof(struct expr));
			e->e1->op = e->op;
			e->e1->e1 = NULL;
			e->e1->e2 = NULL;
			e->op = EXCLAMATION;
		}
		else
		{
			e->e1=e->e2;
			e->e2=NULL;
			push_nots(ep,1);			
		}
	}
	else
	{
		if(e->e2 == NULL)
		{
			struct expr* p;
			push_nots(&(e->e1),0);
			p=e->e1;
			free(e);
			*ep = p;
		}
		else
		{
			if(e->op == ANDOPERATOR)
			{
				e->op = OROPERATOR;
				push_nots(&(e->e1),1);
				push_nots(&(e->e2),1);
			}
			else
			{
				e->op = ANDOPERATOR;
				push_nots(&(e->e1),1);
				push_nots(&(e->e2),1);
			}
		}
	}
	return;
}

struct expr* copy(struct expr* e)
{
	if(e==NULL)
		return NULL;
	struct expr* r= (struct expr*) malloc(sizeof(struct expr));
	r->op = e->op;
	r->e1 = copy(e->e1);
	r->e2 = copy(e->e2);
	return r;	
}

void pop_ands(struct expr* e) /* go on modifying */
{
	if(e==NULL)
		return;
	if(e->e1==NULL || e->e2==NULL)
		return;
	pop_ands(e->e1);
	pop_ands(e->e2);
	if(e->op == OROPERATOR)
	{
		if(e->e1->op == ANDOPERATOR && e->e2->op == ANDOPERATOR)
		{
			struct expr *a, *b, *c, *d;
			a=e->e1->e1;
			b=e->e1->e2;
			c=e->e2->e1;
			d=e->e2->e2;

			e->op = ANDOPERATOR;
			e->e1->op = ANDOPERATOR;
			e->e2->op = ANDOPERATOR;
			e->e1->e1 = (struct expr*) malloc(sizeof(struct expr));	
			e->e1->e2 = (struct expr*) malloc(sizeof(struct expr));	
			e->e2->e1 = (struct expr*) malloc(sizeof(struct expr));	
			e->e2->e2 = (struct expr*) malloc(sizeof(struct expr));	
			e->e1->e1->op = OROPERATOR;	
			e->e1->e2->op = OROPERATOR;
			e->e2->e1->op = OROPERATOR;
			e->e2->e2->op = OROPERATOR;	
			e->e1->e1->e1 = a;
			e->e1->e1->e2 = c;
			e->e1->e2->e1 = b;
			e->e1->e2->e2 = copy(c);
			e->e2->e1->e1 = copy(a);
			e->e2->e1->e2 = d;
			e->e2->e2->e1 = copy(b);
			e->e2->e2->e2 = copy(d);
			pop_ands(e->e1->e1);
			pop_ands(e->e1->e2);
			pop_ands(e->e2->e1);
			pop_ands(e->e2->e2);
			return;
		}
		if(e->e1->op == ANDOPERATOR)
		{
			struct expr *b, *c;
			b=e->e1->e2;
			c=e->e2;

			e->op = ANDOPERATOR;
			e->e1->op = OROPERATOR;
			e->e1->e2 = c;
			e->e2 = (struct expr*) malloc(sizeof(struct expr));
			e->e2->op = OROPERATOR;
			e->e2->e1 = b;
			e->e2->e2 = copy(c);
			pop_ands(e->e1);
			pop_ands(e->e2);
			return;
		}
		if(e->e2->op == ANDOPERATOR)
		{
			struct expr *a;
			a = e->e1;
			e->e1 = e->e2;
			e->e2 = a;
			pop_ands(e);
			return;
		}
		return;
	}
}

struct expr* iff(int n, struct expr* e)
{
	struct expr* r = (struct expr*) malloc(sizeof(struct expr));
	r->op = ANDOPERATOR;
	r->e1 = (struct expr*) malloc(sizeof(struct expr));
	r->e1->op = OROPERATOR;
	r->e1->e1= (struct expr*) malloc(sizeof(struct expr));
	r->e1->e1->op = EXCLAMATION;
	r->e1->e1->e1=(struct expr*) malloc(sizeof(struct expr));
	r->e1->e1->e1->op = n; r->e1->e1->e1->e1 = NULL; r->e1->e1->e1->e2 = NULL;
	r->e1->e2 = e;
	r->e2 = (struct expr*) malloc(sizeof(struct expr));
	r->e2->op = OROPERATOR;
	r->e2->e1 = (struct expr*) malloc(sizeof(struct expr));
	r->e2->e1->op = EXCLAMATION;
	r->e2->e1->e1 = copy(e);
	r->e2->e1->e2 = NULL;
	r->e2->e2 = copy(r->e1->e1->e1);
	return r; 
}

int transform(struct expr** e, int i) 
{
	if((*e)==NULL)
		return i;
	if((*e)->e1 == NULL)
	{
		if((*e)->e2 == NULL)
			return i;
		else
		{
			(*e)->e1 = (*e)->e2;
			(*e)->e2 = NULL;
			return transform(e,i);
		}
	}
	else
	{
		if((*e)->e2 == NULL)
		{
			if((*e)->e1->e1 == NULL)
			{
				if((*e)->e1->e2 == NULL)
					return i;
				(*e)->e1->e1=(*e)->e1->e2;
				(*e)->e1->e2 = NULL;
				return transform(e,i);
			}
			else
			{
				if((*e)->e1->e2 == NULL)
				{
					struct expr *p = (*e)->e1->e1;
					*e = p;
					return transform(e,i);
				}
				else
				{
					struct expr *f;
					int n;
					f=(*e)->e1;
					n=transform(&f,i);
					if(n==i)
						printf("Impossible, must be an error...\n");
					else
					{
						struct expr *p, *q;
						q=(struct expr*) malloc(sizeof(struct expr));
						q->op=EXCLAMATION;
						q->e1 = (struct expr*) malloc(sizeof(struct expr));
						q->e1->op = n-1; q->e1->e1 = NULL; q->e1->e2 = NULL;
						q->e2 = NULL;
						p=iff(n,q);
						(*e)->op = ANDOPERATOR;
						(*e)->e1 = p;
						(*e)->e2 = f;
						return n+1;
					}
				}
			}
		}
		else
		{
			int n,m;
			struct expr *f1=(*e)->e1, *f2=(*e)->e2;
			n=transform(&f1,i);
			m=transform(&f2,n);
			if(n==i)
			{
				if(m==n)
				{
					struct expr *p=*e;
					*e=iff(m,p);
					return m+1;
				}
				else
				{
					struct expr *q;
					q=(struct expr*)malloc(sizeof(struct expr));
					q->op=(*e)->op;
					q->e1=f1;
					q->e2=(struct expr*)malloc(sizeof(struct expr));
					q->e2->op=m-1; q->e2->e1=NULL; q->e2->e2=NULL;
					(*e)->op = ANDOPERATOR;
					(*e)->e1 = iff(m,q);
					(*e)->e2 = f2;
					return m+1;
				}
			}
			else
			{
				if(m==n)
				{
					struct expr *q;
					q=(struct expr*)malloc(sizeof(struct expr));
					q->op=(*e)->op;
					q->e1=(struct expr*)malloc(sizeof(struct expr));
					q->e1->op=n-1; q->e1->e1=NULL; q->e1->e1=NULL;
					q->e2=f2;
					(*e)->op = ANDOPERATOR;
					(*e)->e1 = iff(m,q);
					(*e)->e2 = f1;
					return m+1;
				}
				else
				{
					struct expr *p, *q;
					p=(struct expr*)malloc(sizeof(struct expr));
					p->op=(*e)->op;
					p->e1=(struct expr*)malloc(sizeof(struct expr));
					p->e1->op=n-1; p->e1->e1=NULL; p->e1->e2=NULL;
					p->e2=(struct expr*)malloc(sizeof(struct expr));
					p->e2->op=m-1; p->e1->e1=NULL; p->e1->e2=NULL;
					q=(struct expr*)malloc(sizeof(struct expr));
					q->op=ANDOPERATOR;
					q->e1=f1;
					q->e2=f2;
					(*e)->op=ANDOPERATOR;
					(*e)->e1=iff(m,p);
					(*e)->e2=q;
					return m+1;
				}
			}
		}
	}	
}

int compute_lvar()
{
	int i, c, r=1;;
	FILE *fin=fopen("suf.aon","r");
	do
	{
		c = fgetc(fin);
		if(c=='i')
		{
			fscanf(fin,"%d",&i);
			r=(r>i)?r:i;
		}
	}while(c!=EOF);
	fclose(fin);
	return r;
}

int tseitin(int ts)
{
//	int ts=1;
	int c;
	int i=0, n,m, end=0, l=0;
	n=compute_lvar();
	FILE *fin=fopen("suf.aon","r");
	FILE *fout=fopen("input.cnf","w");
	std::vector<struct expr*> s;
	struct expr* p;
	fprintf(fout,"p cnf ");
	n++;
	do
	{
		while((c=fgetc(fin)) != '\n' && c!=EOF)  
		{	
			p = (struct expr*) malloc(sizeof(struct expr));
			if(c=='&')
				p->op=ANDOPERATOR;
			else if(c=='|')
				p->op=OROPERATOR;
			else if(c=='!')
				p->op=EXCLAMATION;
			else if(c=='X')			// p XOR q = (p or q) & ! (p and q)
				p->op=ANDOPERATOR;
			else if(c=='I')			// p => q = !p or q
				p->op=OROPERATOR;
			else if(c=='f')			// p <=> q = p & q or !p & !q
				p->op=OROPERATOR;
			if(c=='&' || c== '|')
			{
				p->e1=copy(s[i-2]);
				p->e2=copy(s[i-1]);
				(s[i-1]);
				free(s[i-2]);
				s[i-2]=p;
				s.pop_back();
				i--;
			}
			else if(c=='!')
			{
				p->e1=copy(s[i-1]);
				p->e2=NULL;
				free(s[i-1]);
				s[i-1]=p;
			}
			else if(c=='X')
			{
				p->e1=(struct expr*) malloc(sizeof(struct expr));
				p->e1->op = OROPERATOR;
				p->e1->e1 = copy(s[i-2]);
				p->e1->e2 = copy(s[i-1]);
				p->e2=(struct expr*) malloc(sizeof(struct expr));
				p->e2->op = EXCLAMATION;
				p->e2->e1 = (struct expr*) malloc(sizeof(struct expr));
				p->e2->e2 = NULL;
				p->e2->e1->op = ANDOPERATOR;
				p->e2->e1->e1 = copy(s[i-2]);
				p->e2->e1->e2 = copy(s[i-1]);
				free(s[i-1]);
				free(s[i-2]);
				s[i-2]=p;
				s.pop_back();
				i--;
			}
			else if(c=='I')
			{
				p->e1 = (struct expr*) malloc(sizeof(struct expr));
				p->e1->op = EXCLAMATION;
				p->e1->e1 = copy(s[i-2]);
				p->e1->e2 = NULL;
				p->e2 = copy(s[i-1]);
				free(s[i-1]);
				free(s[i-2]);
				s[i-2]=p;
				s.pop_back();
				i--;
			}
			else if(c=='f')
			{
				p->e1 = (struct expr*) malloc(sizeof(struct expr));
				p->e1->op = ANDOPERATOR;
				p->e1->e1 = copy(s[i-2]);
				p->e1->e2 = copy(s[i-1]);
				p->e2 = (struct expr*) malloc(sizeof(struct expr));
				p->e2->op = ANDOPERATOR;
				p->e2->e1 = (struct expr*) malloc(sizeof(struct expr));
				p->e2->e1->op = EXCLAMATION;
				p->e2->e1->e1 = copy(s[i-2]);
				p->e1->e1->e2 = NULL;
				p->e2->e2 = (struct expr*) malloc(sizeof(struct expr));
				p->e2->e2->op = EXCLAMATION;
				p->e2->e2->e1 = copy(s[i-1]);
				p->e1->e2->e2 = NULL;
				free(s[i-1]);
				free(s[i-2]);
				s[i-2]=p;
				s.pop_back();
				i--;
			}
			else if(c=='i')
			{
				fscanf(fin,"%d",&c);
				p->op=c;
				p->e1=NULL;
				p->e2=NULL;
				s.push_back(p);
				i++;
			}
			else if(c=='0')
				end=1;	
			else
			{
				free(p);
				continue;
			}			
		}
		if(end==0)
		{
			push_nots(&s[i-1],0);
			pop_ands(s[i-1]);
			if(ts==1)
			{
				struct expr *p;
				m=transform(&(s[i-1]),n);
				if(n!=m)
				{
					p=(struct expr *)malloc(sizeof(struct expr));
					p->op=ANDOPERATOR;
					p->e1=(struct expr *)malloc(sizeof(struct expr));
					p->e1->op=m-1; p->e1->e1=NULL; p->e1->e2=NULL;
					p->e2=s[i-1];
					s[i-1]=p;
				}
				push_nots(&(s[i-1]),0);
				push_nots(&(s[i-1]),0);
				pop_ands(s[i-1]); 
			}	
			l += lignes(s[i-1]);
			i++;
			if(ts==0)
				l++;
		}
		else
			break;
	}while(c!=EOF);
	fprintf(fout,"%d %d\n",m-1, l);
	while(!s.empty())
	{
		outp(fout,s.back());
		fprintf(fout,"0\n");
		s.pop_back();
	}
	fclose(fout);
	return 0;
}
