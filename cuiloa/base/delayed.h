/* Copyright © 2014–2015 University of California, Irvine
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
#include "concepts.h"

namespace cuiloa
{

  template <typename T, ArrayIndex N> class Array;


/**
 * Represent an array expression.
 * Make sure to register each dependency with add_reference.
 */
template <typename T, ArrayDimension N, typename Expr>
class DelayedArray : public AbstractArray<DelayedArray<T,N,Expr>,T,N>
{
public:
  using dtype = T;
  enum { ndim = N };

  template <typename U, ArrayIndex M, typename Expr2> friend class DelayedArray;

  /// Short name for the parent class
  typedef AbstractArray<DelayedArray<T,N,Expr>,T,N> Parent;

  DelayedArray(const std::array<ArrayIndex,N>& dims, Expr e)
    : Parent(dims)
    , m_e(std::move(e))
  {
    static_assert(is_callable<Expr,const Coordinates<N>&>::value,
		  "function wrapped in delayed array has invalid arguments");
    static_assert(std::is_convertible<typename std::result_of<Expr(const Coordinates<N>&)>::type,T>::value,
		  "function wrapped in delayed array has invalid return type");
  }
  
  /**
   * Indicates whether the wrapped function returns references.
   */
  static constexpr bool is_modifiable()
  {
    return std::is_convertible<
      typename std::result_of<Expr(const Coordinates<N>&)>::type,
                              T&>::value;
  }
  
  typedef typename std::conditional<is_modifiable(), T&, T>::type ReturnType;

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  std::enable_if_t<sizeof...(Indices)==N && all_indices<Indices...>(), T>
  operator()(Indices... indices) const
  {
    std::array<ArrayIndex,N> idx{static_cast<ArrayIndex>(indices)...};
    return this->operator()(idx);
  }

  template <typename ...Indices>
  std::enable_if_t<sizeof...(Indices)==N && all_indices<Indices...>(), ReturnType>
  operator()(Indices... indices)
  {
    std::array<ArrayIndex,N> idx{static_cast<ArrayIndex>(indices)...};
    return this->operator()(idx);
  }

  /**
   * Return the value of a single element.
   */
  T operator()(const std::array<ArrayIndex,N>& path) const
  {
    return m_e(path);
  }

  ReturnType operator()(const std::array<ArrayIndex,N>& path)
  {
    return m_e(path);
  }

protected:
  Expr m_e;
};

  template <typename T=double, ArrayDimension N=1, typename Expr>
  DelayedArray<T,N,Expr>
  make_delayed(const Dimensions<N>& dimensions, Expr fun)
  {
    // TODO: pass dimensions by value since we copy them in the constructor
    return DelayedArray<T,N,Expr>(dimensions, std::move(fun));
  }

  /*
  template <typename T=double, typename Expr,
            typename std::enable_if_t<is_callable<Expr,ArrayIndex>::value>* = nullptr>
  auto make_delayed(ArrayDimension size, Expr fun)
  {
    return make_delayed<T,1>({size}, [fun](auto& coords) { return fun(coords[0]); });
  }*/

  template <typename T=double, typename Expr,
            typename std::enable_if_t<is_callable<Expr,const Coordinates<1>&>::value>* = nullptr>
  auto make_delayed(ArrayDimension size, Expr fun)
  {
    return DelayedArray<T,1,Expr>({size}, std::move(fun));
  }

  /// Converts any array into a delayed one.
  template <typename Concrete, typename T, ArrayIndex N>
  auto delay(const AbstractArray<Concrete,T,N>& a)
  {
    auto fun = [b=a.shallow_copy()](auto& path) {return b(path);};
    return DelayedArray<T,N,decltype(fun)>(a.dimensions(), std::move(fun));
  }

  /**
   * \defgroup Delayed Delayed arrays.
   * Define delayed arrays from functions and their utilities.
   * @{
   */
  
  /**
   * Create a delayed array from an indexable one.
   * The created array will have the same element type and array dimensions
   * as the one in first argument, and will have its element values
   * defined by the second argument.
   */
  template <typename Array, typename Expr,
	    typename std::enable_if_t<is_array<Array>::value>* = nullptr>
  auto make_delayed(const Array& a, Expr&& e)
  {
    return DelayedArray<typename Array::dtype, Array::ndim, Expr>
      (a.dimensions(), std::forward<Expr>(e));
  }

  namespace delayed
  {
    /**
     * Create a delayed array from the absolute values of another.
     * \param a	An \ref IndexableArray "indexable array".
     */
    template <typename IndexableArray,
	      typename std::enable_if_t<is_indexable<IndexableArray>::value>* = nullptr>
    auto abs(const IndexableArray& a)
    {
      return make_delayed(a.dimensions(),
			  [a](const Coordinates<IndexableArray::ndim>& path)
			  { return std::abs(a(path)); });
    }

    /**
     * Norms available to function \ref norm.
     */
    enum class Norm {
      /** Maximum of the absolute values. */
      Infinity
    };

    /**
     * Average an array across a given dimension.
     * \param a	An \ref IndexableArray "indexable array".
     */
    template <typename IndexableArray>
    auto norm(const IndexableArray& a, Norm norm)
    {
      switch (norm) {
      case Norm::Infinity:
	return max(abs(a));
      }
    }
  }

  /**@}*/

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

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_arithmetic<U>::value && is_promotable<U,T>::value>* = nullptr>
  auto operator*(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=static_cast<T>(value)]
			     (auto& path) { return a(path)*value; });
  }

  template <typename Concrete, typename T, ArrayIndex N, typename U,
	    typename std::enable_if_t<std::is_arithmetic<U>::value && is_promotable<T,U>::value>* = nullptr>
  auto operator*(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<U,N>(a.dimensions(),
			     [a=a.shallow_copy(), value=value]
			     (auto& path)
			     { return static_cast<U>(a(path))*value; }
			    );
  }

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
   * Create an array filled with a constant value.
   */
  template <typename T=double, ArrayIndex N=1>
  auto constants(const Dimensions<N>& dims, T value)
  {
    return make_delayed<T>(dims, [value](auto&){ return value; });
  }
  
