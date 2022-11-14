#ifndef LCD_CHAR_1602_2005_4_9
#define LCD_CHAR_1602_2005_4_9
#define uchar unsigned char
#define uint unsigned int

sbit lcdrs = P2^0;   
sbit lcdrw = P2^1;
sbit lcden = P2^2;

void delay(uint z)		  //��ʱ�������˴�ʹ�þ���Ϊ11.0592MHz
{
    uint x,y;
    for(x=z;x>0;x--)
        for(y=110;y>0;y--);
}

void write_com(uchar com)   //д��ָ�����ݵ� lcd
{ 
    lcdrw=0;
    lcdrs=0;
    P0=com;
    delay(5);
    lcden=1;
    delay(5);
    lcden=0;
}

void write_data(uchar date) 	//д���ַ���ʾ���ݵ� lcd
{	
    lcdrw=0;
    lcdrs=1;
    P0=date;
    delay(5);
    lcden=1;
    delay(5);
    lcden=0;
}

void init1602()		//1602Һ����ʼ���趨
{
    lcdrw=0;
    lcden=0;
    write_com(0x3C);
    write_com(0x0c);
    write_com(0x06);
    write_com(0x01);
    write_com(0x80);
}

void write_string(uchar *pp,uint n)//����ָ��ķ��������ַ���nΪ�ַ���Ŀ
{
  int i;
  for(i=0;i<n;i++)
  write_data(pp[i]);
}
#endif