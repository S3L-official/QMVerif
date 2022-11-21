require import Byte.


module M = {
  proc main(x:byte): byte = {
    var x0,x1;
    var z0,z1;
    var w0,w1;
    var temp1,temp2,temp3;
    var r01_1,r01_2,r01_3;
    var r10_1,r10_2,r10_3;

    (* Presharing x *)
    x0 = $distr;
    x1 = x ^ x0;

    z0 = pow2 x0;
    z1 = pow2 x1;

    (* SecMult *)
    r01_1 = $distr;
    temp1 = z0 * x1;
    temp2 = z1 * x0;
    temp3 = r01_1 ^ temp1;
    r10_1 = temp3 ^ temp2;
    y0 = z0 * x0;
    y0 = y0 ^ r01_1;
    y1 = z1 * x1;
    y1 = y1 ^ r10_1;

    w0 = pow4 y0;
    w1 = pow4 y1;

    (* SecMult *)
    r01_2 = $distr;
    temp1 = y0 * w1;
    temp2 = y1 * w0;
    temp3 = r01_2 ^ temp1;
    r10_2 = temp3 ^ temp2;
    y0 = y0 * w0;
    y0 = y0 ^ r01_2;
    y1 = y1 * w1;
    y1 = y1 ^ r10_2;

    y0 = pow16 y0;
    y1 = pow16 y1;

    (* SecMult *)
    r01_3 = $distr;
    temp1 = y0 * w1;
    temp2 = y1 * w0;
    temp3 = r01_3 ^ temp1;
    r10_3 = temp3 ^ temp2;
    y0 = y0 * w0;
    y0 = y0 ^ r01_3;
    y1 = y1 * w1;
    y1 = y1 ^ r10_3;

  }
}.
