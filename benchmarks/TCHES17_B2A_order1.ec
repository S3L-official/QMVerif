require import Byte.
op Ox0: byte.
op OxFE: byte.

module M = {
  proc main(x:byte): byte = {
    var x1, x2;
    var D1, D2;
    var y1, y2, y3;
    var z1, z2;
    var A1, A2
    var B1, B2;
    var temp1, temp2, temp3, temp4, temp5, temp6;
    var r_1, r_2;
  
    (* Presharing x *)
    x1 = $distr;
    x2 = x ^ x1;
    
   
    (* RefreshMasks *)
    y1 = x1;
    r_1 = $distr;
    y2 = x2 ^ r_1;
    y1 = y1 ^ r_1;
    r_2 = $distr;
    y3 = Ox0 ^ r_2;
    y1 = y1 ^ r_2;


    (* fun *)
    temp1 = y1 ^ y2;
    temp5 = temp1 - y2;

    temp6 = OxFE @ y1;
    z1 = temp6 ^ temp5;

    (* fun *)
    temp1 = y1 ^ y3;
    z2 = temp1 - y3;

    A1 = y2 ^ Ox0;
    B1 = z1 ^ Ox0;
    D1 = A1;
    D2 = B1;



  }
}.
