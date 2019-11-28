#include <msp430g2553.h>
#include <Projeto.h> // header contendo função em assembly

#define TEMP_IN      BIT4 // DEFINE P1.4 COMO TEMP_IN PRA FACILITAR ENTENDIMENTO
#define RX BIT1 // DEFINE PINOS P1.1 E P1.2 COMO TX E RX PRA FACILITAR SUA COMPREENSÃO
#define TX BIT2

#define BAUD_9600   0 // OPÇÕES PARA SELEÇÃO DE BAUD_RATE, NO CASO DO
#define BAUD_19200  1 // HC-06, O PADRÃO É 9600, MAS PODE SER MODIFICADO
#define BAUD_38400  2 // CASO NECESSÁRIO
#define BAUD_56000  3
#define BAUD_115200 4
#define BAUD_128000 5
#define BAUD_256000 6
#define NUM_BAUDS   7

#define IN1     BIT0 //DEFINE OS PINOS RESPONSÁVEIS PELO CONTROLE DE ROTAÇÃO DO MOTOR
#define IN2     BIT1 // PARA QUE SEJA FACILITADA SUA DECLARAÇÃO / MODIFICAÇÃO
#define IN3     BIT2 // AO LONGO DO PROJETO
#define IN4     BIT5

int i=0, j=0, a = 0, counter = 0, cond1 = 0, k = 0; // VARIÁVEIS USADAS AO LONGO DO PROJETO
// USADAS EM LOOPS, IFS, ETC.
float temp_init = 0, temperature = 0, temperature2 = 0; // VARIÁVEIS USADAS PARA LEITURA DE TEMPERATURA
int cond4 = 0; // VARIÁVEL USADA DENTRO DA INTERRUPÇÃO PARA SELECIONAR SE É MEDIDA DE TEMPERATURA INICIAL
//OU MODIDA DE TEMPERATURA ATUAL
char str[20]; //VARIÁVEL NA QUAL SERÁ ARMAZENADA OS DADOS OBTIDOS ATRAVÉS DO APLICATIVO

int Read_String(char str[], int strlen); // DECLARANDO FUNÇÕES ANTES DE SEREM USADAS
                    // PARA EVITAR WARNINGS
char Receive_Data(void);
void Send_Data(unsigned char c);
void Send_Int(int n);
void Send_String(char str[]);
void Init_UART(unsigned int baud_rate_choice);
int cmp_str(char str1[], char str2[]);


int main(void) //MAIN
{

    WDTCTL = WDTPW + WDTHOLD; // STOP WATCHDOG TIMER

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P2DIR = (IN1 + IN2 + IN3 + IN4);
    P1OUT = 0; // RESETA QUALQUER SAÍDA
    P2OUT = 0;
    P1DIR = BIT6;
    P1DIR &= ~TEMP_IN;  // define sensor como entrada
    Init_UART(BAUD_9600); // INICIANDO UART COM BAUD RATE DE 9600

    TA1CCR0 = 12500-1;
    TA1CTL = TASSEL_2 + ID_3 + MC_1;

    IE2 |= UCA0RXIE;// ATIVANDO INTERRUPÇÕES DA PORTA RX
    _enable_interrupts(); // PERMITINDO INTERRUPÇÕES
    __low_power_mode_1(); // ENTRANDO EM LOW POWER MODE 1, POIS SERÁ USADO CLOCK SMCLK
  while(1) // ENTRA EM UM LOOP INFINITO
   {
      }
}


int Read_String(char str[], int strlen) // FUNÇÃO QUE LÊ A STRING RECEBIDA DO APLICATIVO
{                   //E ARMAZENA EM STR[]
    int i = 0;
    do
    {
        // ARMAZENA CADA CARACTERE RECEBIDO
        str[i] = Receive_Data();

        if(str[i]!='\r')// CASO SEJA RECEBIDO UM \R, ELE O IGNORA
        {
            if(str[i]=='\n') // CASO RECEBIDO UM \n (PULA LINHA), ELE FINALIZA A LEITURA DA STRING
            {
                str[i] = '\0'; //  ANTES DE FINALIZAR A LEITURA, SUBSTITUI O \N POR UM \0, QUE É VAZIO PARA A STRING
                break;
            }
            i++; // INCREMENTA I
        }
    } while(i<strlen); // REPETE O PROCEDIMENTO ACIMA, CASO I AINDA NÃO SEJA IGUAL AO TAMANHO DA STRING
            //TAMANHO ESTE DECLARADO QUANDO CHAMADA A FUNÇÃO
    if(i==strlen) // CASO SEJA ATINGIDO O TAMANHO DA STRING, ADICIONA UM \0 E FINALIZA A FUNÇÃO
    {
        i--;
        str[i] = '\0';
    }
 //ENVIA DE VOLTA O TAMANHO DA STRING (QUE PODE SER MENOR QUE O TAMANHO CHAMADO NA FUNÇÃO)
    return i;
}

