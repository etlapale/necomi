Random number generation
========================

.. highlight:: c++

Seeding
-------
	       
Efficiently generating good quality pseudo random numbers on a
computer is hard. One has to choose a decent random number generator,
with expected statistical properties depending on the task, which
should be fast if a great number of data is expected, and the
generator should be `correctly seeded`_.

.. _correctly seeded: http://www.pcg-random.org/posts/cpp-seeding-surprises.html

The C++ standard defines_ a few random number engines. Although all of
them are too weak to serve in cryptography applications, the
Mersenne-Twister engine is a common choice for numerical
simulation. Correctly seeding it requires a few hundred bytes from a
secure source. On some systems we can use `std::random_device`_, and
its wrapper `RandomDevSeedSequence`, to seed it::

  necomi::RandomDevSeedSequence rdss;
  std::mt19937_64 prng(rdss);

.. _defines: http://en.cppreference.com/w/cpp/numeric/random
.. _`std::random_device`: http://en.cppreference.com/w/cpp/numeric/random/random_device

Distributions
-------------

Necomi defines several functions wrapping the C++ distribution
functions.

.. cpp:function:: Array<T,N> normal(Dims dims, PRNG& prng)

   Generate an array of floating point values following a normal_
   (Gaussian) distribution.

   By default an array of `double` values is generated, but you can
   set `T` to another element type. The dimensionality `N` is infered,
   if possible, from the `dims` argument: if a single scalar is given,
   a 1D array is returned; passing a `const Dimensions<N>&` allows
   specifying an arbitrary shape. The random number generator `prng`
   is used to generate the elements.

   ::
      
     auto a = normal<float>(42, rng);  // 1D array of 42 floats
     Dimensions<3> d = {5,3,7};
     auto c = normal(d, rng);          // 3D array of doubles

.. cpp:function:: Array<T,N> normal(T mean, T deviation, Dims dims, PRNG& prng)

   Idem but with a specific mean and deviation.

.. _normal: https://en.wikipedia.org/wiki/Normal_distribution
