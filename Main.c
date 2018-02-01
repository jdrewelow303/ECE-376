/* 
 * File:   Main.c
 * Author: James Drewelow
 *
 * Created on December 4, 2016, 7:44 PM
 */
#include <pic18.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD_PortD.c"

#define COUNT_MAX   255
#define COUNT_2_MAX 10

//Global variables
unsigned int R1, G1, B1, R2, G2, B2 = 1; // LED variables (LED2 cycles)
unsigned int count, count2 = 0;
unsigned int score1, score2 = 0; //player's scores
unsigned int foul, P1in, P2in, roundNum = 0; //variables to keep track of end of rounds
const unsigned char MSG0[16] = "Player 1:       "; 
const unsigned char MSG1[16] = "Player 2:       "; 
const unsigned char MSG2[16] = "   Press  RB4   "; 
const unsigned char MSG3[16] = "  To begin game "; 
const unsigned char MSG4[16] = " Player 1 Wins! ";
const unsigned char MSG5[16] = " Player 2 Wins! ";
const unsigned char MSG6[16] = " RB4 to Restart ";

//Subroutines



unsigned int A2D_Read(unsigned char c)
{
   //unsigned int result; un-needed?
   unsigned char i;
   c = c & 0x0F;
   ADCON0 = (c << 2) + 0x01; // set Channel Select
   for (i=0; i<20; i++); // wait 2.4us (approx)
   GODONE = 1; // start the A/D conversion
   while(GODONE); // wait until complete (approx 8us)
   return(ADRES);
   }   

void LCD_Out(unsigned int DATA)
{
   unsigned char A[4], i;
   
   for (i=0; i<4; i++) {
      A[i] = DATA % 10;
      DATA = DATA / 10;
   }
   for (i=4; i>0; i--) {
      LCD_Write(A[i-1]+48);
   }
}

// Interrupt Service Routine
void interrupt IntServe(void)
{
   if (TMR0IF) {
      TMR0 = -1300;
      if (count == COUNT_MAX){
          count = 0;
           RC0 = 1;
           RC1 = 1;
           RC2 = 1;
           count2++;
           if (count2 == COUNT_2_MAX){
               
               count2 = 0;
           }
      }
      if (count == R2 )
      {
          RC0 = 0;
      }
      if (count == G2)
      {
          RC1 = 0;
      }
      if (count == B2)
      {
          RC2 = 0;
      }
      count++;
      
      TMR0IF = 0;
    }

   }

void RandomColor()
    {     
            R1 = rand()%256;
            G1 = rand()%256;
            B1 = rand()%256;
    }

void Score(int playerSelect)
{
    if(playerSelect == 1)
    {
        score1 = score1 + abs(R1-R2) + abs(G1-G2) + abs(B1-B2);
    }
    else if(playerSelect == 2)
    {
        score2 = score2 + abs(R1-R2) + abs(G1-G2) + abs(B1-B2);
    }
}

void Start()
{
    RandomColor();
    LCD_Move(0,0);  for (int i=0; i<16; i++) LCD_Write(MSG0[i]);
    LCD_Move(1,0);  for (int i=0; i<16; i++) LCD_Write(MSG1[i]);
    roundNum = 0;
    score1 = 0;
    score2 = 0;
}


void main(void) {     
 TRISB = 0xFF;
 TRISC = 0;
 TRISD = 0;
 PORTC = 0;
 
 // Initialize the A/D port
   TRISA = 0xFF;
   TRISE = 0x0F;
   ADCON2 = 0x85;
   ADCON1 = 0x07;
   ADCON0 = 0x01;


LCD_Init();   // initialize the LCD
LCD_Move(0,0);  for (int i=0; i<16; i++) LCD_Write(MSG2[i]);
LCD_Move(1,0);  for (int i=0; i<16; i++) LCD_Write(MSG3[i]);


   while(!RB4)
{     
}

//Interrupts
 // set up Timer0 for PS = 1

   T0CS = 0;
   T0CON = 0x88;
   TMR0ON = 1;
   TMR0IE = 1;
   TMR0IP = 1;
   PEIE = 1;
// set up Timer1 for PS = 1
   //T1CON = 0x81;
   //TMR1ON = 1;
   //TMR1IE = 1;
   //TMR1IP = 1;
   //PEIE = 1;
   //TMR1CS = 0;

// turn on all interrupts
   GIE = 1;

   
srand(A2D_Read(0));
Start();

   while(1)
   {
  
       LCD_Move(0, 10); LCD_Out(score1);
       LCD_Move(1, 10); LCD_Out(score2);
       Wait_ms(1000);
       
       while(P1in + P2in != 2){
       if(RB0)
       {
           P1in = 1;
           Score(1);
       }
       if(RB7)
       {
           P2in = 1;
           Score(2);
       }
       if(foul == 1 && !P1in)
       {
           score1 = score1 + 765; // add max score for foul
       }
       if(foul == 1 && !P2in)
       {
           score2 = score2 + 765; // add max score for foul
       }


                    }
       P1in = 0;
       P2in = 0;
       roundNum++;

       while(roundNum == 3)
       {
           while(!RB4){

                   if(score1 < score2)
                   {
                       LCD_Move(0,0);  for (int i=0; i<16; i++) LCD_Write(MSG4[i]);
                       LCD_Move(1,0);  for (int i=0; i<16; i++) LCD_Write(MSG6[i]);
                   }
                   if(score2 < score1)
                   {
                       LCD_Move(0,0);  for (int i=0; i<16; i++) LCD_Write(MSG5[i]);
                       LCD_Move(1,0);  for (int i=0; i<16; i++) LCD_Write(MSG6[i]);
                   }

              }
           Start();
       }



   } 
   
}

