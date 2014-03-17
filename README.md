# Wacky Racer 2013

### Software for ENCE463 Wacky Racer project using the AT91SAM7S.

## Authors
[Jarrad Raumati](https://github.com/jarradraumati/),
Stephen Collinson,
Wayne Laker,
[Michael Hayes](https://github.com/mph-/).

## Description

An embedded software application written in C for the AT91SAM7S microcontroller. The original project for ENCE463 consisted of 3 PCB's: Motor/Power, Camera and Communications.



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

Credit to [Michael Hayes](https://github.com/mph-/) from the Electrical and Computer Department at University of Canterbury for the submodules and make system.

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
