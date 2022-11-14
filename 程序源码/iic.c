
#include "reg52.h"
#include "intrins.h"
#include "iic.h"

void delay7us()		//��ʱ7us
{   _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}

void start() //��ʼ�źţ���ʱ�����ߵ�ƽ�仯��ʱ�Ӹߵ�ƽʱ���½�����Ч
{   SCL = 1; //ʱ�������ߣ�
    delay7us();
    SDA = 1; //����������
    delay7us(); //��ʱ����ʱӦ����Ч��Χ�ڱ��� >4.7us
    SDA = 0;	//����
    delay7us(); //��ʱ����ʱӦ����Ч��Χ�ڱ��� >4us
}

void stop()//��ֹ�ź�	  ��ʱ�����ߵ�ƽ�仯��ʱ�Ӹߵ�ƽʱ������������Ч
{   //SDA=0;
    //delay7us();
    /// SCL=1;
    //delay7us();
    // SDA=1;
    //delay7us();
    SDA = 0; // �������ȵ�
    delay7us();
    SCL = 1; //	ʱ�����ٸ�
    delay7us();  //��ʱ����ʱӦ����Ч��Χ�ڱ��� >4us��SDA�ڸ�
    SDA = 1;	 //����������
    delay7us();//��ʱ����ʱӦ����Ч��Χ�ڱ��� >4.7us
}

void response()//Ӧ���źţ�"0"
{   uchar i;
    SCL = 1;	 //ʱ�������ߣ�
    delay7us(); ///��ʱ����ʱӦ����Ч��Χ�ڱ��� >4us
    while((SDA == 1) && (i < 200))i++; //�ȴ�Ӧ���� SCL>4us ���ж� SDA�Ƿ�Ϊ��0����Ϊ��0����ʾӦ��
    //��i<200�ڣ�δ�յ�Ӧ���źţ���Ĭ���յ�Ӧ�𣬲��˳���
    SCL = 0; //ʱ��������
    delay7us();
}

void noack() //��Ӧ���źţ���1����Ӧ��Ҳ�ɲ������ж�
{   uchar i;
    //SDA=1;
    // delay7us();
    SCL = 1;
    delay7us();//��ʱ>4us,
    while((SDA != 1) && (i < 100))i++; //�� SCL>4us ���ж� SDA�Ƿ�Ϊ��1����Ϊ��1����ʾ��Ӧ��
    // ͬ�ϣ�
    SCL = 0;
    delay7us();
    //SDA=0;
}

void init_iic() //SCL,SDA��ʼ��
{   SCL = 1;
    SDA = 1;
}

void write_byte(uchar date)	//дһ�ֽ�
{   uchar i;
    /*      for(i=0;i<8;i++)
    {
    	date=date<<1;//��λ����λ����CY��
    	SCL=0;
        delay7us();
    	SDA=CY;
    	delay7us();
    	SCL=1;
    	delay7us();
      }

      SCL=0;
      delay7us();
      SDA=1;
      delay7us(); */

    SCL = 0;	//SCL����׼��д�����ݣ�SCL�͵�ƽʱ����SDA���ݱ仯��д�룩��SCL�ߵ�ƽʱ����Ӧ�����ȶ�
    delay7us();	//��ʱ
    for(i = 0; i < 8; i++)	 //ѭ��8��д��һ���ֽ�
    {
        if(date & 0x80) //��λ��ǰ����д��λ
        {
            SDA = 1;   //д�� 1
        }
        else
        {
            SDA = 0;   //д�� 0
        }
        SCL = 1; //SCL����
        delay7us();	//��ʱ
        date = date << 1; //��λ��׼����һλ����
        SCL = 0; //SCL���ͣ�׼��д��һλ
        delay7us(); //
    }
    SDA = 1; //�ͷ�SDA������,�Ա�����Ӧ���źš� 0 ����
    delay7us();//
}

uchar read_byte() //��һ���ֽڣ�
{   uchar i, k;
    SCL = 0;	 //
    delay7us();  //
    SDA = 1;	 //�ͷ�SDA���Ա�ͨ��
    delay7us();  //
    for(i = 0; i < 8; i++) //ѭ��8�ζ���һ���ֽ�
    {   SCL = 1; //SCLΪ1ʱ��SDA����Ӧ�����ȶ�
        delay7us();
        //k=(k<<1)|SDA;
        k = k << 1; //��λ
        if(SDA) //���SDA��ĳλ���ݣ��Ƿ�Ϊ1���ȶ���λ
            k++;	  //���Ϊ 1 �� k++д��1,�൱�� k �ĵ�0λ����һ�� 1���´ν���ʱ����Ϊk������һλ
        // �������˵�1λ��8�ξͽ���λ�����8λ������λͬ��
        SCL = 0; //SCL�͵�ƽʱ����SDA���ݱ仯���������ݶ���
        delay7us();//��ʱ
    }
    return k; //����
}

void iic_write(uchar add, uchar date) //д����
{   //init_iic();
    start(); //�ȷ���ʼ�źţ�����֮ǰ����
    write_byte(0xa0);//������ַ��1010 A0 A0 A0(000) R/W(0)�������ʸ�оƬ�ĵ�ַ��
    //ǰ4λΪ�̶���ַ������ʱ������3λ�û��Զ����ַ�����λ"0"��ʾд�룬
    response(); //��Ӧ���ź�
    write_byte(add); //д���ַ��������д��ĵ�ַ�������ַ��
    response(); //��Ӧ���ź�
    write_byte(date);//д�������
    response(); //��Ӧ���ź�
    stop(); //����ֹ�ź�

}
uchar iic_read(uchar add) //������
{   uchar dat;
    start(); //�ȷ���ʼ�źţ�����֮ǰ����
    write_byte(0xa0);//���ʸ�оƬ�ĵ�ַ��������ַ��
    response(); //��Ӧ���ź�
    write_byte(add);// ���ʸ�оƬ�ڲ������ݵ�ַ��д�����ݵĵ�ַ��
    response();//��Ӧ���ź�
    start();  //�ȷ���ʼ�źţ���ʼ����
    write_byte(0xa1);//������ַ��1010 A0 A0 A0(000) R/W(1)�������ʸ�оƬ�ĵ�ַ��
    //ǰ4λΪ�̶���ַ������ʱ������3λ�û��Զ����ַ�����λ"1"��ʾ����
    response();//��Ӧ���ź�
    dat = read_byte(); //��
    noack();//��Ӧ�𣬶���������Ӧ��
    stop();//��ֹ����
    return dat;//��������
}