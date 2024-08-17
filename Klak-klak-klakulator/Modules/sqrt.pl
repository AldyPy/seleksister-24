use lib '.';
# require 'Modules/Number.pm' ;
require 'Modules/basic-operators.pl' ;
# require 'Modules/multiplication.pl' ;
# require 'Modules/division.pl' ;

use integer;
use strict ;
use warnings ;

sub _sqrt
# the process is similar with division
{
    my $N = shift ;
    if ($N->to_str() eq '0.0') { return Number->new('0.0') }
    if ($N->{sign}) { die }
    $N->clean() ;

    # approx 1/sqrt(x) for each x in the range [1..10]
    my @approx_invsq = (
        '0.31622776601',    # 1 / sqrt(10)
        '1' ,               # 1 / sqrt(1)
        '0.70710678118',    # 1 / sqrt(2)
        '0.57735026919',    # 1 / sqrt(3)
        '0.5',              # 1 / sqrt(4)
        '0.4472135955',     # 1 / sqrt(5)
        '0.40824829046',    # 1 / sqrt(6)
        '0.377964473',      # 1 / sqrt(7)
        '0.35355339059',    # 1 / sqrt(8)
        '0.33333333333',    # 1 / sqrt(9)
    );

    # constants
    my $onehalf = Number->new('0.5') ;
    my $threehalfs = Number->new('1.5') ;
    my $one = Number->new('1');
    my $ten = Number->new('10');
    my $root10 = Number->new('3.16227766017');

    # normalize N
    my $magnitude = 0;
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

    # Initial 'guess'
    my $idx = pop(@{$N->{whole}})  ;
    if (!defined $idx) { $idx = 0 }
    else { push @{$N->{whole}}, $idx }
    my $x = Number->new($approx_invsq[$idx]);

    # Newton x5
    my $iterations = 5;
    newton:
        {
            my $Nx2by2 = multiply($N, $x) ;
            _truncate($Nx2by2, 10) ;
            $Nx2by2 = multiply($Nx2by2, $x) ;
            _truncate($Nx2by2, 10) ;
            $Nx2by2 = multiply($onehalf, $Nx2by2) ;

            $threehalfs = Number->new('1.5');
            $x = multiply($x, $threehalfs->subtract($Nx2by2));
            $iterations = _sub_int($iterations, 1) ;
            if ($iterations) { _truncate($x, 10) ; goto newton }
        }

    # just a chore,
    # basically magnitude is 1/10^p, and we need to calculate sqrt(1/10^p)
    my ($rootmg, $parity) = _div_mod_int_by2(_neg_int($magnitude));
    $x = mul_power_of_10($x, $rootmg);

    if ($parity) 
    {   
        $x = multiply($x, $root10) ;
    }
    
    $N = mul_power_of_10($N, $magnitude) ;
    return multiply($x, $N);
}

1;