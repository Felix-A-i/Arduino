#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C // 0x3C device address of I2C OLED
Adafruit_SSD1306 oled(128, 64);

int x=0;
int lastx=0;
int lasty=60;
int TermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf, temp, templast;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //valores constante para calculo

int tempodeexec;  //Representa o tempo de execuÃ§Ã£o
int add; //Representa o botÃ£o "+"
int sub; //Representa o botÃ£o "-"
int start; //Representa o botÃ£o "e"
const int buzzer = 8;
long tempomax;

float temperatura;
float temperaturaSetada = 0;
bool estaRodando = false;

void setup() {
	tempomax = 50000;
  // put your setup code here, to run once:
	tempodeexec = 0;
	pinMode(TermistorPin, INPUT);
	pinMode(13, OUTPUT);
	pinMode(buzzer, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(4, INPUT);
	Serial.begin(9600);
	
	//inicialização do display oled 
	oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
	oled.clearDisplay();
	oled.setTextSize(1);
	oled.setTextColor(WHITE);
}

void loop() {
	adicionarTempo();
	diminuirTempo();
	temperatura = readTemp(TermistorPin);
	if(temperaturaSetada!=temperatura){
		Serial.print("Temperatura atual: ");
		Serial.println(temperaturaSetada);
		
		Serial.print("Temperatura setada: ");
		Serial.println(temperatura);
		temperaturaSetada = temperatura;
	}
  	oledTemp(temperaturaSetada);
  	graphTemp(temperaturaSetada);

	start = digitalRead(4);
	
	informaTempo();
	if(temperaturaSetada > 0 && tempodeexec > 0 && start == LOW && estaRodando == false){
		inicia();
	}else if(start == LOW && estaRodando == true){
		para();
	}else if(estaRodando == true && start == HIGH){
		validaTempo();
	}
}

float readTemp(int ThermistorPin){
  
	int Vo = analogRead(ThermistorPin);
	float R1 = 10000;
	float R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculo R2, demonstraÃ§Ã£o no arquivo pdf da aula
	float logR2 = log(R2);
	float T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// EquaÃ§Ã£o de Steinhartâ€“Hart 
	float Tc = T - 273.15; //temp em Graus celcius
	//Tf = (Tc * 9.0)/ 5.0 + 32.0; // temp em fahrenheit
	return Tc;
	
}

void inicia(){
	digitalWrite(6, HIGH);
	digitalWrite(9, HIGH);
	estaRodando = true;
	Serial.println("Startando ...");
}

void para(){
	digitalWrite(6, LOW);
	digitalWrite(9, LOW);
	estaRodando = false;
	tempodeexec = 0;
	informaTempo();
	Serial.println("Parando...");
	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);
	delay(5000);
	tone(buzzer,1500);   
  	delay(500);
	noTone(buzzer);
  	delay(500); 
}

void informaTempo(){
	Serial.print("Tempo setado: ");
	Serial.println(tempodeexec);
}

void adicionarTempo(){
	add = digitalRead(2);
	if(add == LOW && tempodeexec < tempomax){
		tempodeexec += 1000;
		informaTempo();
	}	
}

void diminuirTempo(){
	sub = digitalRead(3);
	if(sub == LOW && tempodeexec > 0){
		tempodeexec -= 1000;
	}
}

void validaTempo(){
	tempodeexec -= 1000;
	informaTempo();
	if(tempodeexec <= 0){
		para();
	}

}


//função que exibe no display oled a temperatura
void oledTemp(float temp){

  oled.setCursor(0,0); 		//seta o cursor no inicio
  oled.setTextColor(BLACK); 	//e apaga o
  oled.print("temp:");			// valor da temp.
  oled.print(templast);		// antigo, o templast

  oled.setCursor(0,0);			// seta o cursor no inicio
  oled.setTextColor(WHITE);	// 
  oled.print("temp:");
  oled.print(temp);
  templast = temp;
  oled.display();
}

//exibe grafico da temp no oled
void graphTemp(float Tc){

  if(x>127) {
    oled.clearDisplay();
    x=0;
    lastx=x;
  } //quando chegar no fim da tela volta para a primeira coluna do oled

  int y=60-(Tc/7);      // Ajusta a posição em Y do valor lido
  oled.drawLine(lastx,lasty,x,y, SSD1306_WHITE);
  lasty=y;    //atualiza os valores de posição do oled em Y
  lastx=x;    // atualiza os valores de posição do oled em x

  oled.display(); 
  x++;  //incremeta a posição em X

}