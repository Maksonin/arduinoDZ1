#define peopleGreenLed 7
#define peopleRedLed 6
#define carGreenLed 3
#define carYellowLed 4
#define carRedLed 5

#define peopleGoBtn 2

#define checkTimeUpr (tmp < delay) && !uartUpr() && !uUpr

int sost = 1;
int uUpr = 0;

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
  uint32_t tmp = 0;
  uint16_t delay = 2000;
  while( checkTimeUpr ){
    tmp = millis() - time;
    digitalWrite(peopleGreenLed, LOW);
    digitalWrite(peopleRedLed, HIGH);
  }

  // желтый свет на автомобильном
  time = millis();
  tmp = 0;
  delay = 2000;
  while( checkTimeUpr ){
    tmp = millis() - time;
    digitalWrite(carYellowLed, HIGH);
  }

  // включение зеленого на постоянное свечение, пешеходный красный
  time = millis();
  tmp = 0;
  delay = 20000;
  while( checkTimeUpr ){ // свечение зеленым 20 секунд
    tmp = millis() - time;

    // Реация на прерывание по нажатию кнопки //
    /* Если произошло прерывание (sost == 0), то идёт проверка, что зеленый свет горит больше минимального времени (чтобы машины успели проехать после разрешения) 
    и меньше максимально-разумного времени (чтобы с учетом задержки не вышло за максимальный срок свечения зеленого) */
    if((sost == 0) && (tmp > 3000) && (tmp < 13000)) { 
        Serial.print("END!!\n");
        delay = tmp + 5000; // тогда задержка свечения выставляется как текущее время таймера + 5 секунд
        sost = 1; // 
    }

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
  uint32_t time = millis();
  uint32_t tmp = 0;
  uint16_t delay = 3000;
  while( checkTimeUpr ){
    tmp = millis() - time;
    digitalWrite(carGreenLed, LOW);
    digitalWrite(carYellowLed, HIGH);
    digitalWrite(carRedLed, LOW);
    
    digitalWrite(peopleGreenLed, LOW);
    digitalWrite(peopleRedLed, HIGH);
	
  }
}

// красный свет для авто
void carStop(){
  sost = 3;

  uint32_t time = millis();
  uint32_t tmp = 0;
  uint16_t delay = 2000;
  while( checkTimeUpr ){
    tmp = millis() - time;
    digitalWrite(carGreenLed, LOW);
    digitalWrite(carYellowLed, LOW);
    digitalWrite(carRedLed, HIGH);
  }

  time = millis();
  tmp = 0;
  delay = 5000;
  while( checkTimeUpr ){
    tmp = millis() - time;
    digitalWrite(peopleGreenLed, HIGH);
    digitalWrite(peopleRedLed, LOW);
  }
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
  Serial.print("\n-----S-----\n");
}

// прерывание по нажатию кнопки
void btnIsr() {
  if(sost == 1){ // если горит зеленый для машин
    Serial.println("Int!");
    sost = 0; // устанавливаем 
  }
}

// главный цикл
void loop() {
  Serial.println("Автоматическое управление");
  initLed();
  while(!uUpr){ // цикл автоматического управления
    carRun();
    carWait();
    carStop();
  }
  Serial.println("Ручное управление");
  while(uUpr){ // цикл ручного 
    uartUpr();
    if (uUpr == 1){
      initLed();
    }
    else if (uUpr == 2){
      digitalWrite(carGreenLed, HIGH);
      digitalWrite(carYellowLed, LOW);
      digitalWrite(carRedLed, LOW);
      digitalWrite(peopleGreenLed, LOW);
      digitalWrite(peopleRedLed, HIGH);
    }
    else if (uUpr ==  3){
      digitalWrite(carGreenLed, LOW);
      digitalWrite(carYellowLed, LOW);
      digitalWrite(carRedLed, HIGH);
      digitalWrite(peopleGreenLed, HIGH);
      digitalWrite(peopleRedLed, LOW);
    }
    else if (uUpr ==  4){
      initLed();
    }
  }
}

// функция включает начальное свечение светофора (все красные)
void initLed(){
    digitalWrite(carGreenLed, LOW);
    digitalWrite(carYellowLed, LOW);
    digitalWrite(carRedLed, HIGH);
    digitalWrite(peopleGreenLed, LOW);
    digitalWrite(peopleRedLed, HIGH);
}

// фукнция проверяет наличие сообщений в буфере UART и если определяет управляющие команды - меняет режимы
int uartUpr(){
  if(Serial.available()){
    int val = Serial.parseInt();
    Serial.print("UART - ");
    Serial.print(val);
    Serial.print(" UPR - ");
    Serial.println(uUpr);

    if((val < 1) || (val > 4)){ // если не найдено управляющих команд
      Serial.println("Введите 1 для активации режима удаленного управления");
      Serial.println("Введите 2 для активации зеленого света для авто");
      Serial.println("Введите 3 для активации зеленого света для пешеходов");
      Serial.println("Ввведите 4 для активации красного света для всех");
      Serial.println("Для отключения режима удаленного управления нажмите 1");
      return 0;
    }
    else if(val == 1){ // если передана 1 - включается/выключается режим автоматического управления
      if(uUpr > 0) uUpr = 0;
      else if (!uUpr) uUpr = 1;
      Serial.print(uUpr);
      Serial.println(" - Смена режима");
    }
    else if((uUpr > 0) && (val > 1) && (val < 5) ) { // если включен ручной режим управления и пришли цифры от 2 до 4, то считаем их командами
      Serial.println("Сигнал управления");
      uUpr = val;
    }
    return 1;
  }
  else return 0;
}