template <ArrayIndex N=1, typename T=double>
auto zeros(const Dimensions<N>& dims)
{
  return constants<T,N>(dims,0);
}

template <typename ...Dims,
	  typename T=double,
	  typename std::enable_if<all_indices<Dims...>::value>* = nullptr>
auto zeros(Dims... dims)
{
  return constants<T,sizeof...(Dims)>({static_cast<std::size_t>(dims)...}, 0);
}


  /**
   * Create an array with the same dimensions filled with a constant
   * value.
   * \see zeros_like
   */
  template <typename T, typename U, ArrayIndex N, typename Concrete>
  auto constants_like(const AbstractArray<Concrete,U,N>& a, const T&& value)
  {
    return make_delayed<T>(a.dimensions(), [value](auto&){ return value; });
  }

  /**
   * Create an array with the same dimensions filled with a constant
   * value.
   * \see zeros_like
   */
  template <typename T, ArrayIndex N, typename Concrete>
  auto constants_like(const AbstractArray<Concrete,T,N>& a, const T&& value)
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
    return constants_like<T,N,Concrete>(a, 0);
  }

  /**
   * Create an array with the same dimensions filled with zero values.
   * \see constants_like
   */
  template <typename T, typename U, ArrayIndex N, typename Concrete>
  auto zeros_like(const AbstractArray<Concrete,U,N>& a)
  {
    return constants_like<T,U,N,Concrete>(a, 0);
  }

  template <typename T>
  auto range(T stop)
  {
    return make_delayed<T,1>({{static_cast<ArrayIndex>(stop)}},
			     [](auto& path){ return path[0]; });
  }

  template <typename T>
  auto range(T start, T stop, T step=1)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    if (stop <= start)
      throw std::out_of_range("stop must be greater than start for ranges");
    if (step <= 0)
      throw std::out_of_range("step must be positive for ranges");
#endif
    auto size = static_cast<ArrayIndex>(std::ceil(static_cast<double>(stop-start)/step));
    return make_delayed<T,1>({size},
			     [start,step](auto& coords)
			     { return start+step*coords[0]; });
  }
  
  /**
   * Return an array of evenly spaced floating point numbers.
   * If the type of the boundaries is floating, the resulting array elements
   * will be of that type. Otherwise, they will be double values.
   */
  template <typename T,
	    typename U=typename std::conditional<std::is_floating_point<T>::value,
						 T,double>::type,
	    std::enable_if_t<std::is_convertible<T,U>::value>* = nullptr>
  auto linspace(T start, T stop, ArrayDimension size, bool endpoint=true)
  {
    auto step = static_cast<U>(stop - start)/(endpoint ? size - 1 : size);
    return make_delayed<U,1>({size},
			     [start,step](auto& coords)
			     { return start+step*coords[0]; });
  }


template <std::size_t M, typename Array>
auto reshape(const Array& a, const Dimensions<M>& d)
{
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the input and output array sizes are the same
    auto out_size = std::accumulate(d.cbegin(), d.cend(),
				    static_cast<ArrayDimension>(1),
        [] (ArrayIndex a, ArrayIndex b) { return a * b; });
    if (out_size != a.size())
      throw std::length_error("invalid dimensions for reshaped array");
#endif
    auto old_strides = default_strides(a.dimensions());
    auto new_strides = default_strides(d);
    return make_delayed<typename Array::dtype,M>(d,
			     [a=a.shallow_copy(),old_strides,new_strides]
			     (auto& path)
	       { auto idx = std::inner_product(path.cbegin(), path.cend(),
					       new_strides.cbegin(), 0);
		 auto old_path = index_to_path(idx, old_strides);
		 return a(old_path); });
}

