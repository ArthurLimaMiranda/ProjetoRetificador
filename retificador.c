#include <lcd.h>
#include <REG8253.H>
#include <ADCFuncoes.h>
#include < stdio.h >

unsigned char Tecla, ValorADC[4];
int interCount = 0, modo = 0, ass = 1, overflow_count = 0, i=0, Vadcaux = 0, posicao = 0, casa1 = 0, casa2 = 0, casa3 = 0, casa4 = 0, inter = 0;
int fase = 0;

//INICIA O DISPLAY LCD//
Lcd4_init();
//--------------------//

//CONEXÕES DO MÓDULO LDC//
sbit RS = P2 ^ 0;
sbit EN = P2 ^ 1;
sbit D4 = P2 ^ 2;
sbit D5 = P2 ^ 3;
sbit D6 = P2 ^ 4;
sbit D7 = P2 ^ 5;
//----------------------//

//FUNÇÃO PARA CONVERSÃO DO VALOR LIDO DE CHAR PARA FLOAT//
void CGeraADC(float Vadc) {
  Vadcaux = (int)(Vadc * 1000);
  ValorADC[3] = Vadcaux / 1000;
  ValorADC[2] = (Vadcaux % 1000) / 100;
  ValorADC[1] = ((Vadcaux % 1000) % 100) / 10;
  ValorADC[0] = ((Vadcaux % 1000) % 100) % 10;
}
//------------------------------------------------------//

//FUNÇÃO PARA O USO DO TECLADO MATRICIAL//
char VarreLinha(char Coluna) {
  char retornaTecla = 0;
  switch (Coluna) {
  case 1:
    if (P0_4 == 0) retornaTecla = '1';
    if (P0_5 == 0) retornaTecla = '4';
    if (P0_6 == 0) retornaTecla = '7';
    if (P0_7 == 0) retornaTecla = '*';
    break;
  case 2:
    if (P0_4 == 0) retornaTecla = '2';
    if (P0_5 == 0) retornaTecla = '5';
    if (P0_6 == 0) retornaTecla = '8';
    if (P0_7 == 0) retornaTecla = '0';
    break;
  case 3:
    if (P0_4 == 0) retornaTecla = '3';
    if (P0_5 == 0) retornaTecla = '6';
    if (P0_6 == 0) retornaTecla = '9';
    if (P0_7 == 0) retornaTecla = '#';
    break;

  case 4:
    if (P0_4 == 0) retornaTecla = '+';
    if (P0_5 == 0) retornaTecla = '-';
    if (P0_6 == 0) retornaTecla = '*';
    if (P0_7 == 0) retornaTecla = '/';
    break;
  default:
    break;
  }
  return retornaTecla;
}

char LerTeclado() {
  char i;
  unsigned char TeclaLida = 0;
  P0 = 0xff;
  for (i = 1; i < 5; i++) 
  {
    switch (i) {
    case 1:
      P0_0 = 0;
      if (TeclaLida == 0) TeclaLida = VarreLinha(i);
      P0_0 = 1;
      break;
    case 2:
      P0_1 = 0;
      if (TeclaLida == 0) TeclaLida = VarreLinha(i);
      P0_1 = 1;
      break;
    case 3:
      P0_2 = 0;
      if (TeclaLida == 0) TeclaLida = VarreLinha(i);
      P0_2 = 1;
      break;
    default:
      break;
    }
  }
  return TeclaLida;
}
//--------------------------------------//

//FUNÇÃO DE DELAY//
void Delay(int a){
    int j, i;   
    for(i=0;i<a;i++){
        for(j=0;j<100;j++){
        }
    }
}


//---------------//

