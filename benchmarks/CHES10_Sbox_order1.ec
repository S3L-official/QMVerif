require import Byte.

op Ox63:byte.

op affineF: byte -> byte.
axiom affineF_affine x y: affineF (x ^ y) ^ Ox63 = affineF x ^ affineF y.

module M = {
  proc main(x:byte): byte = {
    var x0, x1;
    var y0, y1;
    var z0, z1;
    var w0, w1;
    var temp1, temp2, temp3;
    var tmp_1,r01_2,tmp_3,r01_4,r01_5,r01_6,r01_7;

    (* Presharing x *)
    x0 = $distr;
    x1 = x ^ x0;
   
	(* SecSBox *)

	(* SecExp254 *)
	z0 = pow2 x0;
	z1 = pow2 x1;

	(* RefreshMasks *)
	tmp_1 = $distr;
	z0 = z0 ^ tmp_1;
	z1 = z1 ^ tmp_1;


	(* SecMult *)
	r01_2 = $distr;
	temp1 = z0 * x1;
	temp2 = z1 * x0;
	temp3 = r01_2 ^ temp1;
	r10_2 = temp3 ^ temp2;
	y0 = z0 * x0;
	y0 = y0 ^ r01_2;
	y1 = z1 * x1;
	y1 = y1 ^ r10_2;

	w0 = pow4 y0;
	w1 = pow4 y1;

	(* RefreshMasks *)
	tmp_3 = $distr;
	w0 = w0 ^ tmp_3;
	w1 = w1 ^ tmp_3;


	(* SecMult *)
	r01_4 = $distr;
	temp1 = y0 * w1;
	temp2 = y1 * w0;
	temp3 = r01_4 ^ temp1;
	r10_4 = temp3 ^ temp2;
	y0 = y0 * w0;
	y0 = y0 ^ r01_4;
	y1 = y1 * w1;
	y1 = y1 ^ r10_4;

	y0 = pow16 y0;
	y1 = pow16 y1;

	(* SecMult *)
	r01_5 = $distr;
	temp1 = y0 * w1;
	temp2 = y1 * w0;
	temp3 = r01_5 ^ temp1;
	r10_5 = temp3 ^ temp2;
	y0 = y0 * w0;
	y0 = y0 ^ r01_5;
	y1 = y1 * w1;
	y1 = y1 ^ r10_5;


	(* SecMult *)
	r01_6 = $distr;
	temp1 = y0 * z1;
	temp2 = y1 * z0;
	temp3 = r01_6 ^ temp1;
	r10_6 = temp3 ^ temp2;
	y0 = y0 * z0;
	y0 = y0 ^ r01_6;
	y1 = y1 * z1;
	y1 = y1 ^ r10_6;


	y0 = affineF y0;
	y1 = affineF y1;
	y0 = Ox63 y0;



  }
}.

