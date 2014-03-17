# Wacky Racer 2013

### Software for ENCE461 Wacky Racer project using the AT91SAM7S.

## Authors
Stephen Collinson,
Wayne Laker,
[Jarrad Raumati](https://github.com/jarradraumati/),
Nick Wyatt.


## Description

An embedded software application written in C for the AT91SAM7S microcontroller. The original project for ENCE461 consisted of 3 PCB's: Motor/Power, Camera and Communications.

Also included in a gui-driven python application which facilitates communications to the device over bluetooth using the [Bluegiga WT12](http://www.bluegiga.com/WT12_Class_2_Bluetooth_Module) Bluetooth module.

I2C is the communications protocol between the 3 boards.

Note: Some of this code (particularly the I2C implementation) was written the the very early hours of the morning before the final inspection. Therefore comments and sanity may be lacking. This isn't even guaranteed to compile to be honest (the arm-none-eabi toolchain is a pain to get working seamlessly from my experiences).

I've released this software due to the fact that it'll just rot away on my hard drive for the next decade, and that very little of the stuff we did was available on the internet at the time (except for Hayes's work). Maybe this might be of use to someone in the future.

## Using GIT and writing code

Initial Checkout of repo

```
$ git clone --recursive git@github.com/jarradraumati/wacky_racer_2013/ ./wacky_racers`
```

You now have everything you need. Board specific targets are located in

```
./boards/camera
./boards/motor
./boards/comms
```

Any drivers you create need to be placed in

```
./wrlib/driver_name
```

You will need to create a makefile for the driver.
Use the makefile from another driver as an example.

DRIVERS is anything located within a folder in mmculib
PERIPHERALS is anything located within a folder in mat91lib

Any code you write needs to be given an application folder under

```
./apps
```

There are three main application folders, 1 for each board. Please use these
for the main program that each board will run.

Again you will need to create a small makefile to include any drivers.
Same as before except with the addition of
WR_DRIVERS which is any driver under a folder in wrlib

Any programs you make can be built/programmed/debugged the same as MPH's apps, ie

```
$ make program
$ make debug
```

Credit to [Michael Hayes](https://github.com/mph-/) from the Electrical and Computer Department at University of Canterbury for the submodules, make system and support for us to get through this project.

### Driving GIT

Updating. Change to root dir

```
$ git pull
```

Updating submodules. Change to root

```
$ git submodule foreach git pull origin master
```

Adding new files (please don't add files that make creates)

```
$ git add files
```

Commiting everything

```
$ git commit -m "Message"
```

Committing file(s)

```
$ git commit file1 file2 -m "message"
```

Checking status of local changes

```
$ git status
```

Sending your commits to the repo

```
$ git push
```
