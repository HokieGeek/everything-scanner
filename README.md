# Olive's Scanner [![Build Status](https://travis-ci.org/HokieGeek/olive-scanner.svg?branch=master)](https://travis-ci.org/HokieGeek/olive-scanner)

Toy I am making my toddler so that when she touches an object the "scanner" vibrates and lights up. Should second as a flashlight, of sorts.

### Prototype

![Current](http://i.imgur.com/xCZDH0e.jpg)

## Design

![Schematics](https://raw.githubusercontent.com/HokieGeek/olive-scanner/master/schematics/olive-scanner.png)
![Board](https://raw.githubusercontent.com/HokieGeek/olive-scanner/master/schematics/olive-scanner.brd.png)

## Parts list

* [ATtiny13A](http://www.atmel.com/images/doc8126.pdf) - The driver of the whole thing
* [MCP23S08](http://www.mouser.com/ds/2/268/21919b-65915.pdf) - SPI 8-bit I/O expander used to drive just the LEDs
* [SparkFun's LED Rainbow Pack](https://www.sparkfun.com/products/13767) - Got it in a recent order for free as a christmas giveaway
* [Vibrator motor from an old phone]() - Took it out of my waterlogged MotoX :-D
* [CdS Photocell](http://cdn.sparkfun.com/datasheets/Sensors/LightImaging/SEN-09088.pdf) - When she touches this sensor to an object, the LEDs and motor activate
