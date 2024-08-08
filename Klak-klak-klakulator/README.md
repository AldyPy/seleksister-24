#TODO:
1. Make exponent flexible.
2. Make sqrt(-1) throw syntax error.
3. Clean up code.

# Klakulator
*"To reinvent the world, one wheel at a time..."*

## About
A simple calculator that implements addition `+`, subtraction `-`, multiplication `*`, division `/`, integer exponents `^`, as well as thesquare root `sqrt()` calculations, purely using bitwise operators, array functions, and string functions. There are also no for loops or while loops. The main idea of the implementation is to represent any real number as two arrays of single digit numbers: the whole part, and the fractional part.

## How To Run
Make sure you're in the `Klak-klak-klakulator` folder, then just run with Perl:

```
perl main.pl
```

# How To Use

1. Operations are always executed from left to right, regardless of operator.
```
>>> 1 + 2 * 3
9.0
```
2. Of course, you can use parentheses to determine the order of operations if you want.
```
>>> 1 + (2 * 3)
7.0
```
3. As stated in the "about" section, exponents and square roots are available operators. As you can expect, for exponents, the syntax is the same as the four basic operators. For `sqrt`, you have to place parentheses for the expression you would like to take the square root of.
```
>>> 2 ^ (-1)
0.5

>>> -1 / sqrt(16)
-0.25
```

4. Whitespace is ignored.
```
>>> -1 + 2
1.0

>>>   -  1+  2
1.0
```
5. Dividing by zero, square rooting a negative number, raising a number to a non-integer power, as well as inputting faulty syntax will cause the calculator to spew a syntax error.

```
>>> 1 / 0
[SYNTAX ERROR]

>>>  sqrt(-1)
[SYNTAX ERROR]

>>> 2 ^ 3.1
[SYNTAX ERROR]
```

6. Exponential expressions will throw a syntax error if the amount of significant figures exceeds 50 (in other words: if the result gets too large or small). However, an exception is made for numbers whose absolute value is smaller than 2. (This is to support calculation of long term percentage-based changes, like in the fourth example below. In these cases, the result is constantly truncated when calculating, which means it will not be 100% accurate, though it is still a decent approximation. Please keep in mind, the calculation times might be exceedingly long depending on the magnitude of your input).
```
>>> 99999999999999 ^ 9999999999
[SYNTAX ERROR]

>>> 50.31 ^ 16
1684492482679363054407383101.730761611187009

>>> 2 ^ 256
[SYNTAX ERROR]

>>> 2 ^ 255
57896044618658097711785492504343953926634992332820282019728792003956564819968.0

>>> 1.05 ^ 365
54211841.57783942532828282993300159746085844257730256

````

7. Enter `sctoggle` or the Konami Code `UUDDLRLRBASTART` to toggle scientific notation (it is off by default).
```
>>>  1 / 2138923791283
0.0000000000004675248384610120364571366385308372

>>>  sctoggle
>>>  1 / 2138923791283
4.675248384e-13
```

8. Enter `exit` to exit the program.

## Caveats
1. The program is slow when performing division and `sqrt`. It may take more than a few hundred miliseconds or even a few seconds for a long expression. This is probably just a `skill issue`, but I wouldn't say it's *that* bad.
2. There may or may not be a few funny results. Basically, I use Newton's method to perform division and sqrt and while the result is only slightly off, it has to be aggresively rounded in order for integer operations to make sense. 

To put it simply, in order to make *this* possible :
```
>>> sqrt((5 ^ 2) + (12 ^ 2))
13.0

>>> 28 / 2
14.0
```

I had to allow *this* :
```
>>> 1000000000 / 999999991
1.0
```

Note that the actual result is `1.000000009`. That is a difference of less than `10e-7` which is less than a `0.000000000001%` error with respect to the original operands' magnitudes. Unless this calculator is going to be used for intergalactic space travel, I'd say you can trust it.

3. I have never used Perl before so the code is quite unreadable.

## Author

|NAMA|NIM|
|-|-|
|Renaldy Arief Susanto|13522022|