char Receive_Data(void) // FUNÇÃO RESPONSÁVEL POR RECEBER CADA CARACTERE ENVIADO
{
    while((IFG2&UCA0RXIFG)==0);
    return UCA0RXBUF;
}

void Send_Data(unsigned char c) // FUNÇÃO RESPONSÁVEL POR ENVIAR CADA CARACTERE
{
    while((IFG2&UCA0TXIFG)==0);
    UCA0TXBUF = c;
}

void Send_Int(int n) // FUNÇÃO RESPONSÁVEL POR ENVIAR CADA VARIÁVEL DO TIPO INT
{
    int casa, dig;
    if(n==0) //  PARA O CASO ESPECIAL 0
    {
        Send_Data('0'); // ENVIA 0 E FINALIZA A FUNÇÃO
        return;
    }
    if(n<0) //  PARA O CASO ESPECIAL DE UMA VARIÁVEL NEGATIVA
    {
        Send_Data('-'); // ENVIA UM SINAL DE NEGATIVO E DEPOIS ENVIA O SINAL POSITIVO
        n = -n;
    }
    for(casa = 1; casa<=n; casa *= 10); // LOOP RESPONSÁVEL POR ENVIAR CADA DIGITO DA VARIÁVEL
    casa /= 10;
    while(casa>0) //
    {
        dig = (n/casa);
        Send_Data(dig+'0');
        n -= dig*casa;
        casa /= 10;
    }
}

void Send_String(char str[]) // FUNÇÃO QUE ENVIA UMA STRING INTEIRA
{
    int i;
    for(i=0; str[i]!= '\0'; i++) // ENVIA CADA CARACTERE DA STRING USANDO A FUNÇÃO Send_Data
        Send_Data(str[i]);
}



static void Send_Float (float data)
{

    static int pint; // VARIÁVEL PARA ARMAZENAR PARTE INTEIRA
    static int pfract; // VARIÁVEL PAR AARMAZENAR PARTE FRACIONÁRIA
    int sign; //VARIÁVEL PARA ARMAZENAR SINAL DO NÚMERO

    sign=(int)data;
    pint =(int)data;
    pfract= (int)((data - pint)*100);
    pint = abs(pint);
    pfract = abs(pfract);
    if (sign < 0)
    {
        UCA0TXBUF= 45; //"-"
        while (!(IFG2 & UCA0TXIFG));
    }
    Send_Int(pint);
    Send_String(".");
    Send_Int(pfract);

}

void Init_UART(unsigned int baud_rate_choice) // FUNÇÃO QUE INICIA A UART A DEPENDER DO BAUD_RATE SELECIONADO
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
        P1SEL2 = P1SEL = RX+TX;//HABILITA OS PINOS NECESSÁRIOS PARA A TRANSMISSÃO SERIAL UART

        UCA0CTL0 = 0; //ESCOLHE O SMCLK PARA A UART

        UCA0CTL1 = UCSSEL_2;

        UCA0BR0 = BRs[baud_rate_choice]; // ESCOLHE O BAUD_RATE SETANDO AS VARIÁVEIS, PARA O CASO DE 9600, SERIA 104

        UCA0BR1 = 0;
        UCA0MCTL = MCTLs[baud_rate_choice]; // PARA OBTER BAUD RATE DE 9600: SETA UCBRF_0 E UCBRS_0
    }
}


int cmp_str(char str1[], char str2[]) // FUNÇÃO DESENVOLVIDA PARA COMPARAR DUAS STRINGS
{
    int i;
    for(i=0; (str1[i]!='\0')&&(str2[i]!='\0'); i++) //LOOP DE COMPARAÇÃO ENQUANTO NENHUMA DAS STRINGS ACABAR (\0)
    {
        if(str1[i]!=str2[i])// CASO EM ALGUM MOMENTO ELAS SEJAM DIFERENTES (ANTES DE ACABAR), RETORNA FALSO
            return 0;
    }
    if(str1[i]!=str2[i]) // CASO UMA DELAS ACABE E A OUTRA NÃO, RETORNA 0
        return 0;
    else
        return 1;// CASO NÃO ATENDAM AS DEFINIÇÕES ACIMA, ELAS SÃO IGUAIS, RETORNA VERDADEIRO
}


void abertura (void){ // FUNÇÃO PARA ATIVAÇÃO DO MOTOR EM SENTIDO HORÁRIO, OU ABERTURA DA VÁLVULA
          Send_String("Abrindo Valvula\r\n");
          cond1 = 1; // ATIVA COND1 QUE INDICA QUE A VÁLVULA ESTÁ ABERTA
             for (a = 0; a <2048; a++){ // ENQUANTO NÃO SEJA REPETIDO O SEGUINTE PROCESSO 2048 VEZES (UMA VOLTA COMPLETA)
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

                 delay(2); //DELAY DE 2ms PODENDO SER ALTERADO PARA MODIFICAR VELOCIDADE E TORQUE DO MOTOR
                 }
             P2OUT = 0; // ZERA AS SAÍDAS PARA NÃO DANIFICAR O MOTOR


         }


