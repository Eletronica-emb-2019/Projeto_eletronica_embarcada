#include <msp430g2553.h>
#include <intrinsics.h>
#define TEMP_IN      BIT4
#define RX BIT1
#define TX BIT2



#define BAUD_9600   0
#define BAUD_19200  1
#define BAUD_38400  2
#define BAUD_56000  3
#define BAUD_115200 4
#define BAUD_128000 5
#define BAUD_256000 6
#define NUM_BAUDS   7

#define IN1     BIT0
#define IN2     BIT1
#define IN3     BIT2
#define IN4     BIT5


int i=0, j=0, a = 0, counter = 0, cond1 = 0, k = 0;
int temp_init = 0, temperature = 0, temperature2 = 0;
int cond4 = 0;
char str[20];

void atraso(volatile unsigned int T_ms)
{
    TACCR0 = 999;
    TACTL = TACLR;
    TACTL = TASSEL_2 + ID_0 + MC_1;
    while(T_ms>0)
    {
        while((TACTL&TAIFG)==0);
        T_ms--;
        TACTL &= ~TAIFG;
    }
    TACTL = MC_0;
}


int Read_String(char str[], int strlen);
char Receive_Data(void);
void Send_Data(unsigned char c);
void Send_Int(int n);
void Send_String(char str[]);
void Init_UART(unsigned int baud_rate_choice);
//void Atraso_ms(volatile unsigned int ms);
int cmp_str(char str1[], char str2[]);







int main(void)
{

    WDTCTL = WDTPW + WDTHOLD;

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P2DIR = (IN1 + IN2 + IN3 + IN4);
    P1OUT = 0;
    //P2DIR = BIT0 + BIT1;
    P2OUT = 0;
    P1DIR = BIT6;
    P1DIR &= ~TEMP_IN;  // define sensor como entrada
    Init_UART(BAUD_9600);
    TA1CCR0 = 12500-1;
    TA1CTL = TASSEL_2 + ID_3 + MC_1;


    ADC10CTL0 = SREF_0|ADC10SHT_2|ADC10ON; // declarando conversor
    ADC10CTL1 = INCH_4|SHS_0|ADC10DIV_0|ADC10SSEL_0|CONSEQ_0;
    ADC10AE0 = BIT4;//selecionando p1.4
    ADC10CTL0 |= ENC;


    //TACCTL0 = CCIE;
   // TACTL = TASSEL_1|ID_3|MC_3|TAIE;//  100 mS
    //TACCR0 = 160; // 1MHz/(8*6250*2) = 10Hz = 1 interrupção a cada 100mS
                //fim do seTUP




    IE2 |= UCA0RXIE;
    //P1IES = RX;
  //  P1IFG = 0x00;
    _enable_interrupts();
    __low_power_mode_1();
  while(1)
   {


      }


   //return 0;
}





int Read_String(char str[], int strlen)
{
    int i = 0;
    do
    {
        // Salva o caractere recebido
        str[i] = Receive_Data();
        // Ignora o '\r'
        if(str[i]!='\r')
        {
            // Troca '\n' por '\0'
            // e termina a recepção
            if(str[i]=='\n')
            {
                str[i] = '\0';
                break;
            }
            i++;
        }
    } while(i<strlen);

    // Se chegou ao final do vetor,
    // termina-o com '\0'
    if(i==strlen)
    {
        i--;
        str[i] = '\0';
    }
    // Retorna o tamanho da string
    // sem contar o '\0'
    return i;
}

char Receive_Data(void)
{
    while((IFG2&UCA0RXIFG)==0);
    return UCA0RXBUF;
}

void Send_Data(unsigned char c)
{
    while((IFG2&UCA0TXIFG)==0);
    UCA0TXBUF = c;
}

void Send_Int(int n)
{
    int casa, dig;
    if(n==0)
    {
        Send_Data('0');
        return;
    }
    if(n<0)
    {
        Send_Data('-');
        n = -n;
    }
    for(casa = 1; casa<=n; casa *= 10);
    casa /= 10;
    while(casa>0)
    {
        dig = (n/casa);
        Send_Data(dig+'0');
        n -= dig*casa;
        casa /= 10;
    }
}

void Send_String(char str[])
{
    int i;
    for(i=0; str[i]!= '\0'; i++)
        Send_Data(str[i]);
}

void Init_UART(unsigned int baud_rate_choice)
{
    unsigned char BRs[NUM_BAUDS] = {104, 52, 26, 17, 8, 7, 3};
    unsigned char MCTLs[NUM_BAUDS] = {UCBRF_0+UCBRS_1,
                                        UCBRF_0+UCBRS_0,
                                        UCBRF_0+UCBRS_0,
                                        UCBRF_0+UCBRS_7,
                                        UCBRF_0+UCBRS_6,
                                        UCBRF_0+UCBRS_7,
                                        UCBRF_0+UCBRS_7};
    if(baud_rate_choice<NUM_BAUDS)
    {
        // Habilita os pinos para transmissao serial UART
        P1SEL2 = P1SEL = RX+TX;
        // Configura a transmissao serial UART com 8 bits de dados,
        // sem paridade, comecando pelo bit menos significativo,
        // e com um bit de STOP
        UCA0CTL0 = 0;
        // Escolhe o SMCLK como clock para a UART
        UCA0CTL1 = UCSSEL_2;
        // Define a baud rate
        UCA0BR0 = BRs[baud_rate_choice];
        UCA0BR1 = 0;
        UCA0MCTL = MCTLs[baud_rate_choice];
    }
}


