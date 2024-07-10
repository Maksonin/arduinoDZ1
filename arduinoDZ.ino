#define peopleGreenLed 7
#define peopleRedLed 6
#define carGreenLed 3
#define carYellowLed 4
#define carRedLed 5

#define peopleGoBtn 2

#define strobDelay 500
#define carGreenDelay 5000
#define carYellowDelay 3000
#define carRedDelay 5000

char sost = 1;

uint32_t timer = 0; // переменная таймера
uint32_t setDelay = 0;

// зеленый свет для авто
void carRun(){
  sost = 1;

  // моргание зеленого света для пешеходов
  for(int i = 0; i < 5; i++) {
    digitalWrite(peopleGreenLed, !digitalRead(peopleGreenLed));
    delay(500);
  }

  // все светофоры - красный свет
  uint32_t time = millis();
  while((millis() - time) < 2000 ){
    digitalWrite(peopleGreenLed, LOW);
    digitalWrite(peopleRedLed, HIGH);
  }

  // желтый свет на автомобильном
  time = millis();
  while((millis() - time) < 2000 ){
    digitalWrite(carYellowLed, HIGH);
  }
  //delay(2000);

  uint8_t buf[8] = {0};

  // включение зеленого на постоянное свечение, пешеходный красный
  time = millis();
  uint16_t delay = 20000;
  uint32_t tmp = 0;
  while( tmp < delay ){ // свечение зеленым 20 секунд
    tmp = millis() - time;

    // Реация на прерывание по нажатию кнопки //
    /* Если произошло прерывание (sost == 0), то идёт проверка, что зеленый свет горит больше минимального времени (чтобы машины успели проехать после разрешения) 
    и меньше максимально-разумного времени (чтобы с учетом задержки не вышло за максимальный срок свечения зеленого) */
    if((sost == 0) && (tmp > 3000) && (tmp < 13000)) { 
        Serial.write("END!!\n");
        delay = tmp + 5000; // тогда задержка свечения выставляется как текущее время таймера + 5 секунд
        sost = 1; // 
    }

    setIntToCharBuf(tmp, buf, 8);
    printCharBuf(buf, 8);

    Serial.write(" - ");

    setIntToCharBuf(delay, buf, 5);
    printCharBuf(buf, 5);
    
    Serial.write("\n");

    digitalWrite(carGreenLed, HIGH);
    digitalWrite(carYellowLed, LOW);
    digitalWrite(carRedLed, LOW);
  }
}

// желтый свет для авто
void carWait(){
  sost = 2;

  for(int i = 0; i < 5; i++) {
    digitalWrite(carGreenLed, !digitalRead(carGreenLed));
    delay(500);
  }

  digitalWrite(carGreenLed, LOW);
  digitalWrite(carYellowLed, HIGH);
  digitalWrite(carRedLed, LOW);
  
  digitalWrite(peopleGreenLed, LOW);
  digitalWrite(peopleRedLed, HIGH);
	
  delay(3000);
}

// красный свет для авто
void carStop(){
  sost = 3;
  digitalWrite(carGreenLed, LOW);
  digitalWrite(carYellowLed, LOW);
  digitalWrite(carRedLed, HIGH);

  delay(2000);

  digitalWrite(peopleGreenLed, HIGH);
  digitalWrite(peopleRedLed, LOW);
  delay(5000);
}

// перевод int в символьный вид с записью порязрядно в буфер buf
void setIntToCharBuf(uint32_t num, uint8_t buf[], uint8_t len){
  len -= 1;
  
  // очистка буфера
  for(int i = 0; i < len; i++)
    buf[i] = 0;

  if(num == 0){ // если в функцию был передан 0
    buf[len] = '0';
  }
  else { // иначе производим перебор переданного числа с записью каждого разряда в элемент массива buf
    while(num){
      buf[len] = '0' + (num % 10);
      num /= 10;
      len--;
    }
  }
}

// вывод буфера buf в uart
void printCharBuf(uint8_t buf[], uint8_t len){
    for(int i = 0; i < len; i++)
      Serial.write(buf[i]);
}

// настройка
void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, btnIsr, FALLING);
  Serial.write("\n----------\n");
}

// прерывание по нажатию кнопки
void btnIsr() {
  if(sost == 1){ // если горит зеленый для машин
    Serial.write("Int!");
    sost = 0; // устанавливаем 
  }
}

// главный цикл
void loop() {
  // put your main code here, to run repeatedly:
  carRun();
  carWait();
  carStop();
}

void yield(){
  if(Serial.available())
    Serial.write(Serial.read());
}
