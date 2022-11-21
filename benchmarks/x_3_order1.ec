require import Byte.


module M = {
  proc main(k:byte): byte = {
    var x,x0,x1,x2,x3,x4,x5,x6,x7,x8,x9;
    var r0,r1;
   
    r0 = $distr;
    r1 = $distr;

    (* Presharing k *)
    x = k ^ r0;
    
    x0 = x * x;
    x1 = r0 * r0;
    x2 = x0 * r0;
    x3 = x1 * x;
    x4 = r1 ^ x2;
    x5 = x4 ^ x3;
    x6 = x0 * x;
    x7 = x6 ^ r1;
    x8 = x1 * r0;
    x9 = x8 ^ x5;

  }
}.