//INTERRUPÇÃO EXTERNA, RESPONSÁVEL PELA GERAÇÃO DA SINCRONIZAÇÃO COM A REDE//
void TrataIntExt0(void) interrupt 0 {

  if (P0_6 == 0) {
    Lcd4_Clear();
    P3_1 = 0;
    modo = 0;
    inter = 0;
    EX0 = 0;
  }
  fase = 0;
  P2_6 ^= 1;
  TR0 = 1;

}
//-------------------------------------------------------------------------//

//INTERRUPÇÃO DO TIMER, RESPONSÁVEL PELA GERAÇÃO DOS PULSOS TRIFÁSICOS//
void timer0_ISR(void) interrupt 1 {
  
  if (fase == 0) {
    if (ass == 1) {

      TH0 = 0x0ff;
      TL0 = 0x038;

      if (overflow_count++ == (22)) {

        TR0 = 0;
        P2_7 = 0;
        fase++;
        overflow_count = 0;
        TR0 = 1;
      }
      if ((overflow_count > posicao) && (overflow_count < (posicao + 4))) {
        P2_7 = 1;
      } 
	  else {
        P2_7 = 0;
      }
    }
  }
  if (fase == 1) {
    if (ass == 1) {

      TH0 = 0x0ff;
      TL0 = 0x038;

      if (overflow_count++ == (22)) {

        TR0 = 0;
        fase++;
        overflow_count = 0;
        TR0 = 1;
      }
      if ((overflow_count > posicao) && (overflow_count < (posicao + 4))) {
        P3_4 = 1;
      } 
	  else {
        P3_4 = 0;
      }
    }
  }
  if (fase == 2) {
    if (ass == 1) {

      TH0 = 0x0ff;
      TL0 = 0x038;

      if (overflow_count++ == (22)) {

        TR0 = 0;
        fase++;
        overflow_count = 0;
      }
      if ((overflow_count > posicao) && (overflow_count < (posicao + 4))) {
        P3_5 = 1;
      } 
	  else {
        P3_5 = 0;
      }
    }
  }
}
//--------------------------------------------------------------------//


