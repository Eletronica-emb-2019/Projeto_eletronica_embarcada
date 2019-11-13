# include  < math.h >  // apenas para usar uma função fabs
# include  < SoftwareSerial.h >  // usa uma biblioteca necessária para o uso do módulo Bluetooth
SoftwareSerial bluetooth ( 3 , 4 ); // RX, TX da MSP

int A = 0 ; // define uma variável responsável por avaliar os dados recebidos via bluetooth

// variáveis ​​para temperatura
const  int LM35 = 6 ; // Defina o pino que a saída do LM35, no caso, p1.4
temperatura do flutuador = 0 ;  // define como variáveis ​​e sinalizadores necessários para o funcionamento do programa
float temperatura_nova = 0 ;
cond1 booleano = 0 ; // cond1 indica que a válvula deve ser aberta
cond2 booleano = 0 ; // cond2 indica que a válvula deve ser fechada
cond3 booleano = 0 ; // cond3 indica que a válvula está aberta

// biblioteca stepper para controlar motor
# include  < Stepper.h >
// Define o número de etapas, sem caso do motor usado, 1024 equivalente a meia volta do motor
const  int stepsPerRevolution = 1024 ;

// define onde estão os pinos para serem controlados pela biblioteca e o número de etapas
Stepper myStepper = Stepper (stepsPerRevolution, 10 , 8 , 7 , 9 ); // 11 ligado ao pino 1 do ULN2003; 7, 8 e 9 ligados aos pinos 2-4

 configuração nula () {
  // Seta velocidade (em rpm)
  myStepper. setSpeed ( 15 );

  Bluetooth. begin ( 9600 ); // incia módulo bluetooth

  // inicia serial no mesmo baud rate que bluetooth
  Serial. begin ( 9600 );
}


 loop vazio () {

  // medições de temperatura

  if (cond3 == 0 ) { // se uma válvula ainda não foi aberta, será salva esse valor para comparar com os valores após a abertura
    temperatura = temperatura_nova; // temperatura equivalente a Temperatura no momento da abertura da válvula, ou seja, T0
  }

  temperatura_nova = ( float ( analogRead (LM35)) * 35 ) / 100 ; // Temperatura_nova é a temperatura em tempo real

  Serial. print ( " Temperatura: " ); // envia para uma estação terrestre os valores de temperatura medidos
  Serial. println (temperatura_nova);
  atraso ( 500 ); // tem um pequeno atraso, para que seja possível a leitura dos valores pelo usuário

  if (( fabs ((temperatura - temperatura_nova))> 15 ) && (cond3 == 1 )) { // verifica se você tem uma variação na relação a temperatura inicial maior que 15ºC
    cond2 = 1 ;
    Serial. println ( " temperatura para fechar (menos): " ); // envia os dados dos valores de T0 e temperatura atual
    Serial. print ( " Temperatura inicial: " );               // para que seja fácil as respostas sobre o ocorrido pelo usuário
    Serial. temperatura ( println );
    Serial. print ( " Temperatura atual: " );
    Serial. println (temperatura_nova);

  }


  // HC-06 (módulo bluetooth)

  if (Serial. available ()) { // checa se houver algo no buffer da UART
    A = Serial. read (); // se há, lê o valor e salva em A
  }

  if (A == 49 ) { // caso seja recebido o valor 49 pelo módulo bluetooth, ele abre uma válvula
    cond1 = 1 ;  // 49 seria ou equivalente ao valor 1 enviado pelo aplicativo BlueTerm +
  }
  else  if ((A == 50 ) && (cond3 == 1 )) { // caso o valor recebido seja igual a 50, enquanto a válvula estiver aberta, ele fecha a válvula
    cond2 = 1 ;                        // 50 é equivalente ao valor 2 enviado pelo aplicativo BlueTerm +
  }

  // caso condição de abertura atendida:
  if ((cond1 == 1 ) && (cond3 | = 1 )) {
    Serial. println ( " Abrindo valvula " ); // ele envia uma mensagem para uma estação terrestre sobre o que está ocorrendo
    myStepper. step (stepsPerRevolution); // rotação do motor ou passo equivalente a meia volta, quatro vezes
    myStepper. step (stepsPerRevolution); // foi realizado o procedimento de rotação meia volta para facilitar uma calibração posterior
    myStepper. step (stepsPerRevolution);
    myStepper. step (stepsPerRevolution);

    cond1 = 0 ; // redefinir a condição de abertura
    cond3 = 1 ; // ativa a flag que indica que a válvula está aberta
    A = 0 ;     // apaga os dados obtidos pelo último sinal de bluetooth
  }

  // caso condição de fechamento atendida
  if ((cond2 == 1 ) && (cond3 == 1 )) {   // caso a válvula esteja aberta e seja ativada a condição de fechamento
    Serial. println ( " Fechando valvula " );  // ele gira o motor na mesma quantidade anterior. porém em sentido contrário
    myStepper. etapa (-stepsPerRevolution);
    myStepper. etapa (-stepsPerRevolution);
    myStepper. etapa (-stepsPerRevolution);
    myStepper. etapa (-stepsPerRevolution);
    cond2 = 0 ; // reseta como flags de condição
    cond3 = 0 ;
    A = 0 ;     // apaga os dados obtidos pelo último sinal de bluetooth
  }
}
