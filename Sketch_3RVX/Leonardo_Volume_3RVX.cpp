/*
 * Keyboard emulator for USB volume control
 *
 * Ralph Bacon March 2020 v0.1 Basic Mute/Vol Down+Up
 *
 * Uses standard Arduino Keyboard library and 3RVX volume
 * control Windows volume control app.
 * See https://3rvx.com/ and https://github.com/malensek/3RVX
 * by Matthew Malensek.
 *
 * Thanks to John Wasser who supplied the keypad values I'm
 * using here.
 * You can substitute standard key characters if you prefer.
 * Ref:https://forum.arduino.cc/index.php?topic=179548.0
 *
 * Keypad keys are as follows:
 * (Convert DEC value to HEX to use)
 *
 * 		DEC	DESCRIPTION
 * 		221 Keypad *
 * 		222 Keypad -
 * 		223 Keypad +
 * 		220 Keypad /
 * 		224 Keypad ENTER
 * 		225 Keypad 1 and End
 * 		226 Keypad 2 and Down Arrow
 * 		227 Keypad 3 and PageDn
 * 		228 Keypad 4 and Left Arrow
 * 		229 Keypad 5
 * 		230 Keypad 6 and Right Arrow
 * 		231 Keypad 7 and Home
 * 		232 Keypad 8 and Up Arrow
 * 		233 Keypad 9 and PageUp
 * 		234 Keypad 0 and Insert
 * 		235 Keypad . and Delete
 *
 */
#include "Leonardo_Volume_3RVX.h"
#include "Arduino.h"
#include <Keyboard.h>

// Comment out next line to prevent slow response when
// not actively debugging via serial port monitor
#define DEBUG

// Rotary Encoder interrupt pins (0, 1, 2, 3, 7) on Leonardo
#define pinA 2
#define pinB 3

// Use the click as a Mute/Unmute
#define mutePin 11
bool isMutePressed = false;
bool isMuted = false;
bool ignoreMute = false;

void setup()
{
#ifdef DEBUG
	Serial.begin(115200);
	while (!Serial)
		;

	/*
	 * You may beed to put a 5 second delay here for the COM port
	 * to be enumerated and be available
	 */
	Serial.println("Setup starts");
#endif

	// Rotary encoder setup
	pinMode(pinA, INPUT);
	pinMode(pinB, INPUT);

	// Mute function
	pinMode(mutePin, INPUT_PULLUP);

	// Use the library to communicate with Windows
	Keyboard.begin();
}

void loop()
{
	// Mute/Unmute button pressed?
	if (!isMutePressed
		&& digitalRead(mutePin) == LOW)
	{
		isMutePressed = true;
	}

	// Did the Rotary Encode get turned?
	// (Pin A goes LOW before going HIGH at next detent)
	if (digitalRead(pinA) == LOW)
	{
		// Reset the MUTE state as we did not mute anything
		if (isMutePressed)
		{
			ignoreMute = true;
		}

		// Clockwise?
		if (digitalRead(pinB) != LOW)
		{
#ifdef DEBUG
			Serial.println("U");
#endif
			Keyboard.press(KEY_LEFT_ALT);
			Keyboard.press(KEY_LEFT_CTRL);
			Keyboard.press(0xDF);
			delay(100);
			Keyboard.releaseAll();

			if (isMutePressed)
			{
#ifdef DEBUG
				Serial.println("U2");
#endif
				Keyboard.press(KEY_LEFT_ALT);
				Keyboard.press(KEY_LEFT_CTRL);
				Keyboard.press(0xDF);
				delay(100);
				Keyboard.releaseAll();
			}

		} else
		{
#ifdef DEBUG
			Serial.println("D");
#endif
			Keyboard.press(KEY_LEFT_ALT);
			Keyboard.press(KEY_LEFT_CTRL);
			Keyboard.press(0xDE);
			delay(100);
			Keyboard.releaseAll();

			if (isMutePressed)
			{
#ifdef DEBUG
				Serial.println("D2");
#endif
				Keyboard.press(KEY_LEFT_ALT);
				Keyboard.press(KEY_LEFT_CTRL);
				Keyboard.press(0xDE);
				delay(100);
				Keyboard.releaseAll();
			}
		}

		// Wait until rotation to next detent is complete
		while (digitalRead(pinA) != HIGH)
		{
			// Debounce delay
			delay(5);
		}
	}

	// Mute button previously pressed?
	if (isMutePressed)
	{
		// Mute button now released?
		if (digitalRead(mutePin) == HIGH)
		{
			// Reset the flag and wait for a debounce interval
			isMutePressed = false;

			// Should we ignore the mute request because
			// we did a fast volume change?
			if (ignoreMute)
			{
				// Reset that flag but do nothing else
				ignoreMute = false;
			}
			else
			{
				Keyboard.press(KEY_LEFT_ALT);
				Keyboard.press(KEY_LEFT_CTRL);
				Keyboard.press(0xDD);
				delay(100);
				Keyboard.releaseAll();
#ifdef DEBUG
				Serial.println(isMuted ? "Unmuted" : "Muted");
#endif
				isMuted = !isMuted;
			}
		}
	}
}
