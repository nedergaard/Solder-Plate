#include "PidController.h"
#include "globals.h"
#define GATE_PIN 17
#include "voltageReference/AnalogRef.h"
#include "thermistors/TemperatureController.h"

extern AnalogRef analogRef;

extern TemperatureController tempController;

PidController::PidController(PidControllerData *data)
{

    this->data = data;

    controller.begin(&(this->data->currentTemp), &(this->data->setPoint), &(this->data->targetTemp), PID_P, PID_I, PID_D);
    //  controller.reverse();
    controller.setOutputLimits(PID_OUTPUT_MIN, PID_OUTPUT_MAX);
    controller.setSampleTime(PID_SAMPLE_TIME);
    controller.setWindUpLimits(PID_WINDUP_MIN, PID_WINDUP_MAX);
}

double *PidController::compute()
{

    controller.compute();

    return controller.output;
}

void PidController::debug()
{

    controller.debug(&Serial, " ", PRINT_INPUT |      // Can include or comment out any of these terms to print
                                       PRINT_OUTPUT | // in the Serial plotter
                                       PRINT_SETPOINT | PRINT_BIAS | PRINT_P | PRINT_I | PRINT_D);
}

void PidController::loop()

{
    data->targetTemp = chosenReflowProfile.getTargetTemp();

    // data->currentTemp = thermistor1.getTemperature();

    data->currentTemp = tempController.getPlateTemperature();

    // float sysVoltage = analogRef.calculateSystemVoltage();

    float inVoltage = analogRef.calculateInputVoltage();
    compute();

    // Reverseal of the PWM value is needed because the PID controller is set to work with a normal direction, Creates for a better PID control
    // 255= off, 0=full power
    analogWrite(GATE_PIN, 255 - data->setPoint);
}

void PidController::stop()
{
    data->targetTemp = 0; // should not be needed but why not? --- Its needed it randomly starts again and sometimes goes to heat the plate
                          // STOP --> Always use analog write for sanity- Keep PWM pins labled analogWrite -- Writing it LOW will KEEP IT ON

    analogWrite(GATE_PIN, 255); // VERY IMPORTANT, DONT CHANGE!
    controller.reset();
    controller.stop();
}

void PidController::start()
{
    controller.start();
}

double *PidController::getInput()
{
    return controller.input;
}
