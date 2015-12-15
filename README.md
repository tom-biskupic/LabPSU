# LabPSU
Open Source Lab Power Supply

This project is to create an open source modular lab power supply. The power supply is designed to:

* Provide between 0-15V @ 5A and 15-30V @ 3A
* Provide current limiting
* Voltage to be settable to within 1mV and current limit to 1mA resolution.
* Voltage/current readback to 1mV and 1mA with low temperature drift.
* Limit heat loss through the use of a pre-regulator. The pre-regulator uses an SCR and MOSFET arrangement to reduce noise (switches at a low frequency)
* Digitally controllable via a Raspberry PI and touch-screen interface.
* Multiple channels (designed for 3).
* Each channel is isolated and can be paralleled or put in series.
* Each channel uses its own torroidal transformer.

A Blog documenting the development of the project can be found here http://wahz.blogspot.com.au/2015/03/lab-power-supply-project.html

The major components used in the project are:

* IRFP044N N-channel MOSFET as the pass device. http://www.irf.com/product-info/datasheets/data/irfp044n.pdf
* IRF5210 P-channel MOSFET used to switch the input in the pre-regulator http://www.irf.com/product-info/datasheets/data/irf5210.pdf
* MCR-100 SCR http://www.onsemi.com/pub_link/Collateral/MCR100-D.PDF
* LT1639 Op Amps used in the control loop. http://www.linear.com/product/LT1639
* LT1716 Comparator used in the pre-regulator http://cds.linear.com/docs/en/datasheet/1716fa.pdf
* LTC2050 Op Amp used for current sense amplifier. http://www.linear.com/product/LTC2050
* AD5689R DAC with built in 2ppm/degree reference as main output voltage/current DAC. http://www.analog.com/media/en/technical-documentation/data-sheets/AD5689_5687.pdf
* AD7705 ADC used to read output voltage/current http://www.analog.com/media/en/technical-documentation/data-sheets/AD7705_7706.pdf
* ATMEGA328P Used as the main controller
* ADUM1201 dual channel isolator used to isolate the USB interface from the rest of the module http://www.analog.com/media/en/technical-documentation/data-sheets/ADuM1200_1201.pdf
* MCP2200 USB to serial converter http://www.microchip.com/wwwproducts/devices.aspx?dDocName=en546923


