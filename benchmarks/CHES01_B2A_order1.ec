require import Byte.
op Oxff byte
op Ox00 byte
module M = {
  proc main(x:byte): byte = {

    var r,x1,t,n1,r1;

    (* Presharing x *)
    r = $distr;
    x1 = x ^ r;

    r1 = $distr;
    t = x1 ^ r1;
    t = t - r1;
    t = t ^ x1;
    n1 = r1 ^ r;
    a = x1 ^ n1;
    a = a - n1;
    a = a ^ t;

  }
}.
