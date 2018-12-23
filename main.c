/*
 * main.c
 *
 *  Created on: Dec 18, 2018
 *      Author: qianlin
 */

#include "msp430.h"
#include "definition.h"

unsigned long number = 0;        //记录响应按键次数
int flag = 0;
unsigned int time = 0, subtime = 0;
int j=0;
char buffer[20];
//const char head=0xff;
//const char conf=0xfe;
//char right_input[]="RIGHT";
//char clear_alert[]="CLEAR";

void USARTA0_init();
void reset();

void delay(unsigned int i)     //延时函数
{
  unsigned int j;             //定义局部变量
  for (j = 0; j < i; j++);
}

int main(void) {
  WDTCTL = WDTPW + WDTHOLD;       // stop watchdog
  __disable_interrupt();      //_DINT(); 禁止总中断
  P1SEL &= ~BIT0;
  P1SEL2 &= ~BIT0;
  P1DIR |= BIT0;
  P1OUT |= BIT0;

  P2DIR &= ~INTER;       //P2.2 as input (interrupt source), P2.1 as output (buzz)
  P2DIR |= LED;
  P2SEL &= ~(LED+INTER);
  P2SEL2 &= ~(LED+INTER);
  P2OUT |= (LED+INTER); // set as pull-up resistor
  P2REN |= INTER; // enable resistor
  P2DIR &= ~INTER;
  P2IES &= ~INTER; // up edge interrupt
  P2IFG &= ~INTER;
  P2IE |= INTER;

  USARTA0_init();
  IE2 |= UCA0RXIE;  // enable usart interrupt

  flag = 0;
  __enable_interrupt();   //_EINT(); 总中断运行
  while (1) {
    if (flag == 1){
      while((IFG2&UCA0TXIFG)==0);
      UCA0TXBUF = head;
      while((IFG2&UCA0TXIFG)==0);
      UCA0TXBUF = conf;
      while((IFG2&UCA0TXIFG)==0);
      UCA0TXBUF = head;
      while((IFG2&UCA0TXIFG)==0);     //检测发送缓冲是否空
      UCA0TXBUF=number;       // send number
      flag = 2;
      P2OUT &= ~LED1;
    }
    if (flag == 2){
      subtime++;
      if (subtime >= SubTimeLimit){
        subtime = 0;
        time++;
      }
      // Time check. If > timeLimit, light the second light and send report
      if (time >= TimeLimit){
        P2OUT &= ~LED2;
        flag = 3;
        while((IFG2&UCA0TXIFG)==0);
        UCA0TXBUF = head;
        while((IFG2&UCA0TXIFG)==0);
        UCA0TXBUF = conf;
        while((IFG2&UCA0TXIFG)==0);
        UCA0TXBUF = conf;
//        while((IFG2&UCA0TXIFG)==0);     //检测发送缓冲是否空
//        UCA0TXBUF = head;       //取一个数据发送
      }
      // Password check
      if (j==5){
        __disable_interrupt();
        int right=0;
        for (j=0;j<6;j++){
          if (buffer[j] != right_input[j])
            right=1;
        }
        j=0;
        if (right == 0){
          reset();
          P2OUT |= LED1;
        }
        __enable_interrupt();
      }
    }
    // Can reset any time
    if (flag>0 && j==5){
      __disable_interrupt();
      int clear=0, alert=0;
      for (j=0;j<6;j++){
        if (buffer[j] != clear_alert[j])
          clear=1;
        if (buffer[j] != wrong_input[j])
          alert=1;
      }
      j=0;
      if (alert == 0){
        flag = 3;
        P2OUT &= ~LED;
      }
      if (clear == 0){
        reset();
        P2OUT |= LED;
      }
      __enable_interrupt();
    }
  };
}

#pragma  vector=PORT2_VECTOR
__interrupt void port2_infrared(void) {
  if ((P2IFG & BIT2) != 0) {
    number++;
    flag = 1;
  }
  P2IFG &= ~INTER;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void usart_received(void){
  buffer[j] = UCA0RXBUF;
  j++;
}

void  USARTA0_init()
{   UCA0CTL1|= UCSWRST;         //swrst=1
    P1SEL |= BIT1+BIT2;         //收、发管脚功能设置
    P1SEL2 |= BIT1+BIT2;        //
//数据格式选用上电复位设置：无奇偶校验，8位数据，1个停止位，异步串行通信
    UCA0CTL1|=UCSSEL1+UCRXEIE; //波特率时钟选择SMCLK，不论对错均收
    UCA0BR0 = 114;              //波特率9600设置
    UCA0BR1 = 0;
    UCA0MCTL=UCBRF_3;
    UCA0CTL1 &=~UCSWRST;        //swrst=0
}

void reset(){
  flag = 0;
  time = 0;
  subtime = 0;
}

