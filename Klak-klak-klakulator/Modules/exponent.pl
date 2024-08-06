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

    # convert power back to a regular integer
    {
        my $_power = 0;

        if (scalar(@{$power->{whole}}))
        {
            my $n = pop(@{$power->{whole}});
            $_power = _add_int($_power, $n) ;
        }
        if (scalar(@{$power->{whole}}))
        {
            my $n = pop(@{$power->{whole}});
            $_power = _add_int($_power, _mul_int($n, 10)) ;
        }
        if (scalar(@{$power->{whole}})) { die }
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
            $N = multiply($N, $N);
            goto pw ;
        }
    
    if ($sign) { $result = approximate_reciprocal($result) }
    return $result ;
}