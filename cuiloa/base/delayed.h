/*
 * Copyright © 2014 University of California, Irvine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <random>

#include "array.h"

namespace cuiloa
{

  template <typename T, ArrayIndex N> class Array;

/**
 * Represent an array expression.
 * Make sure to register each dependency with add_reference.
 */
template <typename T, ArrayIndex N, typename Expr>
class DelayedArray : public AbstractArray<DelayedArray<T,N,Expr>,T,N>
{
public:
  template <typename U, ArrayIndex M, typename Expr2> friend class DelayedArray;

  /// Short name for the parent class
  typedef AbstractArray<DelayedArray<T,N,Expr>,T,N> Parent;

  DelayedArray(const std::array<ArrayIndex,N>& dims, Expr e)
    : Parent(dims)
    , m_e(e)
  {
  }

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  std::enable_if_t<sizeof...(Indices)==N && all_indices<Indices...>(),
                   T>
  operator()(Indices... indices) const
  {
    std::array<ArrayIndex,N> idx {{static_cast<ArrayIndex>(indices)...}};
    return this->operator()(idx);
  }

  /**
   * Return the value of a single element.
   */
  T operator()(const std::array<ArrayIndex,N>& path) const
  {
    return m_e(path);
  }
protected:
  Expr m_e;
};

  template <typename T, size_t N, typename Expr>
  DelayedArray<T,N,Expr>
  make_delayed(const std::array<ArrayIndex,N>& dimensions, Expr fun)
  {
    return DelayedArray<T,N,Expr>(dimensions, fun);
  }

  /// Converts any array into a delayed one.
  template <typename Concrete, typename T, ArrayIndex N>
  auto delay(const AbstractArray<Concrete,T,N>& a)
  {
    auto fun = [b=a.shallow_copy()](auto& path) {return b(path);};
    return DelayedArray<T,N,decltype(fun)>(a.dimensions(), fun);
  }


/**
 * Namespace to work with DelayedArrays.
 */
