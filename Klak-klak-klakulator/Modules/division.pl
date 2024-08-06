use lib '.';
require 'Modules/basic-operators.pl' ;
# require 'Modules/multiplication.pl' ;

use integer ;
use strict ;
use warnings ;

sub _truncate
{
    my ($N, $precision) = @_ ;
    l00p :
        if (scalar(@{$N->{frac}}) > $precision)
        {
            pop(@{$N->{frac}});
            goto l00p;
        }
    return $N;
}


sub approximate_reciprocal
# returns an approximation of 1 / N using Newton's Method: 
# First, normalize N to the form 0.XXX... * 1/(10^P). Remember P.
# (this is arbitrary step, it's just so that the first approx. can always be 1)
# Then, Yn+1 = Yn * (2 - NYn) where Yn is the n'th Newton iteration/approx.
# Last, re-multiply Y with 10^P; 
{
    my $N = $_[0] ;
    my $sign = $N->{sign} ;
    $N->{sign} = 0;

    if ($N->to_str() eq '1.0')
    {
        return $sign ? $N->negative() : $N ;
    }
    if ($N->to_str() eq '0.0') 
    {
        print "\e[0m \"NYAAAAA~ !!! P-p-pwease do nyat twai to divwide bwai zewwo againn... \" \n\0" ;
        die "division by zero" 
    }
    
    my $magnitude = 0 ;
    my $one = Number->new('1');

    shrink:
        if ($N->absolute_is_greater_than($one))
        {
            $magnitude = _sub_int($magnitude, 1) ;
            $N = mul_power_of_10($N, ~0) ;
            goto shrink ;
        }

    enlarge:
        if ($one->absolute_is_greater_than($N))
        {
            $magnitude = _inc($magnitude);
            $N = mul_power_of_10($N, 1) ;
            goto enlarge ;
        }

    $N = mul_power_of_10($N, ~0);
    $magnitude = _sub_int($magnitude, 1);

    my $Y = Number->new('1'); # initial approximation Y = 1
    my $i = 0;
    
    newton: 
    {
        my $two = Number->new('2') ;
        my $NY = multiply($N, $Y) ;
        $Y = multiply($Y, $two->subtract($NY)) ;

        $i = _inc($i) ;
        if ($i < 10) { $Y = _truncate($Y, 10) ; goto newton }
    }

    $Y = mul_power_of_10($Y, $magnitude) ;
    $Y->clean() ;

    if ($sign) {$Y->negative()}
    return $Y ;
}

sub divide
{
    my ($N1, $N2) = @_ ;
    my $aprn2 = approximate_reciprocal($N2) ;
    my $result = multiply($N1, $aprn2) ;

    round($result) ; return $result ;
}


sub round
# if the fraction has 8 consective nines, it rounds up
# e.g. -0.6999999998 becoms -0.7
# also, if the whole part is not zero, it will round 8 consecutive zeros to 0
# e.g. 13.0000000000056 becomes 13.0
{
    my $N = shift ;
    $N->clean();
    my $frac = $N->{frac} ;
    my $i = 0 ;
    my $nines = 0 ;
    my $zeros = 0 ;

    loop:
        if ($i < scalar(@{$frac}))
        {
            $zeros = ($frac->[$i] == 0 ? _inc($zeros) : 0) ;
            $nines = ($frac->[$i] == 9 ? _inc($nines) : 0) ;
            if (scalar(@{$N->{whole}}) && $zeros == 8)
            {
                splice @{$frac}, _inc($i);
                return ;
            }

            if ($nines == 8)
            {
                splice @{$frac}, _inc($i);

                if ($N->{sign})
                {
                    $N = $N->add(mul_power_of_10(Number->new('-0.1'), _neg_int($i))) ;
                }
                else 
                {
                    $N = $N->add(mul_power_of_10(Number->new('0.1'), _neg_int($i))) ;
                }
                return ;
            }

            $i = _inc($i) ;
            goto loop ;
        }
    
    return ;
}

sub scientific_notation
{
    my $N = shift ;
    $N->clean() ;
    
    my $significant_digits = _add_int(
                                      scalar(@{$N->{whole}}), 
                                      scalar(@{$N->{frac}})
                                     );

    if ($significant_digits > 9)
    {
        my $magnitude = 0;
        my $one = Number->new('1');
        my $ten = Number->new('10');

        shrink :
        if ($N->absolute_is_greater_than($ten))
            {
                $N = mul_power_of_10($N, ~0)  ;
                $magnitude = _inc($magnitude) ;
                goto shrink ;
            }

        enlarge:
            if ($one->absolute_is_greater_than($N))
            {
                $N = mul_power_of_10($N, 1) ;
                $magnitude = _sub_int($magnitude, 1);
                goto enlarge ;
            }
        
        splice @{$N->{frac}}, 9 ;

        if ($magnitude)
            { return $N->to_str() . 'e' . $magnitude }
        else
            { return $N->to_str() }
    }

    else { return $N->to_str() }
}
1;