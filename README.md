## QMVerif

QMVerif is a tool focusing on the veriﬁcation of masked arithmetic programs against power side-channel attack. Please see our paper [Quantitative Veriﬁcation of Masked Arithmetic Programs against Side-Channel Attacks](https://link.springer.com/chapter/10.1007/978-3-030-17462-0_9) for more details. 

### Build
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```


### Usage
- use `-p` to describe the path of the program which needs to be verified
- use `-b` to specify the bit length of variables in the benchmark (i.e. 1 or 8)
- add `-qms` to compute QMS. Otherwise, the tool only conduct the perfect masking checking.
- add `-sim` to conduct simplification. 
- add `-bf` to use brute-force based approach. Otherwise, the SMT-based approach will be invoked.
- add `-oracle` to describe the path of the oracle file.
- add `-hd` to verify the program based on the Hamming Distance model. 

### Examples

Command of perfect masking verification of CHES10_multiplication_order1 in default setting (no simplification, using Z3 if it needs model counting) 

` QMVerif -p ../benchmarks/CHES10_multiplication_order1.ec`


Command of perfect masking verification and QMS computation of x_3_order1.ec (no simplification, using brute-force if it needs model counting) 

`QMVerif -p ../benchmarks/x_3_order1.ec -bf -qms`

### Benchmarks
We also release the benchmarks used in the [paper](https://link.springer.com/chapter/10.1007/978-3-030-17462-0_9), including 6 boolean programs (i.e. P12-P17) and 16 arithmetic programs. The script `run.sh` shows how to run them.

```
chmod +x run.sh
./run.sh
```


