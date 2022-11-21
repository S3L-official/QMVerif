require import Byte.

module M = {
  proc main(a b:byte): byte = {
    var a0, a1;
    var b0, b1;
    var c0, c1;
    var r0_01, r0_10, p0_01, p0_10;
    (* Presharing a *)
    a0 = $distr;
    a1 = a ^ a0;
    (* Presharing b *)
    b0 = $distr;
    b1 = b ^ b0;
    (* c = a * b *)
    r0_01 = $distr;
    p0_01 = a0 * b1;
    r0_10 = r0_01 ^ p0_01;
    p0_10 = a1 * b0;
    r0_10 = r0_10 ^ p0_10;
    c0 = a0 * b0;
    c0 = c0 ^ r0_01;
    c1 = a1 * b1;
    c1 = c1 ^ r0_10;
    c2 = pow2 c1;
    return c0 ^ c1;
  }
}.

hoare correct _a _b: M.main: a = _a /\ b = _b ==> res = _a * _b.
proof. by proc; auto; progress; algebra. qed.

masking 1 M.main (^).
