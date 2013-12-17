
DEBUG MODE
===========================================

To test the protocol on a desktop, a DEBUG profile has been defined to use FIFOs for communications, that **must** be created in the current folder :
   mkfifo to_arduino to_raspberry


RASPBERRY
===========================================

The following commands should be run in the ./raspberry folder.

Compilation
-----------

To execute:
   make run

To execute in DEBUG mode (no serial communication):
   make DEBUG=1 run


ARDUINO
===========================================

The following commands should be run in the ./atmega8 folder.

Compilation
-----------

To compile and send the program to the arduino:
   make send

To execute in DEBUG mode (gcc - desktop mode, no serial):
   make DEBUG=1 && build/serialCom.out
