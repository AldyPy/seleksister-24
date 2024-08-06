# The "Number" class. Refers to real numbers.
# A number is represented as it's whole and fractional parts.
# They are each implemented as an array of single digit integers.
package Number;

require 'basic-operators.pl' ;

use strict;
use warnings;

sub new {
    
    my ($class, $s) = @_;
    my $len = length($s) ;
    
    my @whole = () ;
    my @frac = () ;
    my $sign = 0;

    my $i = 0, my $is_frac = 0;
    if (substr($s, 0, 1) eq '-')
    {
        $sign = 1 ;
        $i = 1;
    }

    loop0:
        my $ch = substr($s, $i, 1);
        if ($ch eq '.') { $is_frac = 1 }
        else
        {
            my $n = _to_int($ch);
            if ($is_frac)
            {
                push @frac, $n ;
            }
            else
            {
                push @whole, $n ;
            }
        }
        $i = _inc($i) ;
        if ($i == $len) { goto end_loop0 } ;
        goto loop0 ;
    
    end_loop0 :

    my $self = {
        whole => \@whole,
        frac => \@frac,
        sign => $sign
    };

    bless $self, $class;
    return $self;
}

sub clean
# just getting rid of leading and trailing zeros
{
    my $self = shift ;
    my $frac = $self->{frac};
    my $whole = $self->{whole};

    if (scalar(@{$whole}))
    {
        cleanleading:
            my $n = shift(@{$whole}) ;
            if ($n != 0)
            {
                unshift @{$whole}, $n ;
                goto endcleanleading ;
            }
            if (scalar(@{$whole}) > 0) { goto cleanleading }
        endcleanleading:
    }

    if (scalar(@{$frac}))
    {
        cleantrailing:
            my $n = pop(@{$frac}) ;
            if ($n != 0)
            {
                push @{$frac}, $n ;
                goto endcleantrailing ;
            }
            if (scalar(@{$frac}) > 0) { goto cleantrailing }
        endcleantrailing:
    }
}

sub to_str
{
    my $self = shift ;
    $self->clean();

    my $self_frac = $self->{frac};
    my $self_whole = $self->{whole};

    my $sign = ($self->{sign} ? '-' : '') ;
    my $lf = scalar(@{$self_frac}) ;
    my $lw = scalar(@{$self_whole}) ;
    if (!($lf) && !($lw)) { return '0.0' }
    if (!($lw)) { return $sign . '0.' . join('', @{$self_frac}) }
    if (!($lf)) { return $sign . join('', @{$self_whole}) . '.0' }
    
    return $sign . join('', @{$self_whole}) . '.' . join('', @{$self_frac}) ;
}

sub set_num {
    my ($self, $whole, $frac) = @_;
    $self->{whole} = $whole;
    $self->{frac} = $frac;
}

sub negative
{
    my $self = shift;
    $self->{sign} = !$self->{sign};
    return $self ;
}

#
# Add and subtract are implemented here. The other operations will have their own file
#

sub absolute_is_greater_than
# returns |self| > |other|
{

    my ($self, $other) = @_;
    $self->clean();
    $other->clean();
    
    my $self_frac = $self->{frac};
    my $other_frac = $other->{frac};
    my $self_whole = $self->{whole};
    my $other_whole = $other->{whole};
    
    if (scalar(@{$self_whole}) != scalar(@{$other_whole})) 
    { 
        return scalar(@{$self_whole}) > scalar(@{$other_whole})
    }

    # skip to first nonzero digits
    my $i = 0 ;
    my $j = 0 ;
    skip1 :
        if ($i < scalar(@{$self_whole}) && $self_whole->[$i] == 0)
        {
            $i = _inc($i) ;
            goto skip1 ;
        }
    skip2 :
        if ($j < scalar(@{$other_whole}) && $other_whole->[$j] == 0)
        {
            $j = _inc($j) ;
            goto skip2 ;
        }
    
    # compare whole part
    if ($i == scalar(@{$self_whole}) || $j == scalar(@{$other_whole}))
    {
        goto endcmpw ;
    }
    cmpw :
        if ($self_whole->[$i] != $other_whole->[$j])
        {
            return $self_whole->[$i] > $other_whole->[$j] ;
        }
        $i = _inc($i) ;
        $j = _inc($j) ;
        if ($i < scalar(@{$self_whole}) && $j < scalar(@{$other_whole})) { goto cmpw }
    endcmpw:

    my $both_end_reached = ($i == scalar(@{$self_whole}) && $j == scalar(@{$other_whole})) ;
    if (!$both_end_reached)
    {
        return ($i < scalar(@{$self_whole})) ;
    }

    # compare fraction part
    $i = 0 ;
    my $len = ( scalar(@{$self_frac}) < scalar(@{$other_frac}) ? 
                scalar(@{$self_frac}) : scalar(@{$other_frac})) ;

    if (!$len) {return scalar(@{$self_frac}) > scalar(@{$other_frac})}
    cmpf :
        if ($self_frac->[$i] != $other_frac->[$i])
        {
            return $self_frac->[$i] > $other_frac->[$i] ;
        }
        $i = _inc($i) ;
        if ($i < $len) { goto cmpf }
    
    return scalar(@{$self_frac}) > scalar(@{$other_frac}) ;
}


