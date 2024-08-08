use lib '.';
require 'Modules/basic-operators.pl' ;

use integer ;
use strict ;
use warnings ;

sub exponent
{
    my ($N, $power) = @_ ;
    $power->clean() ;
    if (scalar(@{$power->{frac}})) { die }
    my $sign = $power->{sign};

    my $is_int = !scalar(@{$N->{frac}}) ;

    # convert power back to a regular integer
    {
        my $_power = 0;

        # ones digit
        if (scalar(@{$power->{whole}}))
        {
            my $n = pop(@{$power->{whole}});
            $_power = _add_int($_power, $n) ;
        }
        
        # tens digit
        if (scalar(@{$power->{whole}}))
        {
            my $n = pop(@{$power->{whole}});
            $_power = _add_int($_power, _mul_int($n, 10)) ;
        }

        # hundreds digit
        if (scalar(@{$power->{whole}}))
        {
            my $n = pop(@{$power->{whole}});
            $_power = _add_int($_power, _mul_int($n, 100)) ;
        }

        # if |power| > 999 and |N| > 2, too large
        my $two = Number->new('2.0');
        if (scalar(@{$power->{whole}}) && $N->absolute_is_greater_than($two)) 
        {
            die ;
        }

        $power = $_power ;
    }

    # iterative logN exponentiation
    my $result = Number->new('1.0');
    my $zero = Number->new('0.0');

    pw :
        if ($power)
        {
            my ($q, $r) = _div_mod_int_by2($power) ;
            if ($r) { $result = multiply($result, $N) }
            $power = $q ;
            if (_add_int(scalar(@{$N->{whole}}), scalar(@{$N->{frac}})) >= 50)
            {
                if ($is_int)
                    { die }
                else
                    { _truncate($N, 15) ; _truncate($result, 15) }
            }
            $N = multiply($N, $N);
            goto pw ;
        }
    
    if ($sign) { $result = approximate_reciprocal($result) }
    return $result ;
}