template <typename Array, typename ...Dimensions,
	  typename std::enable_if_t<all_indices<Dimensions...>::value>* = nullptr>
auto reshape(const Array& a, Dimensions... dims)
{
  cuiloa::Dimensions<sizeof...(Dimensions)> d =
    {static_cast<std::size_t>(dims)...};
  return reshape(a, d);
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
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
      [a=a.shallow_copy(),dim] (auto& path) {
        // Path in the original array
	auto orig_path = add_coordinate(path, dim);
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
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto average(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim)
  {
    return sum(a,dim) / static_cast<T>(a.dimensions()[dim]);
  }

/**
 * Average across all dimensions.
 */
template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto average(const AbstractArray<Concrete,T,N>& a)
{
  return sum(a) / static_cast<T>(a.size());
}
  
  template <unsigned N, typename T>
  std::enable_if_t<N==0,T>
  power(T)
  {
    return 1;
  }

  template <unsigned N, typename T>
  std::enable_if_t<N==1,T>
  power(T val)
  {
    return val;
  }
  
  template <unsigned N, typename T>
  std::enable_if_t<1<N,T>
  power(T val)
  {
    return val * power<N-1>(val);
  }

  /**
   * Square root.
   */
  template <typename Concrete, typename T, ArrayIndex N>
  auto sqrt(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()]
      (auto& path) { return std::sqrt(a(path)); });
  }

  /**
   * Compute a sample variance with a two-pass formula.
   *
   * When the function is called, the average along the given dimension
   * is computed and stored in an immediate array.
   *
   * When the Bessel correction is enabled, an N-1 divider is used
   * (default in Matlab), otherwise a N divider is used (default
   * in NumPy).
   */
  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto variance(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim,
		bool bessel_correction)
  {
    auto avg = immediate(average(a, dim));
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
      [a=a.shallow_copy(),dim,avg=avg.shallow_copy(),bessel_correction]
      (auto& path)
      {
        // Path in the original array
	auto orig_path = add_coordinate(path, dim);
	// Sum the squared deviations to the mean
	T val = 0;
	for (ArrayIndex i = 0; i < a.dimensions()[dim]; i++) {
          orig_path[dim] = i;
	  val += power<2>(a(orig_path) - avg(path));
        }
	return val / (bessel_correction ? a.dim(dim) - 1 : a.dim(dim));
      });
  }

template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto variance(const AbstractArray<Concrete,T,N>& a,
	      bool bessel_correction)
{
  static_assert(std::is_floating_point<T>::value,
		"statistics on arrays require floating elements");
  auto avg = average(a);
  T res = 0;
  a.map([avg,&res](auto&, auto val) {
      res += power<2>(val - avg);
    });
  return res / (bessel_correction ? a.size() - 1 : a.size());
}

  template <typename Concrete, typename T, ArrayIndex N,
	    typename std::enable_if_t<N!=0>* = nullptr>
  auto deviation(const AbstractArray<Concrete,T,N>& a, ArrayIndex dim,
		 bool bessel_correction)
  {
    return sqrt(variance(a, dim, bessel_correction));
  }

template <typename Concrete, typename T, ArrayIndex N,
	  typename std::enable_if_t<N!=0>* = nullptr>
