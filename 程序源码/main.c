#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ds1302.h"
#include "lcd1602.h"
#include "iic.h"
#include "DS18B20.h"


/************�˿ڶ���********************/
sbit CS = P3 ^ 5;  //ADC0832Ƭѡ
sbit CLK = P3 ^ 6; //ADC0832ʱ��
sbit DIO = P3 ^ 7; //ADC0832�������
sbit k1 = P1 ^ 0;
sbit k2 = P1 ^ 1;
sbit k3 = P1 ^ 2;
sbit k4 = P1 ^ 3;
sbit k5 = P1 ^ 4;
sbit k6 = P1 ^ 5;
sbit k7 = P1 ^ 6;
sbit k8 = P1 ^ 7;
sbit led = P3 ^ 4;
sbit beep = P3 ^ 3;


/*****************��������***********************************/

uchar w1 = 0;
w2 = 0;
w3 = 0; //w��weight�ֽ��õ�
unsigned long maxweight = 400; // �Ƶ��������
unsigned long weight = 0; // ��ǰ�������ɳӶ���
uint skin = 0; // ���ȥƤ��Ƥ��

uchar sum = 0; // ��¼һ�����Ե�����
uchar bad = 0; // ��¼�˲�����Ҫ��ĸ���
uchar choice_error = 10;
uchar choice_weight = 100; // ����ѡ������error��steady������ĵ�Ԫ��ֵ��
uchar pro_month, pro_day, pro_hour, pro_minute;




/****************���鶨��*******************************/

//uchar code Duan[21] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E,0xbf,0xff,0xbf,0x7f}; // ���������-�͡�
//uchar p[3][6] = {"MODE 1","MODE 2","MODE 3",};
char key_status[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // ��������ĸ�ģʽ�İ����ĵ�ǰ״̬��Ĭ��s1Ϊ1��������ʾʱ��
//char v[5] = {0,0,0,0,0};
char code error[3] = {10, 20, 30}; // ��������100g 200g 300g����������Ҫ����ѡ��
uint code steady_weight[6] = {10, 50, 100, 150, 200, 250}; // ��׼������

SYSTEMTIME adjusted;//�˴�Ϊ�ṹ�嶨��

void delay_us(uint z); // us�����ӳ�
void delayms(uint ms); // ms�����ӳ�
void send(uchar dat); // �������ݴ���
void send_string(uchar *pp, uchar num); // ���ڷ����ַ���
void send_informations(); // ���ڷ�����Ϣ�ķ�װ����
void init(); // ��ʼ������
void display_deal(); // ��ʾ������������LCD1602����ʾ
void time_deal(); // ʱ�䴦����
uchar weight_deal(uchar ch); // ADת������
void key_deal(); // ����������

void delay_us(uint z)
{
    while(z--);


}
void delayms(uint ms)
{
    char t;
    while(ms--)for(t = 0; t < 120; t++);
}


void send(uchar dat)
{
    SBUF = dat;
    while(TI == 0);
    ;
    TI = 0;

}

void send_string(uchar *pp, uchar num)
{
    uchar i = 0;
    for(i = 0; i < num; i++)
    {
        send(pp[i]);
    }
}


void send_informations()
{   uchar i = 0;
    ET1 = 1;
    TR1 = 1;
    ES = 1;
    // send_string("the sum :",8);
    // w1 = sum/10;
    // w2 = sum%10;
    // send(w1+'0');
    // send(w2+'0');
    //�ȷ��ͱ������
    /*******************************�ϴε�ʱ��****************************************/
    send_string("\r\n\r\n\r\n\r\n", 8);
    send_string("the last record time:\r\n", 23);
    w1 = pro_month / 10;
    w2 = pro_month % 10;
    send(w1 + '0');
    send(w2 + '0');
    send('-');

    w1 = pro_day / 10;
    w2 = pro_day % 10;
    send(w1 + '0');
    send(w2 + '0');
    send(' ');

    w1 = pro_hour / 10;
    w2 = pro_hour % 10;
    send(w1 + '0');
    send(w2 + '0');
    send(':');
    w1 = pro_minute / 10;
    w2 = pro_minute % 10;
    send(w1 + '0');
    send(w2 + '0');
    delay(500);
    send_string("\r\n\r\n\r\n\r\n", 8);
    /*******************************�ϴεı�׼ֵ****************************************/
    send_string("the last record standard:\r\n", 27);
    w1 = choice_error / 10;
    w2 = choice_error % 10;
    send_string("the error:", 10);
    send(w1 + '0');
    send(w2 + '0');

    send_string("   the weight:", 14);
    w1 = choice_weight / 100;
    w2 = choice_weight % 100 / 10;
    w3 = choice_weight % 10;
    send(w1 + '0');
    send(w2 + '0');
    send(w3 + '0');
    send_string("kg", 2);
    send_string("\r\n\r\n\r\n\r\n", 8);
    delay(500);
    /*******************************�ϴε�����****************************************/
    send_string("the last record data:\r\n", 23);
    send_string("the sum:", 8);
    w1 = sum / 10;
    w2 = sum % 10;
    send(w1 + '0');
    send(w2 + '0');
    send_string("   the bad:", 11);
    w1 = bad / 10;
    w2 = bad % 10;
    send(w1 + '0');
    send(w2 + '0');
    send_string("\r\n\r\n\r\n\r\n", 8);
    delay(500);

    /*******************************�ϴε� ÿ�� ����****************************************/
    send_string("the detail data:\r\n", 18);

    do
    {   i++;
        // ÿ��ѭ���ͳ�ȥһ��
        w1 = i / 100;
        w2 = i % 100 / 10;
        w3 = i % 10;
        send_string("the times: ", 10);

        send(w1 + '0');
        send(w2 + '0');
        send(w3 + '0');
        weight = iic_read(0xff - i); // ����������������������ö�Ӧ
        w1 = weight / 100;
        w2 = weight % 100 / 10;
        w3 = weight % 10;
        send_string(" the weight: ", 14);
        send(w1 + '0');
        send('.');
        send(w2 + '0');
        send(w3 + '0');
        send_string("kg\r\n", 4);
        delayms(3000);
        led = ~led; // ״̬��ʾ
    } while(i <= sum);
    // ���ʱ��ɹ�������ȫ�ͳ�ȥ��

    key_status[7] = 2; // �Զ���2��
    ET1 = 0;
    TR1 = 0;
    ES = 0;
    led = 1;
}

void init()
{


    EA = 1;
    //IP=0x10;
    //��ʱ����� ----------------------------------------------------------------------------------
    ET1 = 0;
    ET0 = 0;
    TMOD = 0x21;
    TR0 = 0;
    TR1 = 0;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;



    // �������9600������--------------------------------------------------------
    TH1 = 0xfd;
    TL1 = 0xfd;
    ES = 0;
    SCON = 0x40;
    init1602();
    Initial_DS1302() ;
    led = 1; // ��ʼ��Ϩ��״̬


}

/***************************��ʾ������*******************************************************/
void display_deal()
{
    uint temp;
    uchar intT; // �¶�ʹ�õ�
    bit res;
    // ��һ����ʾ��ǰ��ģʽ��������Ϣ
    // ʱ��ģʽ
    if(key_status[1] == 0 && key_status[2] == 0 && key_status[3] == 0 && key_status[7] == 0)
    {
        write_com(0x80);
        write_string("MODE 1", 6);

        // ��ʾ����
        DateToStr(&adjusted);
        TimeToStr(&adjusted);
        write_com(0x88);
        write_string(adjusted.DateString, 8);
        write_com(0xc2);
        write_string(adjusted.TimeString, 8);
    }
    if(key_status[1] != 0)
    {
        if(weight - skin < 0)
            weight = 0;
        else
            weight = weight - skin; // ȥƤ���ֵ

        write_com(0x80);
        write_string("MODE 2", 6);

        //��һ����ʾ�����������ڶ�����ʾ��ǰ������
        write_com(0x88);
        write_string("MAX:", 4);
        w1 = maxweight % 1000 / 100;
        w2 = maxweight % 100 / 10;
        w3 = maxweight % 10;
        write_data(w1 + 0x30);
        write_data('.');
        write_data(w2 + 0x30);
        write_data(w3 + 0x30);
        write_com(0xc0);
        write_string("WEIGHT:", 7);
        w1 = weight % 1000 / 100;
        w2 = weight % 100 / 10;
        w3 = weight % 10;
        write_data(w1 + 0x30);
        write_data('.');
        write_data(w2 + 0x30);
        write_data(w3 + 0x30);
        write_string("KG", 2);
    }
    if(key_status[2] != 0)
    {

        write_com(0x80);
        write_string("MODE 3", 6);
        if(key_status[2] < 3)
        {
            write_com(0x88);
            write_string("ERROR:", 6);
            w1 = choice_error / 10;
            w2 = choice_error % 10;

            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_com(0xc0);
            write_string("WEIGHT:", 7);
            w1 = choice_weight / 100;
            w2 = choice_weight % 100 / 10;
            w3 = choice_weight % 10;
            write_data(w1 + 0x30);
            write_data('.');
            write_data(w2 + 0x30);
            write_data(w3 + 0x30);
            write_string("KG", 2);
        }
        else if(key_status[2] == 3)
        {
            // ��һ�е�sum��Ҫ��Ϊ�˷�ӳ��ǰ��������
            write_com(0x88);
            write_string("SUM:", 4);
            w1 = sum / 10;
            w2 = sum % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_com(0xc0);
            write_string("WEIGHT:", 7);
            w1 = weight / 100;
            w2 = weight % 100 / 10;
            w3 = weight % 10;
            write_data(w1 + 0x30);
            write_data('.');
            write_data(w2 + 0x30);
            write_data(w3 + 0x30);
            write_string("KG", 2);

        }
        else
        {
            write_com(0x80);
            write_string("SAVING DATA...", 14);
            delay(3000); // ��ʾ3s ֻ��Ϊ�������˻������Ѻö�
            key_status[2] = 0;

        }

    }
    if(key_status[3] != 0)
    {
        if(key_status[3] == 1)
        {
            write_com(0x80);
            write_string("the last time:", 14);
            write_com(0xc0);
            //weight = iic_read(6); // �·� ������weight����Ϊ��ʡ����
            w1 = pro_month / 10;
            w2 = pro_month % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_data('-');
            //weight = iic_read(5); // �� ������weight����Ϊ��ʡ����
            w1 = pro_day / 10;
            w2 = pro_day % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_data(' ');
            //weight = iic_read(4); // Сʱ ������weight����Ϊ��ʡ����
            w1 = pro_hour / 10;
            w2 = pro_hour % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_data(':');
            //weight = iic_read(3); // ���� ������weight����Ϊ��ʡ����
            w1 = pro_minute / 10;
            w2 = pro_minute % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);

        }
        else if(key_status[3] == 2)
        {
            // ��ʱ��ʾ��ʱ�ı�׼
            write_com(0x80);
            write_string("STD: ", 5);
            write_string(" ERROR:", 7);
            //weight = iic_read(8); // ���
            w1 = choice_error / 10;
            w2 = choice_error % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_com(0xc0);
            write_string("WEIGHT:   ", 10);
            //weight = iic_read(7); // ѡ��ı�׼����
            w1 = choice_weight / 100;
            w2 = choice_weight % 100 / 10;
            w3 = choice_weight % 10;

            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_data(w3 + 0x30);
            write_string("KG", 2);
        }
        else	if(key_status[3] == 3)
        {
            // ��ʱ��ʾ��ʱ������
            write_com(0x80);
            write_string("DATA:", 5);
            write_string(" SUM:", 5);
            //weight = iic_read(1);
            w1 = sum / 10;
            w2 = sum % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
            write_com(0xc0);
            write_string("BAD:", 4);
            //weight = iic_read(2);
            w1 = bad / 10;
            w2 = bad % 10;
            write_data(w1 + 0x30);
            write_data(w2 + 0x30);
        }
        else
        {
            write_com(0x80);
            write_string("send data?", 10);
            write_com(0xc0);
            write_string("YES          NO", 15);
        }

    }

    if(key_status[7] != 0)
    {

        if(key_status[7] == 6)
        {
            write_com(0x80);
            write_string("temperature:", 12);
            write_com(0xc0);


            res = Get18B20Temp(&temp);
            if(res) // ��ȡ�ɹ�
            {
                intT = temp >> 4; //������¶�ֵ��������

                w1 = intT / 100;
                w2 = intT % 100 / 10;
                w3 = intT % 10;
                write_data(w1 + 0x30);
                write_data(w2 + 0x30);
                write_data(w3 + 0x30);
                write_data(' ');
                write_data('C');

            }
        }
        else
        {
            if(key_status[7] == 1)
            {
                write_com(0x80);
                write_string("SENDING....", 11);
            }
            else
            {
                write_com(0x80);
                write_string("SEND SUCCEESS!", 13);
                write_com(0xc0);
                write_string("PRESS AND BACK..", 16);

            }
        }


    }

}

/**************************************ʱ�䴦����*************************************************************/
void time_deal()
{

    if(adjusted.Second > 59)
    {
        adjusted.Second = adjusted.Second % 60;
        adjusted.Minute++;
    }
    if(adjusted.Minute > 59)
    {
        adjusted.Minute = adjusted.Minute % 60;
        adjusted.Hour++;
    }
    if(adjusted.Hour > 23)
    {
        adjusted.Hour = adjusted.Hour % 24;
        adjusted.Day++;
    }
    if(adjusted.Day > 31)
        adjusted.Day = adjusted.Day % 31;
    if(adjusted.Month > 12)
        adjusted.Month = adjusted.Month % 12;
    if(adjusted.Year > 100)
        adjusted.Year = adjusted.Year % 100;

    // �ɴ�����ʱ��д��DS1302
    DS1302_SetTime(DS1302_SECOND, adjusted.Second);
    DS1302_SetTime(DS1302_MINUTE, adjusted.Minute);
    DS1302_SetTime(DS1302_HOUR, adjusted.Hour);
    DS1302_SetTime(DS1302_DAY, adjusted.Day);
    DS1302_SetTime(DS1302_MONTH, adjusted.Month);
    DS1302_SetTime(DS1302_YEAR, adjusted.Year);
    delayms(1);
}


