**drvchr this is Linux character driver example**

This example implements the basic callbacks for working with a device file: open, release, read and write.
Declares a class and creates a device file ```devchr```.
This driver does not interact with the hardware, it can only write and read the data buffer in the kernel space. and transmit data to the user space.

To build the driver, you need to run the ```make``` command.

To build the test, you need to run ```make test```.