void fechamento (void){ //  MESMA FUNÇÃO QUE A ACIMA, PORÉM NO SENTIDO CONTRÁRIO DE ROTAÇÃO DO MOTOR, USADO PARA FECHAMENTO DA VÁLVULA
        Send_String("Fechando Valvula\r\n");
        cond1 = 0; // ZERA COND1 INDICANDO QUE A VÁLVULA ESTÁ FECHADA

       cond4 = 0; // ZERA COND4 QUE SERÁ POSTERIORMENTE EXPLICADA NA FUNÇÃO LEITURA
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
                      delay(2);
              }
          P2OUT = 0;// ZERA AS SAÍDAS PARA NÃO DANIFICAR O MOTOR
      }

void leitura(void)
{

      _enable_interrupts(); //PERMITE INTERRUPÇÕES POIS PODE SER ATIVADA A CONDIÇÃO DE FECHAMENTO DA VÁLVULA
    while(cond1 == 1){ //CASO A VÁLVULA ESTEJA ABERTA, REALIZA LEITURAS
        if(cond4 == 0){ // CASO SEJA A PRIMEIRA LEITURA, FAZ UMA LEITURA INICIAL
          cond4 = 1; // ATIVA A FLAG QUE JÁ FOI REALIZADA A LEITURA INICIAL
          temp_init= 0;
          for (i = 0; i<20; i++){ // REALIZA 20 LEITURAS PARA SE TER UMA BOA AMOSTRAGEM
          temperature = convert();
          temperature = temperature*700/1024.0; // MULTIPLICA POR ESSES VALORES PARA OBTER VALOR CERTO DE TEMPERATURA
          temp_init = temp_init + temperature; // VAI SOMANDO A TEMPERATURA DE CADA PONTO
          delay(100); // ATRASO PARA DIMINUIR ERROS PROPAGADOS AO SEREM REALIZADAS LEITURAS EM CURTO PERÍODO DE TEMPO
        }
          temp_init = temp_init/20.0; // TIRA A MÉDIA DAS LEITURAS
          Send_String("Temperatura Inicial: "); // ENVIA OS DADOS DA TEMPERATURA INCICIAL
          Send_Float(temp_init);
          Send_String("\r\n");
        }

        else{

        temperature2 = 0;

      for (i = 0; i<10; i++){ // REALIZA O MESMO PROCEDIMENTO ACIMA, PORÉM PARA A TEMPERATURA DE CADA PONTO
                temperature = convert();
                temperature = temperature*700/1024.0; // MULTIPLICA POR ESSES VALORES PARA OBTER VALOR CERTO DE TEMPERATURA
                temperature2 = temperature2 + temperature;
        delay(100);
    }
                                  //P1OUT ^= BIT6;
    temperature2 = temperature2/10.0;
    Send_String("Temperatura Atual: "); // ENVIA A TEMPERATURA ATUAL
    Send_Float(temperature2);
    Send_String("\r\n");
    if(fabs(temperature2 - temp_init)>5){ // CASO O MÓDULO DA VARIAÇÃO DA TEMPERATURA ATUAL EM RELAÇÃO À INCIIAL CHEGUE A 5, ELE FECHA A VÁLVULA
                 fechamento();

                } }}}


#pragma vector=USCIAB0RX_VECTOR;
    __interrupt void USCI0RX_ISR(void) // ROTINA DE INTERRUPÇÃO
    {
    __low_power_mode_off_on_exit();

    Read_String(str, 20); // LÊ DADO RECEBIDO

    if((cmp_str(str, "Fechar")) && (cond1 == 1)) // CASO O DADO SEJA: "FECHAR" E A VÁLVULA ESTEJA A BERTA (COND1 = 1)
                        // ELE FECHA A VÁLVULA
    {

        fechamento();

        Send_String("Valvula Fechada\r\n"); // ENVIA MENSAGEM SOBRE TER SIDO FINIALIZADO FECHAMENTO DA VÁLVULA
    }
    else if((cmp_str(str, "Abrir")) && (cond1 ==0)) // CASO A VÁLVULA ESTEJA FECHADA E SEJA RECEBIDO COMANDO ABRIR, FECHA A VÁLVULA
    {

        abertura();

        Send_String("Valvula aberta\r\n"); //ENVIA MENSAGEM SOBRE TER SIDO FINIALIZADA A FECHAMENTA DA VÁLVULA
        leitura();
    }
    else // CASO NÃO SEJA NENHUM DOS CASOS ACIMA, O DADO RECEBIDO É UM COMANDO INVÁLIDO
    {
        Send_String("Comando Invalido!\r\n");

    }
}
