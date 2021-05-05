# Fully-Operational-Scientific-Calculator
Main objective of this project is to create a fully operational scientific calculator in C. STM32G031K8 microcontroller was used in this the project.This calculator has a keypad connected to the enter the numbers and execute basic scientific and trigonometric functions. In order to display numbers 4-digit seven segment display was used. 

### How Code Works
First of all in the main function, Keypad and ssd pins have been set and interrupt assignments
have been made. Priority assignment of interrupts is done in the same way. Columns on the keypad
are selected as input. Accordingly, rows are selected as output. When the keypad is pressed, interrupt
handlers, corresponding to the incoming interrupt have been created. To understand which key was
pressed, a continuous control mechanism was established by opening and closing rows in these
handler functions. Interupts corresponding to the numbers on the keypad were assigned those
numbers as “displaynumber1” in the code. At the same time in interrupts corresponding to numbers,
a variable called “pressCounter” was increased by one. In order to print the number pressed on the
keypad, a function called “keypad_press” was created and the digits of “displayNumber1” were
assigned according to the number of presses. This structure can be compared to a shift operation.
Looking at the value of the “pressCounter”, the number printed on the screen shifts to the left
simultaneously accordingly. As a result of these operations, a 4-digit number was printed on the
screen. Keys A, B, C, D, are operation interrupts. Addition, subtraction, multiplication and division
operations are assigned to these interrupts. The previous number on the screen should be kept in a
value called “displayNumber2”, so that operations can be made between them using the new
“displaynumber1”. In order to do this, “displaynumber2_init” function is called in operation
interrupts which stores the old “displayNumber1” data in “displayNumber2”. When the second
number is written, the equation the result can be obtained by pressing the “#” sign on the keypad.
In equals(#) interrupt, the “ssd_enter” function is called and the number obtained is written to the
screen by performing the operations between them. This enter function has the following functions:
“calculation” function, “scientific_calculation” function and number checking function. First of all
calculation function is called and operations are done according the flags corresponding to the
incoming interrupts. Since the inputs in the “calculation” function are digits, the original value is
obtained by multiplying with the digit values. In the calculation function, the operation is performed
between “displaynumber2” and “displaynumber1” and the new value is assigned to the
displaynumberfinal variable. The value of the “displayNumberFinal” is compared to whether it is
negative or not. The negative flag is set to 0 or 1, depending on the result.

After the calculation function, there is a function called “number_control”. The purpose of the
“number_control” is to assign to “displaynumber1” according to the number obtained in the
“calculation” function. Firstly, the “displayNumberFinal” that comes out of the calculation is
divided into the integer and decimal parts. If the decimal part is greater than 0, this number is a
floating number. On the other hand, floating flag assigned as 1 and if floating flag = 1 goes to the
corresponding step separation functions. If the right side is not greater than 0, this number is a
integer. If the number is integer, it goes to integer seperation function which called as
“ssd_final_number_function”. In addition, the number of digits of the “displayNumberFinal” is
found and equalized to the presscounter value. After determining whether the number is floating or
integer, the negativity feature is checked. If negative flag = 1 is set, digit separations are performed
considering the negativity feature.

At the end of everything, it goes to the “ssd_display” function rotating in the “timer1” handler.
Printing operations are performed by deciding whether the number is floating or integer and negative
or positive in the display function. By looking at the number of digits in floating numbers, the
required digit can be printed with a dot. The desired ID number on the screen is controlled in the
“ssd_display” function. In the display function, the predefined idle time variable is continuously
increased at each printing stage. If the idle time exceeds a certain value, the number displayed on
the screen returns to the id number. When the idle time reaches the desired value, the old values are
reset and the id is written to the screen. If the operations done are not to be written on the screen,
the overflowflag is synchronized to one. If overflow = 1, the display function prints “OuFL” to the
screen. If the operations performed are uncertain, invalidflag is equal to one, if invalidflag = 1, the
display function prints “InuL” on the screen.