int cmp_str(char str1[], char str2[])
{
    int i;
    for(i=0; (str1[i]!='\0')&&(str2[i]!='\0'); i++)
    {
        if(str1[i]!=str2[i])
            return 0;
    }
    if(str1[i]!=str2[i])
        return 0;
    else
        return 1;
}



char BTIN(void)
    {
        while((IFG2&UCA0RXIFG)==0);
        return UCA0RXBUF;
    }






void abertura (void){
          Send_String("Abrindo Valvula\r\n");
          cond1 = 1;
             for (a = 0; a <2048; a++){
                 switch (counter)
                                     {
                                     case 0:
                                         P2OUT = IN1|IN2;
                                         counter ++;
                                         break;
                                     case 1:
                                         P2OUT = IN3|IN2;
                                         counter ++;
                                         break;
                                     case 2:
                                         P2OUT = IN3|IN4;
                                         counter ++;
                                         break;
                                     case 3:
                                         P2OUT = IN4|IN1;
                                         counter = 0;
                                         break;
                                     //default:
                                     }
                // Send_String("Aberto\r\n");
                 atraso(2);
                 }
             P2OUT = 0;
           /*  temp_init = 0;

                             for (i = 0; i<20; i++){
                             ADC10CTL0 |= ADC10SC; // converte  A/D
                             while (ADC10CTL1 & BUSY); // espera terminar conversão
                             temperature = ADC10MEM*355/104; // salva dado convertido em AD
                             temp_init= temp_init + temperature;
                             atraso(100);
                             }
                                  //P1OUT ^= BIT6;
                                 temp_init = temp_init/20;
                               Send_String("Temperatura Inicial: ");
                               Send_Int(temp_init);
                               Send_String("\r\n");*/
            // TA1CCTL0 = CCIE;

         }







void fechamento (void){
        Send_String("Fechando Valvula\r\n");
        cond1 = 0;

       cond4 = 0;
          for (a = 0; a <2048; a++){
                      switch (counter)
                           {
                               case 0:
                                  P2OUT = IN1|IN2;
                                     counter ++;
                                      break;
                                case 1:
                                P2OUT = IN1|IN4;
                                       counter ++;
                                      break;
                                  case 2:
                          P2OUT = IN3|IN4;

                                      counter ++;
                                     break;
                                  case 3:
                                   P2OUT = IN3|IN2;
                                   counter = 0;
                                   break;
                                              }
                      atraso(2);
              }
          P2OUT = 0;
         // TA1CCTL0 &= ~CCIE;

      }



void leitura(void)
{

      _enable_interrupts();
    while(cond1 == 1){
        if(cond4 == 0){
          cond4 = 1;
          temp_init= 0;
          for (i = 0; i<20; i++){
          ADC10CTL0 |= ADC10SC; // converte  A/D
          while (ADC10CTL1 & BUSY); // espera terminar conversão
          temperature = ADC10MEM*355/1040.0; // salva dado convertido em AD
          Send_String("Temperatura ponto a ponto: ");
                    Send_Int(temperature);
                    Send_String("\r\n");
          temp_init = temp_init + temperature;
          atraso(100);
        }


          temp_init = temp_init/20;
          Send_String("Temperatura Inicial: ");
          Send_Int(temp_init);
          Send_String("\r\n");
        }



        else{

        temperature2 = 0;

      for (i = 0; i<20; i++){
         ADC10CTL0 |= ADC10SC; // converte  A/D
         while (ADC10CTL1 & BUSY); // espera terminar conversão
         temperature = ADC10MEM*355/1040.0; // salva dado convertido em AD
         temperature2 = temperature2 + temperature;
        atraso(100);
    }
                                  //P1OUT ^= BIT6;
    temperature2 = temperature2/20;
    Send_String("Temperatura Atual: ");
    Send_Int(temperature2);
    Send_String("\r\n");
    if(fabs(temperature2 - temp_init)>10){
                 fechamento();

                } }}}









#pragma vector=USCIAB0RX_VECTOR;
    __interrupt void USCI0RX_ISR(void)
    {
    __low_power_mode_off_on_exit();
    P1OUT ^= BIT6;
    Read_String(str, 20);

    if((cmp_str(str, "Fechar")) && (cond1 == 1))
    {
        //TA1CTL &= ~TAIE;
        fechamento();

        Send_String("Valvula Fechada\r\n");
    }
    else if((cmp_str(str, "Abrir")) && (cond1 ==0))
    {
        //TA1CTL &= ~TAIE;
        abertura();

        Send_String("Valvula aberta\r\n");
        leitura();
    }
    else
    {
        Send_String("Opcao Invalida!\r\n");

    }


}
