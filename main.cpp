/*
 * ButtonControlledArm for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author     Justin Berkshire
 * @copyright   2022
 * @licence     MIT
 *
 */
#include "main.h"
#include <iostream>
#include "./servoControl.cpp"
using namespace std;

int buttonList [8] = { 5, 6, 7, 8, 9, 10, 11, 12};

uint32_t mostrecentButtonPress;
uint pinPushed;

ServoController base(0, 5, 6, 0.0f, 180.0f, 250.0f);
ServoController armCantaLever(1, 7, 8, 0.0f, 100.0f, 225.0f);
ServoController armMain(2, 9, 10, 0.0f, 80.0f, 250.0f);
ServoController claw(3, 11, 12, -2.0f, 180.0f, 200.0f);

void buttonPressed(uint gpio, uint32_t events) {
    mostrecentButtonPress = events;
    pinPushed = gpio;
    base.setIncreaseAngleOption(gpio, events);
    base.setDecreaseAngleOption(gpio, events);
    armCantaLever.setIncreaseAngleOption(gpio, events);
    armCantaLever.setDecreaseAngleOption(gpio, events);
    armMain.setIncreaseAngleOption(gpio, events);
    armMain.setDecreaseAngleOption(gpio, events);
    claw.setIncreaseAngleOption(gpio, events);
    claw.setDecreaseAngleOption(gpio, events);
};


void setupButtons() {
    //initialize button pins and set them to in
    for(int x = 0; x < 8; x ++) {
        gpio_init(buttonList[x]);
        gpio_set_dir(buttonList[x], GPIO_IN);
        gpio_pull_up(buttonList[x]);
    };

    //set up initial callback so that we don't have to impliment  branching logic in the next loop
    gpio_set_irq_enabled_with_callback(buttonList[0], GPIO_IRQ_EDGE_FALL, true, &buttonPressed);
    gpio_set_irq_enabled(buttonList[0], GPIO_IRQ_EDGE_RISE, true);

    //set up interrupts for the remaining buttons;
    for(int x = 1; x < 8; x++) {
        gpio_set_irq_enabled(buttonList[x], GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(buttonList[x], GPIO_IRQ_EDGE_RISE, true);
    }

}

int main()
{

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    setupButtons();

    gpio_put(LED, 1);

    while (true)
    {
        base.actOnButtonState();
        armCantaLever.actOnButtonState();
        armMain.actOnButtonState();
        claw.actOnButtonState();
        // int button = gpio_get_dir(5);
        sleep_ms(30);
    }

    return 0;
}