/************************************* ADת������******************************************************/
uchar weight_deal(uchar ch) // ��װ��ã�ͬʱ���ڵ������Ͷ���ļ��
{
    uchar i, dat1 = 0, dat2 = 0;

    CS  = 0;
    _nop_();
    _nop_();         					//Ƭѡʹ�ܣ��͵�ƽ��Ч
    CLK = 0;
    _nop_();
    _nop_();          					//оƬʱ������
    DIO = 1;
    _nop_();
    _nop_();
    CLK = 1;
    _nop_();
    _nop_();
    //��1���½���֮ǰ����DI=1/0
    //ѡ�񵥶�/���(SGL/DIF)ģʽ�еĵ�������ģʽ
    CLK = 0;
    DIO = 1;
    _nop_();
    _nop_();
    CLK = 1;
    _nop_();
    _nop_();
    //��2���½���֮ǰ,����DI=0/1,ѡ��CHO/CH1

    CLK = 0;

    if(ch == 0)
        DIO = 0; 	//ͨ��0 �ڲ���ѹ����
    else DIO = 1;	//ͨ��1

    _nop_();
    _nop_();

    CLK = 1;
    _nop_();
    _nop_();
    //��3���½���֮ǰ,����DI=1
    CLK = 0;
    DIO = 1;
    _nop_();
    _nop_();
    //��4-11���½��ض�����(MSB->LSB)
    for(i = 0; i < 8; i++)
    {
        CLK = 1;
        _nop_();
        _nop_();
        CLK = 0;
        _nop_();
        _nop_();
        dat1 = dat1 << 1 | DIO;
    }
    //��11-18���½��ض�����(LSB->MSB)
    for(i = 0; i < 8; i++)
    {
        CLK = 1;
        _nop_();
        _nop_();
        CLK = 0;
        _nop_();
        _nop_();
        dat2 = dat2 << ((uchar)(DIO) << i);
    }
    CS = 1;//ȡ��Ƭѡһ�����ڽ���
    //���MSB->LSB��LSB->MSB��ȡ�Ľ����ͬ,�򷵻ض�ȡ�Ľ��,���򷵻�0
    return dat1;
//	return (dat1 == dat2) ? dat1:0;//ȡ��У��


}

