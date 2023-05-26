#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
#include <Wire.h>

// CONFIGURATION

// Morse Code Pin Configuration
const int keyPin = 13;
const int buzzerPin = 32;

// VGA pin configuration
const int redPin = 33;
const int greenPin = 25;
const int bluePin = 26;
const int hsyncPin = 27;
const int vsyncPin = 14;

float dashDuration = 200.0;
boolean previousKeyPressed = false;
long timeStartPress, timeStartPause;
boolean keyPressed;
String letter = "";

VGA3Bit vga;
const char* headerText = "Try typing using the tapper";
long clearTime = 120000;
long backColour = vga.RGB(0, 0, 255);
long frontColour = vga.RGB(0, 255, 0);

// ARDUINO FUNCTIONS

void setup() {
	// Set up our morse code IO
	pinMode(keyPin, INPUT_PULLUP);
	pinMode(buzzerPin, OUTPUT);

	// Set up our VGA
	// Start vga on the specified pins
	vga.init(vga.MODE400x300, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
	// Select our screen colours
	vga.backColor = backColour;
	vga.frontColor = frontColour;
	// Select the font
	vga.setFont(Font6x8);
	// Clear the screen and write the header
	clearScreen();

	// Set up the serial interface
	//Serial.begin(115200);	 
}

void loop() {
	keyPressed = !digitalRead(keyPin);

	if (keyPressed) {
		if (keyPressed != previousKeyPressed) {
			timeStartPress = millis();
			decodePause(timeStartPause);
		}
		digitalWrite(buzzerPin, HIGH);
	}
	else {
		if (keyPressed != previousKeyPressed) {
			timeStartPause = millis();
			decode(timeStartPress);
		}
		digitalWrite(buzzerPin, LOW);
	}

	if ((millis() - timeStartPause) > dashDuration * 10) {
		decodePause(timeStartPause);
	}

	if ((millis() - clearTime) > timeStartPause)
	{
		clearScreen();
	}
	

	previousKeyPressed = keyPressed;
}

// FUNCTIONS

void decode(long startTime) {
	char symbol = '?';
	long time = millis() - startTime; // Duration of the signaled character (a dot or a dash)
	float dotDuration = dashDuration / 3.0;

	if (time <= 2) return; // Debouncing

	if (time <= dotDuration) symbol = '.';
	else if (time > dashDuration) symbol = '-';
	else if ((time > (dashDuration + dotDuration) / 1.9) && time <= dashDuration) symbol = '-';
	else symbol = '.';

	if (symbol == '-') {
		if (time > dashDuration) dashDuration++;
		if (time < dashDuration) dashDuration--;
	}
	else if (symbol == '.') {
		if (time > dashDuration / 3.0) dashDuration++;
		if (time < dashDuration / 3.0) dashDuration--;
	}
	letter += symbol;
}

void decodePause(long startTime) {
	if (letter == "") return;

	char symbol = '?';
	long time = millis() - startTime;
	if (time > dashDuration - dashDuration / 40) {
		decodeLetter();
		//Serial.print();
	}
	if (time > dashDuration * 2) {
		decodeLetter();
		Print(" ");
	}
	//else decodeLetter();
}

void decodeLetter() {
	static String letters[] = {
		".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
		".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "E"
	};
	int i = 0;
	while (letters[i] != "E") {
		if (letters[i] == letter) {
			Print((char)('A' + i));
			break;
		}
		i++;
	}
	if (letters[i] == "E") {
		Print(letter);
	}
	letter = "";
}

void Print(String &s) {
	for (int i = 0; i < s.length(); i++) {
		vga.print(s[i]);
		//Serial.print(s[i]);
	}
}
void Print(char s) {
	vga.print(s);
	//Serial.print(s);
}
void Print(char *s) {
	for (int i = 0; i < strlen(s); i++) {
		vga.print(s[i]);
		//Serial.print(s[i]);
	}
}

void clearScreen() {
	//make the background blue
	vga.clear(backColour);
	// Write the header
	vga.setCursor(0, 0);
	vga.println("----------------------");
	vga.println(headerText);
	vga.println("----------------------");
}