#include <iostream>

using namespace std;

class idCard
{
	private:
	string cardnum;
	bool   iscardnum;
	
	public:
	
	idCard(string &num)
	{
		cardnum = num;
		if (isRightNum())
			iscardnum = getmatch(getRemainder());
		else
			iscardnum = false;
	}
	bool isRightNum()
	{
		int i = 0;
		if (cardnum.length() == 18)
		{
			for (i = 0 ; i < 17 ; i++)
			{
				if(cardnum[i] < '0' || cardnum[i] > '9')
						break;			
			}
			if (i == 17)
				return true;
		}
		return false;
	}
	
	bool getmatch(int ch)
	{
		switch(ch)
		{		
			case 0: if (cardnum[17] == '1')
										return true;
										break;
			case 1: if (cardnum[17] == '0')
										return true;
										break;
			case 2: if (cardnum[17] == 'x')
										return true;
										break;												
			case 3: if (cardnum[17] == '9')
										return true;
										break;
			case 4: if (cardnum[17] == '8')
										return true;
										break;										
			case 5: if (cardnum[17] == '7')
										return true;
										break;										
			case 6: if (cardnum[17] == '6')
										return true;
										break;									
			case 7: if (cardnum[17] == '5')
										return true;
										break;										
			case 8: if (cardnum[17] == '4')
										return true;
										break;										
			case 9: if (cardnum[17] == '3')
										return true;
										break;											
			case 10: if (cardnum[17] == '2')
										return true;
										break;
			default:  return false;														
		}
		return false;
	}
	int getRemainder()
	{
		int sum = 0;
		int remainder = 0;
		sum = (cardnum[0]- 48) * 7 + 
					(cardnum[1]- 48) * 9 +
					(cardnum[2] - 48) * 10 + 
					(cardnum[3] - 48)* 5 + 
					(cardnum[4] - 48) * 8 + 
					(cardnum[5] - 48) * 4 + 
					(cardnum[6] - 48) * 2 + 
					(cardnum[7] - 48) * 1 + 
					(cardnum[8] - 48) * 6 + 
					(cardnum[9] - 48) * 3 + 
					(cardnum[10] - 48)* 7 + 
					(cardnum[11] - 48) * 9 + 
					(cardnum[12] - 48) * 10 + 
					(cardnum[13] - 48)* 5 + 
					(cardnum[14] - 48)* 8 + 
					(cardnum[15] - 48) * 4 + 
					(cardnum[16] - 48) * 2 ;
		
		remainder = sum % 11 ;
		return remainder;
	}
	bool checkOut()
	{
		return iscardnum;
	}
};

int main()
{
	string number;
	cout<<"input cardnum:";
	cin>>number;
	idCard idcard(number);
	cout<<boolalpha<<idcard.checkOut()<<endl;
	return 0;
}