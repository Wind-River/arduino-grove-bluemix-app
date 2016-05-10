# End-to-end connectivity with IBM Watson IoT 

This example demonstrates how to enable end-to-end connectivity with IBM Watson IoT. The code also makes use of an Arduino style API and components from the Grove Starter Kit.

The project can be configured to build an application that includes various functionality. Configuration is controlled through the use of macros defined at the beginning of the code as follows:

 - ENABLE_GROVE_KIT
 - ENABLE_BLUEMIX
 - ENABLE_BLUEMIX_REGISTERED

By default, the ENABLE_GROVE_KIT and ENABLE_BLUEMIX macros are defined meaning that the built code will include support for components from the Grove Starter Kit (including a temperature and an LCD display). For the example code to run correctly (if the Grove Starter Kit is used) then the following should be connected:

 * the Grove Shield should be plugged onto the Galileo Gen2 board
 * plug the LCD Display into one of the I2C connectors
 * plug the temperature sensor into the A0 connector

If the ENABLE_GROVE_KIT macro is undefined then the code will be configured to generate data that will appear, within the IBM Watson IoT tools, as a sawtooth shaped wave. This will replace the temperature data published if the Grove Starter Kit is used.

If the ENABLE_BLUEMIX_REGISTERED macro is defined then the code will be configured to connect to IBM Watson IoT in Registered mode. This mode requires connection parameters provided during the IBM Watson IoT registration process and has the advantage that you are able to subscribe to commands and events transmitted by the IBM Watson IoT cloud.

If the ENABLE_BLUEMIX macro is defined and the ENABLE_BLUEMIX_REGISTERED macro is undefined (the default case) then the device will connect to IBM Watson IoT in Quickstart mode. Only a device ID is required in this case - this will be configured automatically by the code (by setting the ID to a twelve byte representation of the boards MAC address).

## More Information

Instructions for running this example are available from the  
[example wiki](https://github.com/???).
