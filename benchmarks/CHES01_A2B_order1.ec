require import Byte.

op Ox02: byte.
op Ox00: byte.

module M = {
  proc main(x:byte): byte = {
    var r,a,x,x1,n1,n2;

    (* Presharing x *)
    r = $distr;
    a = x - r;

    r1 = $distr;
    t  = Ox02 * r1;
    x1 = r1 ^ r;
    n1 = r1 & x1;
    x1 = t ^ a;
    n2 = r1 ^ x1;
    n2 = n2 & r;
    n1 = n1 ^ n2;
    n2 = t & a;
    n1 = n1 ^ n2;
    x1 = x1 ^ t;

  }
}.
