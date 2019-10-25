#include <math.h> // só pra usar a função fabs
#include <SoftwareSerial.h> // usa a biblioteca necessária para o uso do módulo Bluetooth
SoftwareSerial bluetooth(3, 4); // RX, TX da MSP

int A = 0; // define a variável responsável por avaliar os dados recebidos via bluetooth

//variáveis para temperatura
const int LM35 = 6; // Define o pino que lera a saída do LM35, no caso, p1.4
float temperatura = 0;  // define as variáveis e flags necessárias para o funcionamento do programa
float temperatura_nova = 0;
boolean cond1 = 0; // cond1 indica que a válvula deve ser aberta
boolean cond2 = 0; //cond2 indica que a válvula deve ser fechada
boolean cond3 = 0; //cond3 indica que a válvula ESTÁ aberta

// biblioteca stepper para controlar motor
#include <Stepper.h>
// Define número de passos, no caso do motor usado, 1024 equivale a meia volta do motor
const int stepsPerRevolution = 1024;

// define onde estão os pinos para serem controlados pela biblioteca, e o número de passos
Stepper myStepper = Stepper(stepsPerRevolution, 10, 8, 7, 9); //11 ligado ao pino 1 do ULN2003; 7, 8 e 9 ligados aos pinos 2-4

void setup() {
  // Seta velocidade (em rpm)
  myStepper.setSpeed(15);

  bluetooth.begin(9600); // incia módulo bluetooth

  // inicia serial no mesmo baud rate que o bluetooth
  Serial.begin(9600);
}


void loop() {

  // medições de temperatura

  if (cond3 == 0) { // se a válvula ainda não foi aberta, vai salvando esse valor pra comparar com os valores depois da abertura
    temperatura = temperatura_nova; // temperatura equivale a Temperatura no momentode abertura da válvula, ou seja, T0
  }

  temperatura_nova = (float(analogRead(LM35)) * 35) / 100; // Temperatura_nova é a temperatura em tempo real

  Serial.print("Temperatura: "); // envia para a ground station os valores de temperatura medidos
  Serial.println(temperatura_nova);
  delay (500); // tem um pequeno delay, para que seja possível a leitura dos valores pelo usuário

  if ( (fabs((temperatura - temperatura_nova)) > 15) && (cond3 == 1) ) { // verifica se ouve uma variação em relação a temperatura inicial maior que 15ºC
    cond2 = 1;
    Serial.println("temperatura para fechar (menos): "); // envia os dados dos valores de T0 e temperatura atual
    Serial. print("Temperatura inicial: ");               // para que seja fácil a visualização sobre o ocorrido pelo usuário
    Serial.println(temperatura);
    Serial. print("Temperatura atual: ");
    Serial.println(temperatura_nova);

  }


  //HC-06 (módulo bluetooth)

  if (Serial.available()) { // checa se há algo no buffer da UART
    A = Serial.read(); // se há, lê o valor e salva em A
  }

  if (A == 49) { // caso seja recebido o valor 49 pelo módulo bluetooth, ele abre a válvula
    cond1 = 1;  // 49 seria o equivalente ao valor 1 enviado pelo aplicativo BlueTerm+
  }
  else if ((A == 50) && (cond3 == 1)) { // caso o valor recebido em A seja igual à 50, enquanto a válvula está aberta, ele fecha a válvula
    cond2 = 1;                        // 50 é o equivalente ao valor 2 enviado pelo aplicativo BlueTerm+
  }

  // caso condição de abertura atendida:
  if ((cond1 == 1) && (cond3 |= 1)) {
    Serial.println("Abrindo valvula"); // ele envia uma mensagem para a ground station sobre o que está ocorrendo
    myStepper.step(stepsPerRevolution); // rotaciona o motor de passo o equivalente à meia volta, quatro vezes
    myStepper.step(stepsPerRevolution); // foi realizado o procediento de rotacionar meia volta para facilitar uma calibração posterior
    myStepper.step(stepsPerRevolution);
    myStepper.step(stepsPerRevolution);

    cond1 = 0; // reseta a condição de abertura
    cond3 = 1; // ativa a flag que indica que a válvula está aberta
    A = 0;     // apaga os dados obtidos pelo último sinal de bluetooth
  }

  // caso condição de fechamento atendida
  if ((cond2 == 1) && (cond3 == 1)) {  // caso a válvula esteja aberta e seja atingida a condição de fechamento
    Serial.println("Fechando valvula");  // ele rotaciona o motor na mesma quantidade anterior. porém em sentido contrário
    myStepper.step(-stepsPerRevolution);
    myStepper.step(-stepsPerRevolution);
    myStepper.step(-stepsPerRevolution);
    myStepper.step(-stepsPerRevolution);
    cond2 = 0; // reseta as flags de condição
    cond3 = 0;
    A = 0;     // apaga os dados obtidos pelo último sinal de bluetooth
  }
}
