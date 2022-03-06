# 3CollisionFindingAlgorithms
This repository implements the algorithms proposed in "Improved Generic Algorithms for 3-Collisions" of A.Joux e S.Lucks.
It is the final workshop for the course "IN450" (algorithms for cryptography) of the University of Roma Tre in the academic year 2019/2020.
We now give a short description of these algorithms, but we highly recommend referring to the original article (publicly available at link https://eprint.iacr.org/2009/305.pdf)
for a full explanation.

### Introduction
Let *f* be a hash function with an output set of cardinality **N**; our goal is to find a three-collision for *f* (i.e. three different *a*, *b*, and *c* such that *f(a) = f(b) = f(c)*).
It is well known that finding an *r*-collision for a random map over a finite set of cardinality **N** requires more than N<sup>(r−1)/r</sup> map evaluations, but, besides this lower bound on the evaluations needed, a prime goal is to find algorithms that do it in a memory-efficient way.
Here, we both implement the *folklore* and the *new proposed* algorithm (respectively in the executable file *naive_alg* and *new_alg*).

### The Folklore algorithm
The folklore algorithm is divided into two steps:
1. In the first step, we generate a number *Na* of random string *x<sub>1</sub>*, ... *x<sub>Na</sub>*, compute the hash values *f(x<sub>1</sub>)*, ... *f(x<sub>Na</sub>)*, and store them in the memory like in the following table.

<table>
<thead>
  <tr>
    <th></th>
    <th colspan="5"></th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>Image</td>
    <td>x<sub>1</sub></td>
    <td>x<sub>2</sub></td>
    <td>x<sub>3</sub></td>
    <td>...................</td>
    <td>x<sub>Na</sub></td>
  </tr>
  <tr>
    <td>Preimag1<br></td>
    <td>f(x<sub>1</sub>)</td>
    <td>f(x<sub>2</sub>)</td>
    <td>f(x<sub>3</sub>)</td>
    <td>...................</td>
    <td>f(x<sub>Na</sub>)</td>
  </tr>
  <tr>
    <td>Preimag2<br></td>
    <td>NULL<br></td>
    <td>NULL</td>
    <td>NULL</td>
    <td>...................</td>
    <td>NULL</td>
  </tr>
</tbody>
</table>

2. In the second step, we generate Nb random hash evaluations without storing them. At every cycle we have a random string *y* and its hash *f(y)*; if for some i in {1, ..., Na} the hash *f(y)* is equal to *f(x<sub>i</sub>)*, *y != x<sub>i</sub>*, and *Preimage2[i] == NULL*, we set *Preimage2[i] = y*. When we found two different *y<sub>1</sub>* and *y<sub>2</sub>* (both different from some *x<sub>i</sub>*) such that *f(x<sub>i</sub>) = f(y<sub>1</sub>) = f(y<sub>2</sub>)*, the algorithm stops and return the 3-collision *(x<sub>i</sub>, y<sub>1</sub>, y<sub>2</sub>)*.
