require import Byte.
op Ox0: byte.
op Ox2: byte.

module M = {
  proc main(A:byte): byte = {
    var A1, A2;
    var x1, x2;
    var xx1, xx2;
    var y1, y2;
    var yy1, yy2;
    var temp1, temp2, temp3;
    var c1, c2;
    var z1, z2;
    var xc1, xc2;
    var yc1, yc2;
    var xy1, xy2;
    var r1_1, r1_2, r12_3, r12_4, r12_5;


    (* Presharing A *)
    A1 = $distr;
    A2 = A - A1;
   
    (* ConvertA2B *)
    x1 = A1;

    (* Expand *)
    r1_1 = $distr;

    xx2 = x1 ^ r1_1;

    xx1 = r1_1;

    y1 = A2;

    (* Expand *)
    r1_2 = $distr;

    yy2 = y1 ^ r1_2;

    yy1 = r1_2;


    (* SecAdd *)
    c1 =  Ox0;
    c2 =  Ox0;

    (* SecAnd *)
    r12_3 = $distr;
    temp1 = xx1 & yy2;
    temp2 = xx2 & yy1;
    temp3 = r12_3 ^ temp1;
    r21_3 = temp3 ^ temp2;
    xy1 = xx1 & yy1;
    xy1 = xy1 ^ r12_3;
    xy2 = xx2 & yy2;
    xy2 = xy2 ^ r21_3;


    (* SecAnd *)
    r12_4 = $distr;
    temp1 = xx1 & c2;
    temp2 = xx2 & c1;
    temp3 = r12_4 ^ temp1;
    r21_4 = temp3 ^ temp2;
    xc1 = xx1 & c1;
    xc1 = xc1 ^ r12_4;
    xc2 = xx2 & c2;
    xc2 = xc2 ^ r21_4;


    (* SecAnd *)
    r12_5 = $distr;
    temp1 = yy1 & c2;
    temp2 = yy2 & c1;
    temp3 = r12_5 ^ temp1;
    r21_5 = temp3 ^ temp2;
    yc1 = yy1 & c1;
    yc1 = yc1 ^ r12_5;
    yc2 = yy2 & c2;
    yc2 = yc2 ^ r21_5;

    c1 = xy1 ^ xc1 ^ yc1;
    c2 = xy2 ^ xc2 ^ yc2;

    z1 = xx1 ^ yy1 ^ c1;
    z2 = xx2 ^ yy2 ^ c2;


  }
}.
