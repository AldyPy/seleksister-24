use lib '.';
require 'Modules/basic-operators.pl' ;

use strict ;
use warnings ;
use integer ;

sub mul_power_of_10
# returns N * 10^p
{
    my ($N, $p) = @_ ;

    my $whole = $N->{whole};
    my $frac = $N->{frac};

    # return $N ;

    if ($p > 0)
    {
        my $i = 0 ;
        loop15 :
            my $digit = (scalar(@{$frac}) ? shift (@{$frac}) : 0) ;
            push @{$whole}, $digit ;
            $i = _inc($i) ;
            if ($i < $p) { goto loop15 }

        return $N ;
    }

    elsif ($p == 0) 
    { 
        return $N ;
    }
    
    else
    {
        my $i = 0 ;
        loop404 :
            my $digit = (scalar(@{$whole}) ? pop(@{$whole}) : 0) ;
            unshift @{$frac}, $digit ;
            $p = _inc($p) ;
            if ($p < $i) { goto loop404 }
        return $N ;
    }
}

sub multiply
# this is basically the paper multiplication method we learn in elementary school
{
    my ($N1, $N2) = @_;
    my $sign = $N1->{sign} ^ $N2->{sign} ;
    $N1->clean();
    $N2->clean();

    if ($N1->{sign}) { $N1->{sign} = 0 }
    if ($N2->{sign}) { $N2->{sign} = 0 }

    my $power1 = scalar(@{$N1->{frac}})  ;
    my $power2 = scalar(@{$N2->{frac}})  ;

    $N1 = mul_power_of_10($N1, $power1) ;
    if ($N1 != $N2)
        { $N2 = mul_power_of_10($N2, $power2) }

    my @arr = () ;
    my @N1_digits = @{$N1->{whole}}  ;
    my @N2_digits = @{$N2->{whole}}  ;
    
    my $cur_pow = 0;
    loop186:
        if (scalar(@N2_digits))
        {
            my @current_result_digits = () ;
            my $digit2 = pop(@N2_digits) ;
            my $carry = 0 ;
            my $i = scalar(@N1_digits) ;
            $i = _sub_int($i, 1) ;
            if ($i < 0) { goto endloop187 }
            loop187 :
            {
                my $digit1 = $N1_digits[$i] ;
                my $cur = _mul_int($digit1, $digit2) ;
                $cur = _add_int($carry, $cur) ;
                ($carry, $cur) = _div_mod_int($cur, 10) ;
                $i = _sub_int($i, 1);
                unshift @current_result_digits, $cur ;
                if ($i >= 0) { goto loop187 }
            }
            endloop187:
            
            if ($carry)
            {
                unshift @current_result_digits, $carry ;
            }


            # yes I know this code is cancer,
            # but I can't be bothered to look up Perl's syntax sugar.
            # if it works, it works.
            my @empty_array = () ;
            
            my $current_result = Number->new('0') ;
            $current_result->set_num(\@current_result_digits, \@empty_array) ;
            $current_result = mul_power_of_10($current_result, $cur_pow) ;
            $cur_pow = _inc($cur_pow) ;
            push @arr, $current_result ;
            
            goto loop186;
        }
    
    my $result = Number->new('0.0') ;
    loop132:
        if (scalar(@arr))
        {
            my $c = pop(@arr) ;
            $result = $result->add($c) ;
            goto loop132;
        }

    # reset objects so they be can reused outside the function
    $N1 = mul_power_of_10($N1, _neg_int($power1));
    if ($N1 != $N2)
        { $N2 = mul_power_of_10($N2, _neg_int($power2)) }

    $result = mul_power_of_10($result, _neg_int(_add_int($power1, $power2))) ;
    $result->{sign} = $sign ;

    return $result ;
}

1;