#include "main.h"
using namespace std;

class ServoController
{
private:
    uint slice;

private:
    uint channel;

private:
    float minMillis;

private:
    float maxMillis;

private:
    float minAngle;

private:
    float maxAngle;

private:
    float angleMultiplier;

public:
    int currentMillis;

public:
    float currentAngle;

public:
    bool increaseAngle = false;

public:
    bool decreaseAngle = false;

public:
    uint increasePin;

public:
    uint decreasePin;

public:
    ServoController(int pwm_pin, uint upPin, uint downPin, float leastAngle = 0.f, float largestAngle = 180.f, float floorMillis = 250.f, float cielMillis = 1275.f)
    {

        minAngle = leastAngle;
        maxAngle = largestAngle;
        minMillis = floorMillis;
        maxMillis = cielMillis;
        currentMillis = floorMillis;
        currentAngle = leastAngle;
        increasePin = upPin;
        decreasePin = downPin;

        gpio_set_function(pwm_pin, GPIO_FUNC_PWM); /// Set the pin number sent to be PWM
        slice = pwm_gpio_to_slice_num(pwm_pin);
        channel = pwm_gpio_to_channel(pwm_pin);

        pwm_set_clkdiv(slice, 256.0f); /// Setting the divider to slow down the clock
        pwm_set_wrap(slice, 9804);     /// setting the Wrap time to 9764 (20 ms)
        pwm_set_enabled(slice, true);

        pwm_set_chan_level(slice, channel, currentMillis); /// Setting the duty period (0.6 ms)

        setupAngleServoUpdateByAngle();

        // setupButtonInterops(increasePin);
    }

private:
    void setupAngleServoUpdateByAngle()
    {
        try
        {

            if (maxAngle - minAngle == 0)
                throw "0 value denominator detected";

            angleMultiplier = (maxMillis - minMillis) / (maxAngle - minAngle);
        }
        catch (string message)
        {
            cout << "Could not set up angle multiplier to set Servo by Angle" << '\n';
            cout << message << '\n';
        }
    }

private:
    void setServoAngleByMill(int mill)
    {
        if (mill >= minMillis && mill <= maxMillis)
            pwm_set_chan_level(slice, channel, mill);
        currentMillis = mill;
    }

public:
    void setServoAngleByAngle(float angle)
    {
        try
        {

            if (angle < minAngle || angle > maxAngle || !angleMultiplier)
                return;

            currentMillis = (angleMultiplier * angle) + minMillis;

            pwm_set_chan_level(slice, channel, currentMillis);

            currentAngle = angle;
        }
        catch (string message)
        {
            cout << message << '\n';
        }
    }
    // rate is the number of milliseconds between single servo angle moves
public:
    void moveToAngleAtAnglePerTimeRate(int angle, int rate)
    {
        try
        {

            int angleDifference = currentAngle - angle;

            if (angleDifference > 0)
            {
                for (int i = currentAngle; i >= angle; i--)
                {
                    setServoAngleByAngle(i);
                    sleep_ms(rate);
                };
            }
            if (angleDifference < 0)
            {
                for (int i = currentAngle; i <= angle; i++)
                {
                    setServoAngleByAngle(i);
                    sleep_ms(rate);
                }
            }
            delete &angleDifference;
        }
        catch (...)
        {
            throw 2;
        }
    }

public:
    void setIncreaseAngleOption(uint gpio, uint32_t event)
    {
        try
        {
            if (gpio == increasePin && event == 4)
                increaseAngle = true;
            else if (gpio == increasePin && event != 4)
                increaseAngle = false;
        }
        catch (...)
        {
            throw 2;
        }
    }

public:
    void setDecreaseAngleOption(uint gpio, uint32_t event)
    {
        try
        {
            if (gpio == decreasePin && event == 4)
                decreaseAngle = true;
            else if (gpio == decreasePin && event != 4)
                decreaseAngle = false;
        }
        catch (...)
        {
            throw 2;
        }
    }

public:
    void actOnButtonState()
    {
        try
        {
            if (increaseAngle == true && decreaseAngle == true)
                return;

            if (increaseAngle == true)
            {
                float newAngle = currentAngle + 1.0f;
                setServoAngleByAngle(newAngle);
            }
            if (decreaseAngle == true)
            {
                float newAngle = currentAngle - 1.0f;
                setServoAngleByAngle(newAngle);
            }
        }
        catch (...)
        {
            throw 2;
        }
    }
};
