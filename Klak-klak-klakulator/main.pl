use lib 'Modules'       ;
require 'Number.pm'     ;
require 'basic-operators.pl' ;
require 'multiplication.pl' ;
require 'division.pl' ;
require 'sqrt.pl' ;
require 'exponent.pl' ;

use strict;
use warnings;

# ANSI codes
my $Red     = "\e[1;31m";      # Red
my $Green   = "\e[1;32m";      # Green
my $blue    = "\e[0;36m";      # Blue
my $Blue    = "\e[1;36m";      # Blue Bold
my $White   = "\e[1;37m";      # White
my $reset   = "\e[0m";         # Reset color
my $ctrlu   = "\e[4m";         # Underline

# my $N = <STDIN> ;
# chomp $N ;
# $N = Number->new($N) ;
# print _sqrt($N)->to_str() ;


sub remove_whitespace
{
    my $s = $_[0] ;
    my ($i, $j) = (0, 0) ;
    my $n = length($s) ;
    
    rmwhitesp :
        if ($i < $n)
        {
            my $char = substr($s, $i, 1) ;
            if ($char ne ' ')
            {
                substr($s, $j, 1, $char);
                $j = _inc($j) ;
            }

            $i = _inc($i) ;
            goto rmwhitesp ;
        }
    
    return substr($s, 0, _sub_int($j, 1)) ;
}


sub evaluate
{
    my $expr = $_[0];
    my $length = length($expr);
    my $ch;
    my @nums = ();
    my @ops = ();
    my $current_number = '';
    my $i = 0;
    my $numerator;
    my $denominator;
    
    eval 
    {   

        inp :

            if ($i == $length) { goto stop_inp }
            
            $ch = substr($expr, $i, 1) ;
            $i = _inc($i) ;
            if ((_to_int($ch) <= 9 && _to_int($ch) >= 0) || $ch eq '.') 
            {
                $current_number = $current_number . $ch ;
            }
            else
            {
                if ($ch eq '-' and $current_number eq '') {$current_number = '-'}
                elsif ($ch eq '-' and $current_number eq '-') {$current_number = ''}
                elsif ($current_number eq '') { die }
                else 
                {
                    push @nums, Number->new($current_number) ;
                    push @ops, $ch ;
                    $current_number = '';
                }
            }

            goto inp ;

        stop_inp :
        if ($current_number ne '') { push @nums, Number->new($current_number) }

        @nums = reverse @nums   ;
        @ops = reverse @ops     ;
        $numerator = pop(@nums) ;
        $denominator = Number->new("1");

        calculate :
            if (scalar(@ops))
            {
                $ch = pop(@ops) ;
                if ($ch eq '+')
                {
                    my $N = pop(@nums) ;
                    $numerator = $numerator->add(multiply($N, $denominator)) ;
                }
                elsif ($ch eq '-')
                {
                    my $N = pop(@nums) ;
                    $numerator = $numerator->subtract(multiply($N, $denominator)) ;
                }
                elsif ($ch eq '*')
                {
                    my $N = pop(@nums) ;
                    $numerator = multiply($N, $numerator) ;
                }
                elsif ($ch eq '/')
                {
                    my $N = pop(@nums) ;
                    $denominator = multiply($N, $denominator) ;
                }
                elsif ($ch eq '^')
                {
                    my $N = pop(@nums) ;
                    my $Ncopy = Number->new($N->to_str()) ;
                    $numerator = exponent($numerator, $N) ;
                    $denominator = exponent($denominator, $Ncopy) ;
                }

                print $numerator->to_str();
                print "\n";
                print $denominator->to_str();
                print "\n";
                goto calculate ;
            }

    };

    if ($@) {die}
    return divide($numerator, $denominator) ;
}

my $sci_mode = 0;
my $result = undef;

mainloop :
{
     eval 
     {   
        my $ch ;
        print "${White}>>>  "   ;
        my $expr = <STDIN>      ;
        if ($expr eq "\n") {goto mainloop}

        $expr = remove_whitespace($expr) ;
        if ($expr eq "exit") { print $reset ; exit}
        if (($expr eq "UUDDLRLRBASTART") or ($expr eq "sctoggle")) 
        { 
            $sci_mode = !$sci_mode ; 
            goto mainloop;
        }

        my @stack = () ;
        my $current_expr = '' ;
        my $i = 0 ;
        my $length = length($expr) ;

        inp :
            if ($i < $length)
            {
                $ch = substr($expr, $i, 1) ;
                
                if (substr($expr, $i, 5) eq 'sqrt(')
                {
                    push @stack, $current_expr ;
                    push @stack, "sqrt" ;
                    $i = _add_int($i, 4);
                    $current_expr = '';
                }
                elsif ($ch eq "(") 
                {

                    push @stack, $current_expr ;
                    $current_expr = '';
                }
                elsif ($ch eq ")")
                {
                    if ($current_expr eq '') { die }
                    my $N = evaluate($current_expr) ;
                    
                    if (!scalar(@stack)) { die }
                    my $top = pop(@stack) ;
                    if ($top eq "sqrt") 
                    { 
                        $N = _sqrt($N) ;
                        $top = pop(@stack) ;
                    }

                    $current_expr = $top . $N->to_str() ;
                }
                else { $current_expr = $current_expr . $ch }
                
                $i = _inc($i) ;
                goto inp ;
            }
        
        if (scalar(@stack)) { die }
        $result = evaluate($current_expr) ;
        
        outputs:
        {
            print "${ctrlu}${Blue}" ;
            if ($sci_mode)
            {
                print scientific_notation($result) ;
            }
            else
            {
                print $result->to_str() ;
            }
            print "${reset}\n\n";
        }
    };

    if ($@) 
    { 
        # print ($@) ;
        print "${Red}[SYNTAX ERROR]\n\n${reset}"
    }

    goto mainloop ;
}