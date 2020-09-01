# Naming and other notes #
Naming is always a contentious issue in teams and the following presents
some useful insights.  There are two hopefully somewhat compatible options,
the first is presented in the Barr Group (Similar to MISRA) coding standard and the second is
for people who are more comfortable it the OOP world like JAVA, C# or more to the point python PEP8.
At the end of the day, the goal is to make it clear what the code does for future maintainers. 

I have included the idea of borrowing from PEP8 here because if you are going to learn a coding standard PEP8 is good and it is much more pedantic and enforced than most. Also it is not uncommon for developers to use both C and python in the same project.




## Modules ##
lower case, numbers and underscores
try for first 8 unique
any module with a main function should have main in its name

## Data Types ##
### Barr ###
Lower case and underscores and end with a _t
All structs, unions, and enums will be name via typedef

### Class designator/PEP8 ###
For structure types that represent a module class, Capitalized camel case may be used.

#### more ####
Structure or enum tags should be the same as typedef names. (Like C++)

Always use Fixed Width Integers from <stdint.h>
Use float32_t, float64_t and float128_t, instead of double and float.


## Procedures ##
In general function names should be designed to serve two purposes:

	1) Make it clear what the function does
	2) Make it clear what module it is related to
	
To that end, Public names should be prefixed with module or pseudo class name.

Verbs like get, set, is, enable, disable, etc. are encouraged.

### Barr ###
lower case with underscores

### Class designator/PEP8 ###
Mix of Camel case and underscore 

#### more ####
If a procedure is not public make it static with a predefined STATIC macro that we will set globally to static in our config.h file.  This is to facilitate unit testing of potentially static functions.

I think it is ok to prefix module static functions with an underscore especially when they act on pseudo objects.

If doing pseudo object style, the first parameter of the procedure must be a pointer to the pseudo object type.  This parameter should probable be named **self**.

Parameters should use the const specifier whenever possible to limit access or to show intent.

## Variables ##
lower case with underscores.

At least 3 characters long unless it is a loop defined index.  If the loop variable needs to persist beyond the loop, give it a good name.

pseudo object pointers in pseudo class methods should use **self** rather than *this* because *this* is a keyword in C++.

File scope variables should use the STATIC prefix to limit scope and aid in testing.

Document with comments any block scope static variables. 

In general variables should be initialized as soon as possible near their point of definition.


## Final Notes on coding ##
Embedded engineers often have an obsession with code size and speed.
This can lead to writing shorthand code that the engineer knows is optimal or produces fewer code lines.
Remember that modern compilers know more than you do about optimization.
Don't hand optimize code or use tricks when first writing your code. Make it clear what you intend.  
For example if you are doing math and need to divide by 8, use the divide.
If you are doing bit manipulation and you need to shift right by 3 use the right shift.  We all know they are the same operation. The compiler will emit the best code it can.

Document your invariants with asserts if it makes your intent clearer. Don't rely on them at runtime but they may be additionally useful in your unit tests.  They are often better than comments because they are easier to maintain.