sub absolute_sum
# returns |self| + |other|
# (result sign follows self's sign)
{
    my ($self, $other) = @_;

    my @self_frac = @{$self->{frac}};
    my @other_frac = @{$other->{frac}};
    my @self_whole = @{$self->{whole}};
    my @other_whole = @{$other->{whole}};

    my @result_whole = () ;
    my @result_frac = () ;

    my $carry = 0;
    my ($cur, $sf, $of, $sw, $ow) ;
    addf:
        $sf = (scalar(@self_frac) >= scalar(@other_frac) ? pop(@self_frac) : 0) ;
        $of = (scalar(@other_frac) > scalar(@self_frac) ? pop(@other_frac) : 0) ;
        if (!defined $sf) {$sf = 0}
        $cur = _add_int($sf, $of) ;
        $cur = _add_int($cur, $carry) ;
        if ($cur > 9)
        {
            $carry = 1;
            $cur = _sub_int($cur, 10);
        } else { $carry = 0 }

        unshift @result_frac, $cur ;
        if (scalar(@self_frac) || scalar(@other_frac)) { goto addf }
    
    addw:
        $sw = (scalar(@self_whole) > 0 ? pop(@self_whole) : 0) ;
        $ow = (scalar(@other_whole) > 0 ? pop(@other_whole) : 0) ;
        $cur = _add_int($sw, $ow) ;
        $cur = _add_int($cur, $carry) ;
        if ($cur > 9)
        {
            $carry = 1;
            $cur = _sub_int($cur, 10);
        } else { $carry = 0 }
        
        unshift @result_whole, $cur ;
        if (scalar(@self_whole) || scalar(@other_whole)) { goto addw }
    
    if ($carry) { unshift @result_whole, 1 }

    # @result_frac = reverse @result_frac ;
    # @result_whole = reverse @result_whole ;

    $self->set_num(\@result_whole, \@result_frac) ;
    return $self ;
}


sub absolute_difference
# returns |(|self| - |other|)|
# result sign is always positive
{
    my ($self, $other) = @_;
    if ($other->absolute_is_greater_than($self))
    {
        my $temp = $self;
        $self = $other;
        $other = $temp;
    }

    my @self_frac = @{$self->{frac}};
    my @other_frac = @{$other->{frac}};
    my @self_whole = @{$self->{whole}};
    my @other_whole = @{$other->{whole}};

    my @result_whole = () ;
    my @result_frac = () ;

    my $carry = 0;
    my ($cur, $sf, $of, $sw, $ow) ;
    subf:
        $sf = (scalar(@self_frac) >= scalar(@other_frac) ? pop(@self_frac) : 0) ;
        $of = (scalar(@other_frac) > scalar(@self_frac) ? pop(@other_frac) : 0) ;
        if (!defined $sf) {$sf = 0}
        $sf = _sub_int($sf, $carry) ;
        $cur = _sub_int($sf, $of) ;
        if ($cur < 0)
        {
            $carry = 1;
            $cur = _add_int($cur, 10);
        } else { $carry = 0 }

        unshift @result_frac, $cur ;
        if (scalar(@self_frac) || scalar(@other_frac)) { goto subf }
    
    subw:
        $sw = (scalar(@self_whole) > 0 ? pop(@self_whole) : 0) ;
        $ow = (scalar(@other_whole) > 0 ? pop(@other_whole) : 0) ;
        $sw = _sub_int($sw, $carry) ;
        $cur = _sub_int($sw, $ow) ;
        if ($cur < 0)
        {
            $carry = 1;
            $cur = _add_int($cur, 10);
        } else { $carry = 0 }
        
        unshift @result_whole, $cur ;
        if (scalar(@self_whole) || scalar(@other_whole)) { goto subw }
    
    # @result_frac = reverse @result_frac ;
    # @result_whole = reverse @result_whole ;

    $self->set_num(\@result_whole, \@result_frac) ;
    $self->{sign} = 0 ;
    return $self ;
}

sub add
{
    my ($self, $other) = @_;
    my $sign = $self->{sign} ;
    if ($self->{sign} != $other->{sign})
    {
        my $_gt = $self->absolute_is_greater_than($other) ;
        my $result = $self->absolute_difference($other) ;
        
        if ($_gt ^ $sign) 
        # if (self > other and self > 0 and other < 0) 
        # or (self < other and self < 0 and other > 0),
        # the answer will be positive
        {
            return $result ;
        }
        else 
        {
            return $result->negative();
        }
    }

    return $self->absolute_sum($other) ;
}

sub subtract
{
    my ($self, $other) = @_;
    $other = $other->negative();
    return $self->add($other) ;
}

1;