/*******************************************������⴦����**************************************************/
void key_deal()
{
    static uchar tmp1 = 0;
    static uchar tmp2 = 0;
    char i = 0;
    ET0 = 1;
    TR0 = 1;
    if(k1 == 0)
    {
        delayms(10);
        if(k1 == 0 && key_status[1] == 0 && key_status[2] == 0 && key_status[3] == 0 && key_status[7] == 0)
        {
            //		ֻ��ʱ��ģʽ�½����¶Ȳɼ� ���¶���ʾģʽ��

            key_status[0]++;
            if(key_status[0] == 7) {
                key_status[0] = 0;   // ��ఴ7�ο����޸���Ĵ�Сʱ��
            }
            write_com(0x01); // ���� ��ֹ�л�ģʽ����
        }

        while(!k1);
    }

    if(k2 == 0)
    {
        delayms(10);
        if(k2 == 0 && key_status[0] == 0 && key_status[2] == 0 && key_status[3] == 0 && key_status[7] == 0)
        {
            ET0 = 0;
            TR0 = 0;
            key_status[1]++;
            if(key_status[1] == 2)
            {
                // ��ʼ��  ͬʱ��ʼ��ָʾ�ƺͷ����� ��ֹbug
                key_status[1] = 0;
                led = 1;
                beep = 1;
                skin = 0;
            }
            write_com(0x01); // ���� ��ֹ�л�ģʽ����
        }

        while(!k2);
    }
    if(k3 == 0)
    {
        delayms(10);
        if(k3 == 0 && key_status[0] == 0 && key_status[1] == 0 && key_status[3] == 0 && key_status[7] == 0)
        {

            key_status[2]++;
            if(key_status[2] == 3)
            {
                ET0 = 1;
                TR0 = 1;
                // �򿪴��ڣ�ÿ�γɹ���¼�󣬾Ͱ����ݷ��͵�PC��

                skin = 0;
                sum = 0;
                bad = 0;


            }
            if(key_status[2] == 4)
            {

                ET0 = 0;
                TR0 = 0;
                led = 1; // ������Ϩ��led��
                /*ET1 = 0;
                TR1 = 0;
                ES = 0;*/
                skin = 0;
                // �����ִ洢��ȥ
                iic_write(1, sum);
                delayms(200);
                iic_write(2, bad);
                delayms(200);
                //��3-6��Ž���ʱ��ʱ�䡣
                iic_write(3, adjusted.Minute);
                delayms(200);
                iic_write(4, adjusted.Hour);
                delayms(200);
                iic_write(5, adjusted.Day);
                delayms(200);
                iic_write(6, adjusted.Month);
                delayms(200);
                iic_write(7, choice_weight);
                delayms(200);
                iic_write(8, choice_error);
                // ����ʾ�������״̬��0��


            }


            write_com(0x01); // ���� ��ֹ�л�ģʽ����
        }

        while(!k3);
    }


    if(k4 == 0)
    {
        delayms(10);
        if(k4 == 0 && key_status[0] == 0 && key_status[1] == 0 && key_status[2] == 0 && key_status[7] == 0) // �ж������������ʾʱ���ģʽ�Ļ�����ô�Ҿ���ʾ
        {
            ET0 = 0;
            TR0 = 0;
            key_status[3]++;
            switch(key_status[3])
            {
            case 1: {
                pro_month = iic_read(6);
                delayms(3);
                pro_day = iic_read(5);
                delayms(3);
                pro_hour = iic_read(4);
                delayms(3);
                pro_minute = iic_read(3);
                delayms(3);
                break;
            }
            case 2: {
                choice_error = iic_read(8);
                delayms(3);
                choice_weight = iic_read(7);
                delayms(3);
                break;
            }
            case 3: {
                sum = iic_read(1);
                delayms(3);
                bad = iic_read(2);
                break;
            }

            default:
                break;
            }
            if(key_status[3] == 5)key_status[3] = 0; // �ĸ�ģʽ ��ʾ��ʱ��ʱ�䣬Ȼ������ʾ��ʱ�Ĳ����ı�׼������׼����׼����),����ʾ������;�����ٰ���ѯ���Ƿ�Ҫ��ϸ�����ݣ�����ȷ����ť���򴮿ڷ���
            write_com(0x01); // ���� ��ֹ�л�ģʽ����
        }
    }



    if(k5 == 0) // ���ӱ�����
    {
        delay(10);
        if(k5 == 0 & (key_status[0] != 0 | key_status[1] != 0 | key_status[2] != 0))
        {
            ET0 = 0;
            TR0 = 0;
            // һ��һ��������ж�
            if(key_status[0] != 0)
            {
                switch(key_status[0])
                {
                case 1:
                    adjusted.Second++;
                    break;
                case 2:
                    adjusted.Minute++;
                    break;
                case 3:
                    adjusted.Hour++;
                    break;
                case 4:
                    adjusted.Day++;
                    break;
                case 5:
                    adjusted.Month++;
                    break;
                case 6:
                    adjusted.Year++;
                    break;
                default:
                    break;
                }
            }


            if(key_status[1] != 0)
            {
                maxweight++;
            }

            if(key_status[2] != 0)
            {
                switch(key_status[2])
                {
                case 1: {
                    if(tmp1 != 5) {
                        tmp1++;
                    }
                    choice_weight = steady_weight[tmp1];
                    break;
                }
                case 2: {
                    if(tmp2 != 2) {
                        tmp2++;
                    }
                    choice_error = error[tmp2];
                    break;
                }
                default:
                    break;
                }
            }






            while(!k5);
        }
    }

    if(k6 == 0)
    {
        delayms(10);

        if(k6 == 0 && (key_status[0] != 0 | key_status[1] != 0 | key_status[2] != 0))
        {   ET0 = 0;
            TR0 = 0;
            if(key_status[0] != 0)
            {
                switch(key_status[0])
                {
                case 1:
                    adjusted.Second--;
                    break;
                case 2:
                    adjusted.Minute--;
                    break;
                case 3:
                    adjusted.Hour--;
                    break;
                case 4:
                    adjusted.Day--;
                    break;
                case 5:
                    adjusted.Month--;
                    break;
                case 6:
                    adjusted.Year--;
                    break;
                default:
                    break;
                }
            }


            if(key_status[1] != 0)
            {
                maxweight--;
            }

            if(key_status[2] != 0)
            {
                switch(key_status[2])
                {
                case 1: {
                    if(tmp1 != 0)tmp1--;
                    choice_weight = steady_weight[tmp1];
                    break;
                }
                case 2: {
                    if(tmp2 != 0)tmp2--;
                    choice_error = error[tmp2];
                    break;
                }
                default:
                    break;
                }

            }
            while(!k6);
        }
    }

    if(k7 == 0) // ȥƤ
    {
        delayms(10);
        if(k7 == 0 && key_status[1] == 1) // ֻ���ڵ��γ�����ȥƤģʽ��������
        {
            ET0 = 0;
            TR0 = 0;
            skin = weight; // ����Ƥ������
            weight = 0; //Ƥ��ȥ����
            key_status[6]++;
            if(key_status[6] == 2)key_status[6] = 0;

            while(!k7);
        }
    }
    if(k8 == 0)
    {
        delayms(10);
        if(k8 == 0 && (key_status[3] == 4 | key_status[7] == 2))
        {
            key_status[3] = 0;
            key_status[7]++;

            if(key_status[7] == 3) {
                key_status[7] = 0;
            }

            write_com(0x01); // ���� ��ֹ�л�ģʽ����
            while(!k8);
        }
        else if(k8 == 0 && key_status[0] == 0 && key_status[1] == 0 && key_status[2] == 0 && key_status[3] == 0) // k8�ĵڶ���ģʽ��ֻ���ڰ��µ�ʱ�������ʾ�¶�
        {   // Ϊ�˵������ֳ���һ��ת̬
            if(key_status[7] == 0)key_status[7] = 6;
            else key_status[7] = 0;
            while(!k8); // ֱ��������
        }

        write_com(0x01);
    }
}



