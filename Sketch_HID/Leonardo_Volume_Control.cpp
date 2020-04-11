/*
 * Keyboard emulator for USB volume control
 *
 * Ralph Bacon March 2020 v0.1 Basic Mute/Vol Down+Up
 *
 * Uses the HID-Project library from NicoHood
 * (see https://github.com/NicoHood/HID)
 * to emulate a keyboard with esoteric key values.
 *
 */
#include "Leonardo_Volume_Control.h"
#include "Arduino.h"
#include <HID-Project.h>

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
	Serial.begin(115200);
	while (!Serial)
		;
	/*
	 * You may beed to put a 5 second delay here for the COM port
	 * to be enumerated and be available
	 */
	Serial.println("Setup starts");

	// Rotary encoder setup
	pinMode(pinA, INPUT);
	pinMode(pinB, INPUT);

	// Mute function
	pinMode(mutePin, INPUT_PULLUP);

	// Use the library to communicate with Windows
	Consumer.begin();
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
			Serial.println("U");
			Consumer.write(MEDIA_VOL_UP);
			if (isMutePressed)
			{
				Serial.println("U2");
				Consumer.write(MEDIA_VOL_UP);
			}
		} else
		{
			Serial.println("D");
			Consumer.write(MEDIA_VOL_DOWN);
			if (isMutePressed)
			{
				Serial.println("D2");
				Consumer.write(MEDIA_VOL_DOWN);
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
			delay(30);

			// Should we ignore the mute request because
			// we did a fast volume change?
			if (ignoreMute)
			{
				// Reset that flag but do nothing else
				ignoreMute = false;
			}
			else
			{
				Consumer.write(MEDIA_VOLUME_MUTE);
				Serial.println(isMuted ? "Unmuted" : "Muted");
				isMuted = !isMuted;
			}
		}
	}
}