namespace delayed
{
  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator==(const AbstractArray<Concrete1,T,N>& a,
		  const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot compare arrays of different dimensions");
#endif
    return make_delayed<bool,N>(a.dimensions(),
				[a=a.shallow_copy(),b=b.shallow_copy()]
				(auto& path) {
	return a(path) == b(path);
      });
  }

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator!=(const AbstractArray<Concrete1,T,N>& a,
		  const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot compare arrays of different dimensions");
#endif
    return make_delayed<bool,N>(a.dimensions(),
				[a=a.shallow_copy(),b=b.shallow_copy()]
				(auto& path) { return a(path) != b(path); });
  }

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator*(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return a(path) * b(path); });
  }

  /*
  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2, typename U,
	    typename std::enable_if_t<is_promotable<T,U>::value>* = nullptr>
  auto operator*(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,U,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    return make_delayed<U,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return static_cast<U>(a(path)) * b(path); });
  }

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2, typename U,
	    typename std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
  auto operator*(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,U,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return a(path) * static_cast<T>(b(path)); });
			     }*/

  template <typename Concrete, typename T, ArrayIndex N>
  auto operator*(T value, const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=value]
			     (auto& path) { return value*a(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_arithmetic<U>::value && is_promotable<U,T>::value>* = nullptr>
  auto operator*(U value, const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<T>(value)]
			     (auto& path) { return value*a(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_arithmetic<U>::value && is_promotable<T,U>::value>* = nullptr>
  auto operator*(U value, const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<U,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=value]
			     (auto& path)
			     { return value*static_cast<U>(a(path)); }
			    );
  }

  /*template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  auto operator*(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<T>(value)]
			     (auto& path) { return a(path)*value; });
			     }
  */

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator/(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return a(path) / b(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  auto operator/(U value, const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<U>(value)]
			     (auto& path) { return value/a(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  auto operator/(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<T>(value)]
			     (auto& path) { return a(path)/value; });
  }

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator-(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot multiply arrays of different dimensions");
#endif
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return a(path) - b(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  auto operator-(U value, const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<U>(value)]
			     (auto& path) { return value-a(path); });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_convertible<U,T>::value>* = nullptr>
  auto operator-(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<T>(value)]
			     (auto& path) { return a(path)-value; });
  }

  template <typename Concrete1, typename T, ArrayIndex N,
	    typename Concrete2>
  auto operator+(const AbstractArray<Concrete1,T,N>& a,
		 const AbstractArray<Concrete2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot sum arrays of different dimensions");
#endif
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),b=b.shallow_copy()]
			     (auto& path) { return a(path) + b(path); });
  }

  template <typename T, ArrayIndex N>
  auto operator>(const Array<T,N>& a, const T& val)
  //auto operator>(const AbstractArray<Concrete,T,N>& a, const T& val)
  {
    auto fun = [a,val](auto& path) {
        return a(path) > val;
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dimensions(), fun);
  }

  template <typename T, ArrayIndex N>
  auto operator<(const Array<T,N>& a, const T& val)
  {
    auto fun = [a,val](auto& path) {
        return a(path) < val;
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dimensions(), fun);
  }

  template <typename T, ArrayIndex N>
  auto operator>(const Array<T,N>& a, const Array<T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot sum arrays of different dimensions");
#endif
    auto fun = [a,b](auto& path) {
        return a(path) > b(path);
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dimensions(), fun);
  }

  template <typename T, ArrayIndex N>
  auto operator<(const Array<T,N>& a, const Array<T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot sum arrays of different dimensions");
#endif
    auto fun = [a,b](auto& path) {
        return a(path) < b(path);
      };
    return DelayedArray<bool,N,decltype(fun)>(a.dimensions(), fun);
  }

  /**
   * Create an array with the same dimensions filled with a constant
   * value.
   * \see zeros_like
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto constants_like(const AbstractArray<Concrete,T,N>& a, const T& value)
  {
    return make_delayed<T>(a.dimensions(), [value](auto&){ return value; });
  }

  /**
   * Create an array with the same dimensions filled with zero values.
   * \see constants_like
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto zeros_like(const AbstractArray<Concrete,T,N>& a)
  {
    return constants_like<Concrete,T,N>(a, 0);
  }

  template <typename T>
  auto range(T stop)
  {
    return make_delayed<T,1>({{static_cast<ArrayIndex>(stop)}},
			     [](auto& path){ return path[0]; });
  }

  template <typename T>
  auto range(T start, T stop)
  {
    return make_delayed<T,1>({{static_cast<ArrayIndex>(stop-start)}},
			     [start](auto& path){ return start+path[0]; });
  }

  template <typename T>
  auto range(T start, T stop, T step)
  {
    auto size = static_cast<ArrayIndex>(std::ceil(static_cast<double>(stop-start)/step));
    return make_delayed<T,1>({size},
			     [start,step](auto& path)
			     { return start+step*path[0]; });
  }

  template <ArrayIndex M, typename Concrete, typename T, ArrayIndex N>
  auto reshape(const AbstractArray<Concrete,T,N>& a,
	       const std::array<ArrayIndex,M>& dims)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the input and output array sizes are the same
    auto out_size = std::accumulate(dims.cbegin(), dims.cend(),
				    static_cast<ArrayIndex>(1),
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
    if (out_size != a.size())
      throw std::length_error("invalid size for reshaped array");
#endif
    auto old_strides = default_strides(a.dimensions());
    auto new_strides = default_strides(dims);
    return make_delayed<T,M>(dims,
			     [a=a.shallow_copy(),old_strides,new_strides]
			     (auto& path)
	       { auto idx = std::inner_product(path.cbegin(), path.cend(),
					       new_strides.cbegin(), 0);
		 auto old_path = index_to_path(idx, old_strides);
		 return a(old_path); });
  }

  template <typename Concrete, typename T, ArrayIndex N>
  auto exp(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()]
      (auto& path) { return std::exp(a(path)); });
  }

  /**
   * Sum an array across a given dimension.
   */
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if<N!=0>::type* = nullptr>
  auto sum(const AbstractArray<Concrete,T,N>&a, ArrayIndex dim)
  {
    // Compute the dimensions of the new array
    std::array<ArrayIndex,N-1> dims;
    auto oit = std::copy_n(a.dimensions().cbegin(), dim, dims.begin());
    if (dim != N-1)
      std::copy(a.dimensions().cbegin()+dim+1, a.dimensions().cend(), oit);

    return make_delayed<T,N-1>(dims,
      [a=a.shallow_copy(),dim] (auto& path) {
        // Path in the original array
        Coordinates<N> orig_path;
	auto oit = std::copy_n(path.cbegin(), dim, orig_path.begin());
	if (dim != N-1)
	  std::copy(path.cbegin()+dim, path.cend(), oit+1);
	// Sum all the elements in the dimension
	T val = 0;
	for (ArrayIndex i = 0; i < a.dimensions()[dim]; i++) {
          orig_path[dim] = i;
	  val += a(orig_path);
        }
	return val;
      });
  }

  /**
   * Average an array across a given dimension.
   */
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if<N!=0>::type* = nullptr>
  auto average(const AbstractArray<Concrete,T,N>&a, ArrayIndex dim)
  {
    return sum(a,dim) / static_cast<T>(a.dimensions()[dim]);
  }

  /**
   * Element-wise absolute value.
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto abs(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()]
      (auto& path) { return std::abs(a(path)); });
  }

  enum class Norm {
    Infinity
  };

  /**
   * Average an array across a given dimension.
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto norm(const AbstractArray<Concrete,T,N>&a, Norm norm)
  {
    switch (norm) {
    case Norm::Infinity:
      return max(abs(a));
    }
  }

  /**
   * Shift elements on a given axis.
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto roll(const AbstractArray<Concrete,T,N>&a,
	    ArrayIndex shift, ArrayIndex dim)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    if (dim >= a.dimensions().size())
      throw std::out_of_range("invalid rolling dimension");
#endif
    auto sz = a.dimensions()[dim];
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),sz,dim,shift]
			     (auto path) {
			       path[dim] = (path[dim] + sz - shift) % sz;
			       return a(path);
			     });
  }

  template <typename Concrete, typename T>
  auto roll(const AbstractArray<Concrete,T,1>&a, ArrayIndex shift)
  {
    return roll<Concrete,T,1>(a, shift, 0);
  }

  /**
   * Create an identity matrix.
   */
  template <typename T=double>
  auto identity(ArrayDimension dim)
  {
    return make_delayed<T,2>({dim,dim}, [](auto path) {
	return path[0] == path[1];
      });
  }

  template <typename To, typename ...From>
  struct all_convertible;

  template <typename To>
  struct all_convertible<To> : std::true_type
  {};

  template <typename To, typename From, typename ...Froms>
  struct all_convertible<To, From, Froms...>
    : std::integral_constant<bool,
			     std::is_convertible<From, To>::value &&
			     all_convertible<To, Froms...>::value>
  {};

  template <typename T=double,
	    typename ...Values,
            typename std::enable_if_t<all_convertible<T,Values...>::value>* = nullptr>
  auto litarray(Values... values)
  {
    //std::vector<Values> vals = indices...;
    std::vector<T> vals = {static_cast<T>(values)...};
    return make_delayed<T,1>({sizeof...(Values)},
			     [vals=std::move(vals)](auto& path) {
			       return vals[path[0]];
			     });
  }
} // namespace delayed
} // namespace cuiloa

// Local Variables:
// mode: c++
// End:
