require import Byte.
op Ox00: byte.
op Ox02: byte.
op OxFE: byte.

module M = {
  proc main(x:byte): byte = {
    var x1, x2;
    var D1, D2;
    var a1, a2, a3, a4, a5;
    var temp1, temp2, temp3, temp4;
    var r_1, s_1;
    var u;

    (* Presharing x *)
    x1 = $distr;
    x2 = x ^ x1;
   

   (* GoubinSNI *)
    s_1 = $distr;
    a1 = x1 ^ s_1;
    a2 = x2 ^ s_1;
    r_1 = $distr;
    temp2 = a2 ^ r_1;

    (* fun *)
    temp1 = a1 ^ temp2;
    temp3 = temp1 - temp2;

    u = a1 ^ temp3;

    (* fun *)
    temp1 = a1 ^ r_1;
    temp4 = temp1 - r_1;

    D1 = u ^ temp4;
    D2 = a2;


    return D1 + D2
  }
}.
