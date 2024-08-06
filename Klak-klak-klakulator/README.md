# Klakulator
*"To reinvent the world, one wheel at a time..."*

## About
A simple calculator that implements addition `+`, subtraction `-`, multiplication `*`, division `/`, integer exponents `^`, as well as thesquare root `sqrt()` calculations, purely using bitwise operators, array functions, and string functions. There are also no for loops or while loops. The main idea of the implementation is to represent any real number as two arrays of single digit numbers: the whole part, and the fractional part.

## How To Run
Make sure your in the `Klak-klak-klakulator` folder, then just run with Perl:

```
perl main.pl
```

# How To Use

1. Operations are always executed from left to right, regardless of operator.
```
>>> 2 + 1 / 1
3.0
```
2. Of course, you can use parentheses to determine the order of operations if you want.
```
>>> 2 + (1 / 1)
1.0
```
3. As stated in the "above" section, exponents and square roots are available operators. As you can expect, for exponents, the syntax is the same as the four basic operators. For `sqrt`, you have to place parentheses for the expression you would like to take the square root of.
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

6. Exponential expressions will throw a syntax error if the result is extremely large.
```
>>> 99999999999999 ^ 9999999999
[SYNTAX ERROR]

>>> 2 ^ 100
[SYNTAX ERROR]

>>> 2 ^ 99
633825300114114700748351602688.0

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

In other words, to make *this* possible :
```
>>> sqrt((5 ^ 2) + (12 ^ 2))
13.0

>>> 28 / 2
14.0
```

I have to allow *this* :
```
>>> 1000000000 / 999999991
1.0
```

The actual result is `1.000000009`. That is a difference of less than `10e-7` which is less than a `0.000000000001%` error with respect to the original operands' magnitudes. Unless this calculator is going to be used for intergalactic space travel, I think you can trust it.

## Author

|NAMA|NIM|
|-|-|
|Renaldy Arief Susanto|13522022|