#include "ApplicationState.h"

// HEATER
#define TEMPERATURE_CHECK_INTERVAL 60000
#define TEMPERATURE_CHANGE_STEP 0.5
#define TEMPERATURE_MENU_DISPLAY_TIME 2000;
float setTemperature = DEFAULT_TEMPERATURE;
long lastTemperatureCheck;
long temperatureMenuTime;
// HEATER
//REMOTE
#define REMOTE_CHECK_INTERVAL 300
long lastRemoteCheck;
//REMOTE

//APPLICATION
int applicationState = RUNNING;
//APPLICATION

void loop() {
	long now = millis();
	if (applicationState != RUNNING)
		turnOffHeater();
	switch(applicationState) {
		case RUNNING: {
			displayBaseScreen();
			checkTemperature(now);
		} break;
		case TEMPERATURE_MENU: showTemperatureMenu();
	}
	checkRemoteRequest(now);
	resetApplicationState(now);
	delay(20);
}

void checkTemperature(long currentTime) {
	if (currentTime - lastTemperatureCheck > TEMPERATURE_CHECK_INTERVAL) {
		if(clock.meassureTemperature() < setTemperature){
			turnOnHeater();
		} else {
			turnOffHeater();
		}
	}
}

void turnOnHeater() {
	heaterDiode.turnOn();
	digitalWrite(heater, LOW);
}

void turnOffHeater() {
	heaterDiode.turnOff();
	digitalWrite(heater, HIGH);
}

void checkRemoteRequest(long currentTime) {
	if(currentTime - lastRemoteCheck > REMOTE_CHECK_INTERVAL) {
		int remoteButton = readRemote();
		if(applicationState == RUNNING) {
			applicationRunningRemoteControl(remoteButton);
		} else {
			applicationStopRemoteControl(remoteButton);
		}
	}
}

void applicationRunningRemoteControl(int remoteButton) {
	switch(remoteButton) {
		case POWER_ON_BUTTON: powerOnOff(); break;
		case MENU_BUTTON: applicationState = MENU; break;
		case PLUS_BUTTON: changeTemperature(TEMPERATURE_CHANGE_STEP); break;
		case MINUS_BUTTON: changeTemperature(-1 * TEMPERATURE_CHANGE_STEP); break;
	}
}

void applicationStopRemoteControl(int remoteButton) {
	switch(remoteButton) {
		case POWER_ON_BUTTON: powerOnOff(); break;
	}
}

void powerOnOff() {
	if (applicationState == RUNNING) applicationState = STOP;
	else applicationState = RUNNING;
}

void changeTemperature(float temperatureChange) {
	setTemperature += temperatureChange;
	applicationState = TEMPERATURE_MENU;
	uint8_t oldTemperatureBit = (uint8_t)setTemperature;
	uint8_t youngTemperatureBit = (uint8_t)((setTemperature - oldTemperatureBit) * 10);
	clock.save(TEMPERATURE_OLD_BIT, oldTemperatureBit);
	clock.save(TEMPERATURE_YOUNG_BIT, youngTemperatureBit);
	temperatureMenuTime = millis();
}

void resetApplicationState(long currentTime) {
	switch(applicationState) {
		case TEMPERATURE_MENU: {
			if (currentTime - temperatureMenuTime > TEMPERATURE_MENU_DISPLAY_TIME) {
				applicationState = RUNNING;
			}
		} break;
	}
}

float readStoredTemperature() {
	uint8_t oldTemperatureBit = clock.read(TEMPERATURE_OLD_BIT);
	uint8_t youngTemperatureBit = clock.read(TEMPERATURE_YOUNG_BIT);
	return ((float)oldTemperatureBit) + ((float)youngTemperatureBit) / 10;
}







