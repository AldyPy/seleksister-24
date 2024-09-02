use integer ;

use strict ;
use warnings ;

# ..not really sure if this is necessary, 
# but Perl doc says the 32 or 64 bit behaviour depends on architecture,
# so just to make sure,
my $max_bits = (1 << 32 == 0 ? 31 : 63) ;

sub _to_int 
# returns ascii of s
{
    my $s = $_[0];
    my $o = ord($s) ;
    my $d = _sub_int($o, 48);
    return _sub_int(ord($s), 48) ;
}

sub _inc
{
    my $x = $_[0] ;
    if ($x == (~0)) { return 0 }
    my $i = 1 ;
    loop0 :
        $x = $x ^ $i;
        if ($x & $i) { return $x }
        $i = $i << 1 ;
        goto loop0 ;
}

sub _add_int
# ORIGINALly BY @dragonfly02 at StackOverflow
# https://stackoverflow.com/a/26256440
{
    my ($a, $b) = @_ ;
    loop_add :
        if ($a != 0)
        {
            my $c = $b & $a;
            $b = $b ^ $a; 
            $a = $c << 1;    
            goto loop_add;
        }
    return $b;
}

sub _neg_int
{
    my $x = $_[0] ;
    return _inc(~$x);
}

sub _abs_int
{
    my $x = $_[0] ;
    return ($x < 0 ? _neg_int($x) : $x) ;
}

sub _sub_int
{
    my ($a, $b) = @_ ;
    return _add_int( $a, _neg_int( $b ) );
}


sub _mul_int
{
    my ($a, $b) = @_ ;

    my $result = 0;
    my $is_neg = ($a ^ $b) & (1 << $max_bits) ;
    $a = _abs_int($a);
    $b = _abs_int($b);

    eval 
    {
        my $i = 0 ;
        loop2 :
        {
            if ($a & (1 << $i)) 
            {
                if (($b << $i) < 0) { die ""; } # overflow
                $result = _add_int($result, ($b << $i)) ;
            }
            if ( !($a >> $i) ) { goto end_loop2 }
            $i = _add_int($i, 1) ;
            goto loop2 ;

        } end_loop2 :

    };

    
    if ($@)
    {
        # print "$@" ;
        die "Overflow occured at _mul_int" ;
    }

    return $is_neg ? _neg_int($result) : $result ;
    
}

sub _div_mod_int_by2
{
    my $a = $_[0] ;
    return ($a >> 1, $a & 1) ;
}

# use numbers in the range [0...10^18], should be fine.
# otheriwse might overflow
sub _div_mod_int
{
    my ($a, $b) = @_ ;

    if ($b == 0)
    {
        die "Division by 0\n";
    }

    my $is_neg = ($a ^ $b) & (1 << $max_bits) ; # not used
    $a = _abs_int($a);
    $b = _abs_int($b);

    if ($a < $b) { return (0, $a) }
    
    my $diff = _sub_int($a, $b) ;
    $diff = _inc($diff)  ; # calculate a - b + 1

    # For the case a < b, it is just 0, which has been handled.
    # For a >= b,the following inequality holds:
    # 1 <= a/b <= (a - b + 1), which is to say
    # 1 <= result <= (a - b + 1).
    # Using this range, we can binary search the answer.
    my $ ok = 0 ;
    my $ ng = 10 ; 
    # if this is still 10, that means this function has only been 
    # used to calculate divmod(a, 10).
    # change to $diff + 1 if u gonna use it for somethin else later.

    loop3:
        my $mid = (_add_int($ok, $ng) >> 1) ;
        my $valid ;
        
        $valid = _mul_int($b, $mid) <= $a ;
        if ($valid) { $ok = $mid }
        else { $ng = $mid }

        if ( $ng > _inc($ok) ) { goto loop3 }

    
    return ($ok, _sub_int($a, _mul_int($b, $ok)) ) ;
}

# print _mul_int(29802307, 8214733) ;
# print _inc(-1);
# print _div_mod_int(1000000000000000000, 19) ;

1;