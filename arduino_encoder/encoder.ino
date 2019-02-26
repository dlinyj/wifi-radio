/* Пины, к которым подключен энкодер */
enum { ENC_PIN1 = 2, ENC_PIN2 = 3, SEL_PIN=4 }; 

void setup() {
	pinMode(ENC_PIN1, INPUT);
	pinMode(ENC_PIN2, INPUT);
	pinMode(SEL_PIN, INPUT);
	digitalWrite(ENC_PIN1, HIGH);       // turn on pullup resistors
	digitalWrite(ENC_PIN2, HIGH);       // turn on pullup resistors
	digitalWrite(SEL_PIN, HIGH);       // turn on pullup resistors
	
	Serial.begin(9600);
}

/* Функция декодирования кода Грея, взятая с Википедии.
* Принимает число в коде Грея, возвращает обычное его представление.
*/
unsigned graydecode(unsigned gray) {
	unsigned bin;
	for (bin = 0; gray; gray >>= 1) {
		bin ^= gray;
	}
	return bin;
}

void loop() {
	static uint8_t previous_code = 0; // предыдущий считанный код
	static uint8_t select_sw = 0;
	static uint8_t select_sw_old = 0;
/* gray_code - считанное с энкодера значение
* code - декодированное значение
*/
	uint8_t gray_code = digitalRead(ENC_PIN1) | (digitalRead(ENC_PIN2) << 1),
		code = graydecode(gray_code);
/* Если считался нуль, значит был произведён щелчок ручкой энкодера */
	if (code == 0) {
		if (previous_code == 3) //Если переход к нулю был из состояния 3 - ручка вращалась
			Serial.print("L"); //по часовой стрелке, если из 1 - против.
		else if (previous_code == 1)
			Serial.print("R");
	}
	previous_code = code;
	
	select_sw=digitalRead(SEL_PIN);
	if (select_sw && !select_sw_old) {
		if (select_sw != 0) {
			Serial.print("P");
		}
		if (select_sw_old != 0) {
			Serial.print("U");
		}
	}
	select_sw_old=select_sw;
/* Сохраняем код и ждём 1 мс - вполне достаточно опрашивать энкодер
* не более 1000 раз в секунду.
*/
	delay(1);
}