void main(void) {
  float exemplo = 0;
  int tensaozinha = 0;
  int mande = 0;
  TMOD = 0x01; // (TMOD & 0xF0) | 0x01; /* Configira modo */
  EA = 1; /* Habilitaçãode interrupções */

  ET0 = 1; /* Habilita Interrupção do Timer 0 */
  IT0 = 1; // Conf , Sensibilida e Transição
  TH0 = 0x0ff;
  TL0 = 0x018;

  Lcd4_init();
  Lcd4_Clear();
  while (1) {
    if ((modo == 0) && (inter != 1)) {

      exemplo = readadc(1, 0);
      CGeraADC(exemplo);
      Lcd4_Set_Cursor(1, 0);
      Lcd4_Write_String("Vt=");
      Lcd4_Write_Char(ValorADC[3] + 0x30);

      Lcd4_Write_Char(',');
      Lcd4_Write_Char(ValorADC[2] + 0x30);
      Lcd4_Write_Char(ValorADC[1] + 0x30);
      Lcd4_Write_String("V|Opt=P37");

      Lcd4_Set_Cursor(2, 0);
      Lcd4_Write_String("RETIF_CONTROLADO");

      if (inter == 1) {
        EX0 = 1;
      }
      if (P3_7 == 0) {
        modo = 1;
        Lcd4_Clear();
      }
    }

    if (modo == 1) {
      Lcd4_Set_Cursor(1, 0);
      Lcd4_Write_String("Ajuste_Vout   =1");

      Lcd4_Set_Cursor(2, 0);
      Lcd4_Write_String("Config_Disparo=2");

      if (LerTeclado() == '1') {
        modo = 2;
        Lcd4_Clear();
      } else if (LerTeclado() == '2') {
        modo = 3;
        Lcd4_Clear();
      }
    }
    if (modo == 2) {
      Lcd4_Set_Cursor(1, 1);
      Lcd4_Write_String("VALOR_MEDIO =1");

      Lcd4_Set_Cursor(2, 1);
      Lcd4_Write_String("VALOR_EFICAZ =2");

      if (LerTeclado() == '1') {
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 1);
        Lcd4_Write_String("INSIRA VAL MED:");
        Lcd4_Set_Cursor(2, 1);
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa1 = mande;
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa2 = mande;
        Delay(2000);
        Lcd4_Write_Char(',');
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa3 = mande;
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa4 = mande;
        Delay(2000);;
        tensaozinha = ((((casa1 - 0x30) * 1000) + ((casa2 - 0x30) * 100) + ((casa3 - 0x30) * 10) + ((casa4 - 0x30))) / 100);
        switch (tensaozinha) {
        case 0:
          posicao = 22;
          break;
        case 1:
          posicao = 20;
          break;
        case 2:
          posicao = 19;
          break;
        case 3:
          posicao = 13;
          break;
        case 4:
          posicao = 4;
          break;
        case 5:
          posicao = 2;
          break;
        default:
          posicao = 0;

        }
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 0);
        Lcd4_Write_String("Vt=");
        Lcd4_Write_Char(casa1);
        Lcd4_Write_Char(casa2);
        Lcd4_Write_Char(',');
        Lcd4_Write_Char(casa3);
        Lcd4_Write_String("V|VLT=P06");

        Lcd4_Set_Cursor(2, 0);
        Lcd4_Write_String("RETIF_CONTROLADO");
        Delay(1000);
        modo = 0;
        inter = 1;
        EX0 = 1;

      } 
	  else if (LerTeclado() == '2') {
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 1);
        Lcd4_Write_String("INSIRA VAL EFI:");
        Lcd4_Set_Cursor(2, 1);
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa1 = mande;
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa2 = mande;
        Delay(2000);
        Lcd4_Write_Char(',');
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa3 = mande;
        Delay(2000);
        mande = LerTeclado();
        Lcd4_Write_Char(mande);
        casa4 = mande;
        Delay(2000);;
        tensaozinha = ((((casa1 - 0x30) * 1000) + ((casa2 - 0x30) * 100) + ((casa3 - 0x30) * 10) + ((casa4 - 0x30))) / 100);
        switch (tensaozinha) {
        case 0:
          posicao = 22;
          break;
        case 1:
          posicao = 21;
          break;
        case 2:
          posicao = 19;
          break;
        case 3:
          posicao = 17;
          break;
        case 4:
          posicao = 14;
          break;
        case 5:
          posicao = 12;
          break;
        case 6:
          posicao = 9;
          break;
        case 7:
          posicao = 5;
          break;
        default:
          posicao = 0;

        }
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 0);
        Lcd4_Write_String("Vt=");
        Lcd4_Write_Char(casa1);
        Lcd4_Write_Char(casa2);
        Lcd4_Write_Char(',');
        Lcd4_Write_Char(casa3);
        Lcd4_Write_String("V|VLT=P06");

        Lcd4_Set_Cursor(2, 0);
        Lcd4_Write_String("RETIF_CONTROLADO");
        Delay(1000);
        modo = 0;
        inter = 1;
        EX0 = 1;
      }
    }
    if (modo == 3) {

      Lcd4_Set_Cursor(1, 0);
      Lcd4_Write_String("SINGLE_PULSE =1");

      Lcd4_Set_Cursor(2, 0);
      Lcd4_Write_String("TREM_DE_PULSO=2");

      if (LerTeclado() == '1') {
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 1);
        Lcd4_Write_String("SINGLE PULSE");
        Delay(1000);
        modo = 0;
        ass = 1;
        Lcd4_Clear();
      } 
	  else if (LerTeclado() == '2') {
        Lcd4_Clear();
        Lcd4_Set_Cursor(1, 1);
        Lcd4_Write_String("TREM_DE_PULSO");
        Delay(1000);
        modo = 0;
        ass = 2;
        Lcd4_Clear();

      }
    }
  }
}