//an arena game made in C using only terminal characters

int main() {
	
	int world[10][11];
	int px=5, py=5, sx=50, sy=50;
	int counter=0, score=0, boost=8;
	int go=0, hit=0, stun=0;
	char ac='.';
	
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<11;j++)
		{
			world[i][j]=0;
		}
	}
	
	while(1)
	{
		
		if(world[px][py]==2){break;}
		
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<11;j++)
			{
				if(world[i][j]==2)
				{
					if((i==sx) && (j==sy)){break;}
					
					world[i][j]=0;
					
					if(i>px)
					{
						sx=i-1;
					}
					else if(i<px)
					{
						sx=i+1;
					}
					else
					{
						sx=i;
					}
					
					if(j>py)
					{
						sy=j-1;
					}
					else if(j<py)
					{
						sy=j+1;
					}
					else
					{
						sy=j;
					}
					
					if((sx==px) && (sy==py))
					{
						go=1;
						break;
					}
					else
					{
						world[sx][sy]=2;
					}
				}
			}
			if(go==1){break;}
		}
		if(go==1){break;}
		
		world[sx][sy]=2;
		sx=50;
		sy=50;
		
		world[px][py]=1;
		
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("				#############\n");
		for(int i=0;i<10;i++)
		{
			printf("				#");
			for(int j=0;j<11;j++)
			{
				if(world[i][j]==0)
				{
					printf("_");
				}
				else if(world[i][j]==1)
				{
					printf("0");
				}
				else if(world[i][j]==2)
				{
					printf("X");
				}
			}
			printf("#");
			if(i==3)
			{
				printf("	W-A-S-D -> MOVEMENT\n");
			}
			else if(i==4)
			{
				printf("	Q -> DASH\n");
			}
			else if(i==5)
			{
				printf("	E -> ATTACK\n");
			}
			else
			{
				printf("\n");
			}
		}
		printf("				#############\n");
		printf("\n");
		printf("\n");
		printf("				SCORE:%d\n", score);
		printf("\n");
		printf("				DASH!\n");
		printf("				##########\n");
		printf("				#");
		for(int i=0;i<boost;i++)
		{
			printf("0");
		}
		for(int i=0;i<(8-boost);i++)
		{
			printf(" ");
		}
		printf("#\n");
		printf("				##########\n");
		
		if(counter%8==0)
		{
			sx = rand() % 11;
			sy = rand() % 12;
			
			if(sx==px)
			{
				sx++;
			}
			if(sy==py)
			{
				sy++;
			}
			
			world[sx][sy]=2;
		}
		
		system("stty raw");
		ac=getchar();
		system("stty cooked");
		
		system("tput reset");
		
		world[px][py]=0;
		
		if(boost!=8)
		{
			boost++;
		}
		
		if(ac=='w')
		{
			stun=0;
			hit=0;
			score+=50;
			if(px!=0)
			{
				px--;
			}
		}
		else if(ac=='a')
		{
			stun=0;
			hit=1;
			score+=50;
			if(py!=0)
			{
				py--;
			}
		}
		else if(ac=='s')
		{
			stun=0;
			hit=2;
			score+=50;
			if(px!=9)
			{
				px++;
			}
		}
		else if(ac=='d')
		{
			stun=0;
			hit=3;
			score+=50;
			if(py!=10)
			{
				py++;
			}
		}
		else if(ac=='e')
		{
			if(stun==0)
			{
				if(hit==0)
				{
					if(world[px-1][py]==2)
					{
						world[px-1][py]=0;
						score+=500;
					}
				}
				else if(hit==1)
				{
					if(world[px][py-1]==2)
					{
						world[px][py-1]=0;
						score+=500;
					}
				}
				else if(hit==2)
				{
					if(world[px+1][py]==2)
					{
						world[px+1][py]=0;
						score+=500;
					}
				}
				else if(hit==3)
				{
					if(world[px][py+1]==2)
					{
						world[px][py+1]=0;
						score+=500;
					}
				}
				stun=1;
			}
		}
		else if(ac=='q')
		{
			if(boost==8)
			{
				boost=0;
				if((hit==0) && (px>1))
				{
					world[px][py]=0;
					px=px-2;
					world[px][py]=0;
				}
				else if((hit==1) && (py>1))
				{
					world[px][py]=0;
					py=py-2;
					world[px][py]=0;
				}
				else if((hit==2) && (px<9))
				{
					world[px][py]=0;
					px=px+2;
					world[px][py]=0;
				}
				else if((hit==3) && (py<8))
				{
					world[px][py]=0;
					py=py+2;
					world[px][py]=0;
				}
				stun=0;
			}
		}
		else
		{
			stun=1;
		}
		counter++;
	}
	
	return 0;
}
