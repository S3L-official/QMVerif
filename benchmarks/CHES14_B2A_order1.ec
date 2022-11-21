require import Byte.
op Ox00: byte.
op Ox02: byte.


module M = {
  proc main(x:byte): byte = {
    var x1, x2;
    var r1_2, r1_3, r12_4, r12_5, r12_6, r12_7, r12_8, r12_9, r2_10, r2_11;
    var temp1, temp2, temp3;
    var A1, A2;
    var xx1, xx2;
    var yy1, yy2;
    var c1, c2;
    var zzz1, zzz2;
    var xc1, xc2;
    var yc1, yc2;
    var xy1, xy2;
    var yyy1, yyy2;
   
    (* Presharing x *)
    x1 = $distr;
    x2 = x ^ x1;
  
   (* ConvertB2A *)
    A1 = $distr;
    A1 = Ox00 - A1;
    A2 = Ox00;

    (* ConvertA2B *)
    x1 = A1;

    (* Expand *)
    r1_2 = $distr;

    xx2 = x1 ^ r1_2;

    xx1 = r1_2;

    y1 = A2;

    (* Expand *)
    r1_3 = $distr;

    yy2 = y1 ^ r1_3;

    yy1 = r1_3;


    (* SecAdd *)
    c1 =  Ox00;
    c2 =  Ox00;

    (* SecAnd *)
    r12_4 = $distr;
    temp1 = xx1 & yy2;
    temp2 = xx2 & yy1;
    temp3 = r12_4 ^ temp1;
    r21_4 = temp3 ^ temp2;
    xy1 = xx1 & yy1;
    xy1 = xy1 ^ r12_4;
    xy2 = xx2 & yy2;
    xy2 = xy2 ^ r21_4;


    (* SecAnd *)
    r12_5 = $distr;
    temp1 = xx1 & c2;
    temp2 = xx2 & c1;
    temp3 = r12_5 ^ temp1;
    r21_5 = temp3 ^ temp2;
    xc1 = xx1 & c1;
    xc1 = xc1 ^ r12_5;
    xc2 = xx2 & c2;
    xc2 = xc2 ^ r21_5;


    (* SecAnd *)
    r12_6 = $distr;
    temp1 = yy1 & c2;
    temp2 = yy2 & c1;
    temp3 = r12_6 ^ temp1;
    r21_6 = temp3 ^ temp2;
    yc1 = yy1 & c1;
    yc1 = yc1 ^ r12_6;
    yc2 = yy2 & c2;
    yc2 = yc2 ^ r21_6;

    c1 = xy1 ^ xc1 ^ yc1;
    c2 = xy2 ^ xc2 ^ yc2;

    yyy1 = xx1 ^ yy1 ^ c1;
    yyy2 = xx2 ^ yy2 ^ c2;



    (* SecAdd *)
    c1 =  Ox00;
    c2 =  Ox00;

    (* SecAnd *)
    r12_7 = $distr;
    temp1 = x1 & yyy2;
    temp2 = x2 & yyy1;
    temp3 = r12_7 ^ temp1;
    r21_7 = temp3 ^ temp2;
    xy1 = x1 & yyy1;
    xy1 = xy1 ^ r12_7;
    xy2 = x2 & yyy2;
    xy2 = xy2 ^ r21_7;


    (* SecAnd *)
    r12_8 = $distr;
    temp1 = x1 & c2;
    temp2 = x2 & c1;
    temp3 = r12_8 ^ temp1;
    r21_8 = temp3 ^ temp2;
    xc1 = x1 & c1;
    xc1 = xc1 ^ r12_8;
    xc2 = x2 & c2;
    xc2 = xc2 ^ r21_8;


    (* SecAnd *)
    r12_9 = $distr;
    temp1 = yyy1 & c2;
    temp2 = yyy2 & c1;
    temp3 = r12_9 ^ temp1;
    r21_9 = temp3 ^ temp2;
    yc1 = yyy1 & c1;
    yc1 = yc1 ^ r12_9;
    yc2 = yyy2 & c2;
    yc2 = yc2 ^ r21_9;

    c1 = xy1 ^ xc1 ^ yc1;
    c2 = xy2 ^ xc2 ^ yc2;

    zzz1 = x1 ^ yyy1 ^ c1;
    zzz2 = x2 ^ yyy2 ^ c2;


    (* FullXor *)

    (* RefreshMasks *)
    r2_10 = $distr;
    zzz1 = zzz1 ^ r2_10;
    zzz2 = zzz2 ^ r2_10;


    (* RefreshMasks *)
    r2_11 = $distr;
    zzz1 = zzz1 ^ r2_11;
    zzz2 = zzz2 ^ r2_11;


    A2 = zzz1 ^ zzz2;


  }
}.
