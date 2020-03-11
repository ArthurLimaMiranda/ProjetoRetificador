# include<stdio.h>
sbit cs=P3^5;
sbit miso=P1^6;
sbit mosi=P1^5;
sbit clk=P1^7;

float readadc(bit d1,bit d0)
{
unsigned int adc_val=0;
float temp;
char i;

cs=1;
clk=mosi=1;

cs=0;
clk=0;clk=1;

clk=0;mosi=1;clk=1;
clk=0;mosi=1;clk=1;

clk=0;mosi=d1;clk=1;
clk=0;mosi=d0;clk=1;

clk=0;clk=1;
clk=0;clk=1;

for(i=11;i>=0;i--)
{
clk=0;

if(miso)
adc_val|=(1<<i);

clk=1;
}
cs=1;

temp=((adc_val*4.096)/4095);//((adc_val*5.0)/4095);

return temp;
}



void init_uart(){

	SCON=0X52;
	TMOD=0X20;
	TH1=0xF3;
	TR1=1;
	//TI=1;
	IE = 0x93; /*IE.4 and global */
	//RI = 0;

}

/*
int main()
{ 
int i=0;
init_uart();
while(1)
{
printf("mcp3004.........................................\n\n");
printf("chnl1 :: %f\n",readadc(0,0));
printf("chnl2 :: %f\n",readadc(0,1));
printf("chnl3 :: %f\n",readadc(1,0));
printf("chnl4 :: %f\n",readadc(1,1));
printf("................................................\n\n");

for(i=0;i<30000;i++);
for(i=0;i<30000;i++);
for(i=0;i<30000;i++);

}
return 0;
}
*/