/*****************************************************������*****************************************************/
void main()
{
    uchar i = 0;

    SYSTEMTIME T;
    init();

    /*
    i = iic_read(1);

    i++;
    iic_write(1, i);
    w1 = i/10;
    w2 = i%10;
    write_com(0x80);
    write_string("HELLO :",6);
    write_data(w1+0x30);
    write_data(w2+0x30);*/
//		ES = 1;
//	TR1 = 1;
//	ET1 = 1;
    while(1)
    {


        //write_com(0xc0);
        // write_string(p2,2);
        DS1302_GetTime(&T) ;
        adjusted.Second = T.Second;
        adjusted.Minute = T.Minute;
        adjusted.Hour = T.Hour;
        adjusted.Week = T.Week;
        adjusted.Day = T.Day;
        adjusted.Month = T.Month;
        adjusted.Year = T.Year;

        for(i = 0; i < 9; i++)
        {
            adjusted.DateString[i] = T.DateString[i];
            adjusted.TimeString[i] = T.TimeString[i];
        }
        time_deal();
        key_deal();
        // ��������ж����� ����CPU��ʹ��
        if(key_status[1] != 0 | key_status[2] == 3) {
            weight = (unsigned long)(weight_deal(0) * 1.9608 - 1);

//		send_string("the times: ",10);
//		send(sum);
//		send_string("the weight: ",13);
//		send(weight);

        }
        display_deal(); //��ʾ����


//
//	if(send_flag==1)
//	{
//		send_informations();
//		send_flag=0;
//	}
        if(key_status[7] == 1)
        {
            send_informations();
        }
        if(key_status[1] != 0)
        {
            // �ж�һ���Ƿ񳬹�������̣����������Ǿͷ�������
            if((int)weight > maxweight)
            {
                led = 0; // ָʾ����
                beep = 0;
            }
            else
            {
                led = 1;
                beep = 1;
            }

        }
        delayms(10);





    }




    return;
}

/************************************************��ʱ������*******************************************************/
void t1() interrupt 1
{
    static char i = 0;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
    i++;

    if(i == 20)
    {
        i = 0; // һ����
        if(key_status[2] == 3) {
            if(weight == skin)
            {
                // ���ʱ��Ϳ���ȷ�����μ�����
                sum++;
                if(weight > choice_weight)
                {
                    if(weight - choice_weight > choice_error)bad++;
                }
                else
                {
                    if(choice_weight - weight > choice_error)bad++;
                }
                iic_write(0xff - sum, weight); // �����������¼���ż�¼��24C02��
                led = ~led; // ��תָʾ


            }
            skin = weight; // ����skin�������������洢1sǰ������
        }
        else
        {
            Start18B20();
        }
    }

}