auto deviation(const AbstractArray<Concrete,T,N>& a,
	       bool bessel_correction)
{
  return std::sqrt(variance(a, bessel_correction));
}


  template <typename Concrete, typename T, ArrayDimension N, typename U,
	    typename std::enable_if_t<is_promotable<U,T>::value>* = nullptr>
  auto max(const AbstractArray<Concrete,T,N>& a, U value)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy(),value=static_cast<T>(value)]
			     (auto& coords) { return std::max(a(coords), value); });
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
  
  /**
   * Non-normalized generalized Gaussian function with integral beta.
   */
  template <unsigned beta, typename T, typename Concrete,
	    typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  auto ggd(const AbstractArray<Concrete,T,1>& a, T alpha, T mu=0)
  {
    return make_delayed<T,1>(a.dimensions(),
			     [a=a.shallow_copy(),alpha,mu]
			     (auto& coords) {
			       return std::exp(-power<beta>(std::fabs(a(coords)-mu)/alpha));
			     });

  }

  template <typename T, ArrayDimension N, typename Concrete>
  auto norm_angle_diff(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()](auto& coords) {
			       T x = a(coords);
			       if (x >= -180 && x <= 180)
				 return x;
			       if (x > 180)
				 return 360 - x;
			       else // x < -180
				 return -x-360;
			     });
  }
  
  template <typename T, ArrayDimension N, typename C1, typename C2>
  auto zip(const AbstractArray<C1,T,N>& a, const AbstractArray<C2,T,N>& b)
  {
#ifndef CUILOA_NO_BOUND_CHECKS
    // Make sure the dimensions of a and b are the same
    if (a.dimensions() != b.dimensions())
      throw std::length_error("cannot zip arrays of different dimensions");
#endif
    return make_delayed<T,N+1>(append_coordinate(a.dimensions(), 2),
	[a=a.shallow_copy(), b=b.shallow_copy()]
	(auto& coords) {
          auto c = remove_coordinate(coords, N);
	  return coords[N] == 0 ? a(c) : b(c);
	});
  }

  template <typename T, ArrayDimension N, typename Concrete>
  auto round(const AbstractArray<Concrete,T,N>& a)
  {
    return make_delayed<T,N>(a.dimensions(),
			     [a=a.shallow_copy()](auto& coords) {
			       return std::round(a(coords));
			     });
  }
  
  template <typename T, ArrayDimension N, typename Concrete>
  auto shifted(const AbstractArray<Concrete,T,N>& a,
	       std::array<T,N> offset,
	       T default_value = 0)
  {
    return make_delayed<T,N>(a.dimensions(),
	[a=a.shallow_copy(),offset=std::move(offset),
         default_value=std::move(default_value)]
	(auto& coords) {
	  Coordinates<N> cx;
	  for (ArrayIndex i = 0; i < a.ndim; i++) {
	    // Check for negative resulting coordinates
	    if (offset[i] < 0
		&& static_cast<ArrayDimension>(-offset[i]) > coords[i])
	      return default_value;
	    cx[i] = coords[i] + offset[i];
	    // Check for out of range resulting coordinates
	    if (cx[i] >= a.dim(i))
	      return default_value;
	  }
	  return a(cx);
	});
  }

/**
 * Utility class to get an element amongst heterogeneous arrays.  Used
 * to get the element at coordinates c of the i-th array, you can use
 * \c choose_array<0,Array,Arrays...>::at(i, c, a, as), where the
 * arrays are denoted by a, as..., with respective types Array,
 * Arrays.... The 0 is the mandatory index to start the search.  A
 * simpler interface is provided by stack() which creates a delayed
 * array from a list of arrays.
 */
template <std::size_t, typename...>
struct choose_array;

template <std::size_t I, typename Array, typename... Arrays>
struct choose_array<I, Array, Arrays...>
{
  static double at(std::size_t n, const Coordinates<Array::ndim>& coords,
		   const Array& a, const Arrays&... as)
  {
    if (n == I)
      return a(coords);
    else
      return choose_array<I+1, Arrays...>::at(n, coords, as...);
  }
};

template <std::size_t I>
struct choose_array<I>
{
  template <ArrayDimension N>
  static double at(std::size_t, const Coordinates<N>&)
  { throw std::range_error("invalid array chosen"); }
};


/**
 * Stack several indexable arrays into a single delayed one.  The
 * first dimension of the returned array will select one of the
 * orginal arrays, and the rest of the coordinates will index into them.
 */
template <typename Array, typename ...Arrays>
auto stack(Array a, Arrays... as)
{
  return make_delayed<double,Array::ndim+1>(prepend_coordinate(a.dimensions(), sizeof...(Arrays)+1), [a,as...] (const auto& coords) {
      auto c = remove_coordinate(coords, 0);
      return choose_array<0,Array,Arrays...>::at(coords[0], c, a, as...);
    });							   
}

} // namespace delayed
  
  //////////////////////////////////////////////////////////////////////////
  
  template <typename T, ArrayDimension N, typename Concrete>
  auto fix_dimension(const AbstractArray<Concrete,T,N>& a,
		     ArrayIndex dim, ArrayIndex val)
  {
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
			       [a=a.shallow_copy(),dim,val]
			       (auto& coords) {
				 return a(add_coordinate(coords, dim, val));
			       });
  }
  
  /*template <typename T, ArrayDimension N, typename Concrete,
	    typename std::enable_if_t<AbstractArray<Concrete,T,N>::is_modifiable()>* = nullptr>
  auto fix_dimension(AbstractArray<Concrete,T,N>& a,
		     ArrayIndex dim, ArrayIndex val)
  {
    return make_delayed<T,N-1>(remove_coordinate(a.dimensions(), dim),
			       [a=a.shallow_copy(),dim,val]
			       (auto& coords) -> T& {
				 //return a(add_coordinate(coords, dim, val));
				 auto c = add_coordinate(coords, dim, val);
				 T& t = a(c);
				 return t;
			       });
			       }*/



} // namespace cuiloa
// Local Variables:
// mode: c++
// End:
