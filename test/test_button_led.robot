*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Resource                      ${RENODEKEYWORDS}

*** Test Cases ***
Should Handle Button Press
    Execute Command         mach create
    Execute Command         machine LoadPlatformDescription @platforms/boards/stm32f4_discovery-kit.repl
    Execute Command         sysbus LoadELF @${CURDIR}/../build/${BUILD_NAME}/bitfilled-stm32f4.elf

    Start Emulation

    ${LedState}=  Execute Command  sysbus.gpioPortD.UserLED State
    Should Be Equal         ${LedState.strip()}    False

    # Press the user button
    Execute Command         sysbus.gpioPortA.UserButton Press
    sleep                   10milliseconds

    ${LedState}=  Execute Command  sysbus.gpioPortD.UserLED State
    Should Be Equal         ${LedState.strip()}    True

    # Release the user button
    Execute Command         sysbus.gpioPortA.UserButton Release
    sleep                   10milliseconds

    ${LedState}=  Execute Command  sysbus.gpioPortD.UserLED State
    Should Be Equal         ${LedState.strip()}    False
