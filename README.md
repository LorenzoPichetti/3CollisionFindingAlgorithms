# 3CollisionFindingAlgorithms
This repository implements the algorithms proposed in "Improved Generic Algorithms for 3-Collisions" of A.Joux e S.Lucks.
It is the final workshop for the course "IN450" (algorithms for cryptography) of the University of Roma Tre in the academic year 2019/2020.
We now give a short description of these algorithms, but we highly recommend referring to the original article (publicly available at link https://eprint.iacr.org/2009/305.pdf)
for a full explanation.

### Introduction
Let *f* be a hash function with an output set of cardinality **N**; our goal is to find a three-collision for *f* (i.e. three different *a*, *b*, and *c* such that *f(a) = f(b) = f(c)*).
It is well known that finding an *r*-collision for a random map over a finite set of cardinality **N** requires more than N^((r−1)/r) map evaluations, but, besides this lower bound on the evaluations needed, a prime goal is to find algorithms that do it in a memory-efficient way.
Here, we both implement the *folklore* and the *new proposed* algorithm (respectively in the executable file *naive_alg* and *new_alg*).
