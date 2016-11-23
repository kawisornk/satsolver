#include<stdio.h>
#include<vector>
#include<ctype.h>
/*
priority
( )
/\
\/
X
=>
<=>
~ -
*/
int preproc(char *filename)
{
	FILE *fout=fopen("suf.aon","w");
	std::vector<char> op;
	char c;
	int i, b=1, deja=0, check=1, nt=0;
	if(filename==NULL)
	{
		while(deja || ((c=getchar()) && b && check++))
		{
			deja=0;
			switch(c)
			{
				case '(':
					op.push_back('(');
				break;

				case ')':
					while(op.back()!='(')
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.pop_back();
				break;
			
				case '/':
					c=getchar();
					if(c=='\\')
						op.push_back('&');
					else
						printf("error\n");
				break;

				case '\\':
					c=getchar();
					if(c=='/')
					{
						while(!op.empty() && op.back()=='&')
						{
							op.pop_back();
							fprintf(fout,"&");
						}
						op.push_back('|');
					}
					else
						printf("error\n");
				break;

				case 'X':
					while(!op.empty() && ( op.back()=='&' || op.back()=='|') )
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.push_back('X');
				break;

				case '=':
					c=getchar();
					if(c=='>')
					{
						while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X'))
						{
							fprintf(fout,"%c",op.back());
							op.pop_back();
						}
						op.push_back('I');
					}
					else
						printf("error\n");
				break;
			
				case '<':
					c=getchar();
					if(c=='=')
					{
						c=getchar();
						if(c=='>')
						{
							while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X' || op.back()=='I'))
							{
								fprintf(fout,"%c",op.back());
								op.pop_back();
							}
							op.push_back('f');
						}
						else
							printf("error\n");
					}
					else
						printf("error\n");
				break;

				case '~':
					while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X' || op.back()=='I' || op.back()=='f'))
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.push_back('!');	
				break;

				case ' ':
				case '\t':
				break;

				case '\n':
				case EOF:
					b=0;
				break;		

				default:
					nt=0;
					if(c=='-')
					{
						nt=1;
						c=getchar();
					}
					if(isdigit(c))
					{
						i=0;
						fprintf(fout,"i");
						while(isdigit(c))
						{
							fprintf(fout,"%c",c);
							i++;
							c=getchar();
						}
						if(nt)
							fprintf(fout,"!");
						deja=1;
					}
					else
						printf("unknown character: %d\n",c);
				
					break;
			}
		}
	}
	else
	{
		FILE *fin=fopen(filename,"r");
		while(deja || ((c=fgetc(fin)) && b && check++))
		{
			deja=0;
			switch(c)
			{
				case '(':
					op.push_back('(');
				break;

				case ')':
					while(op.back()!='(')
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.pop_back();
				break;
			
				case '/':
					c=fgetc(fin);
					if(c=='\\')
						op.push_back('&');
					else
						printf("error\n");
				break;

				case '\\':
					c=fgetc(fin);
					if(c=='/')
					{
						while(!op.empty() && op.back()=='&')
						{
							op.pop_back();
							fprintf(fout,"&");
						}
						op.push_back('|');
					}
					else
						printf("error\n");
				break;

				case 'X':
					while(!op.empty() && ( op.back()=='&' || op.back()=='|') )
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.push_back('X');
				break;

				case '=':
					c=fgetc(fin);
					if(c=='>')
					{
						while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X'))
						{
							fprintf(fout,"%c",op.back());
							op.pop_back();
						}
						op.push_back('I');
					}
					else
						printf("error\n");
				break;
			
				case '<':
					c=fgetc(fin);
					if(c=='=')
					{
						c=fgetc(fin);
						if(c=='>')
						{
							while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X' || op.back()=='I'))
							{
								fprintf(fout,"%c",op.back());
								op.pop_back();
							}
							op.push_back('f');
						}
						else
							printf("error\n");
					}
					else
						printf("error\n");
				break;

				case '~':
					while(!op.empty() && (op.back()=='&' || op.back()=='|' || op.back()=='X' || op.back()=='I' || op.back()=='f'))
					{
						fprintf(fout,"%c",op.back());
						op.pop_back();
					}
					op.push_back('!');	
				break;

				case ' ':
				case '\t':
				break;

				case '\n':
				case EOF:
					b=0;
				break;		

				default:
					nt=0;
					if(c=='-')
					{
						nt=1;
						c=fgetc(fin);
					}
					if(isdigit(c))
					{
						i=0;
						fprintf(fout,"i");
						while(isdigit(c))
						{
							fprintf(fout,"%c",c);
							i++;
							c=fgetc(fin);
						}
						if(nt)
							fprintf(fout,"!");
						deja=1;
					}
					else
						printf("unknown character: %d\n",c);
				
					break;
			}
		}
	}
	while(!op.empty())
		{
			fprintf(fout,"%c",op.back());
			op.pop_back();
		}	
	fprintf(fout,"\n0");
	fclose(fout);
	return 0;
}
