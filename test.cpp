#include <stdio.h>

#if(0)
int main()
{
	int maxIndex = 0;
	int numberOfDiffEquals1=0;
	//char allCard[7] = {0x12, 0x23, 0x0A, 0x19, 0x29, 0x28,0x07};
	char allCard[7];

	while (scanf("%x%x%x%x%x%x%x", &allCard[0], &allCard[1], &allCard[2], &allCard[3], &allCard[4], &allCard[5], &allCard[6]))
	{
		for (int i=2;i<7;i++)//2-6
		{
			int j = i;
			
			numberOfDiffEquals1 = 0;
			while (j<7-1)//
			{
				if( ((allCard[j] & 0x0F) - (allCard[j+1] &0x0F) == 1) || (( (allCard[j] & 0x0F) == 0x01) && ( (allCard[j+1] & 0x0F) == 0x0D)) )
				{
					numberOfDiffEquals1++;
				}
				else if ((allCard[j] & 0x0F) == (allCard[j+1] &0x0F))
				{
					j = j;
				}
				else
					break;
				
				if (numberOfDiffEquals1 == 2)
				{
					break;
				}
				j++;
			}
			if (numberOfDiffEquals1 == 2)
			{
				break;
			}
			
		}
		int maxStraightNumber = allCard[i];
		printf("0x%02X\n", maxStraightNumber);
	}

	return 0;
}
#endif