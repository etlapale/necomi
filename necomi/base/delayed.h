// necomi/base/delayed.h – Delayed arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include <random>

#include "array.h"
#include "concepts.h"

namespace necomi
{
template <typename T, ArrayIndex N> class Array;

/**
 * Represent an array expression.
 * Make sure to register each dependency with add_reference.
 */
template <typename T, ArrayDimension N, typename Expr>
class DelayedArray : public DimArray<N>
{
public:
  using dtype = T;
  enum { ndim = N };
  
  template <typename U, ArrayIndex M, typename Expr2> friend class DelayedArray;
  
  DelayedArray(const std::array<ArrayIndex,N>& dims, Expr e)
    : DimArray<N>(dims)
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

  template <typename ConstMapOperation>
  void map(ConstMapOperation f) const
  {
    Coordinates<N> path;
    const_for_looper<ConstMapOperation,0,Array<T,N>>(*this, path, f);
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

  template <typename T, typename Expr,
            typename std::enable_if_t<is_callable<Expr,const Coordinates<1>&>::value>* = nullptr>
  auto make_delayed(ArrayDimension size, Expr fun)
  {
    return DelayedArray<T,1,Expr>({size}, std::move(fun));
  }

/// Converts any array into a delayed one.
template <typename Array>
auto delay(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
							  [a](const auto& x)
							  { return a(x); });
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

  /**@}*/

/**
 * Namespace to work with DelayedArrays.
 */
namespace delayed
{

/**
 * Compare two arrays element-wise.
 * This operator is disabled if the shapes do not match, to allow for
 * shape broadcasting by other operators.
 */
template <typename Array1, typename Array2,
	  typename std::enable_if_t<is_array<Array1>::value &&
	    Array1::ndim==Array2::ndim>* = nullptr>
auto operator==(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dimensions(),
					 [a,b] (const auto& coords) {
					   return a(coords) == b(coords);
					 });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator!=(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot compare arrays of different dimensions");
#endif
  return make_delayed<bool,Array1::ndim>(a.dimensions(),
					 [a,b] (const auto& coords) {
					   return a(coords) != b(coords);
					 });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator*(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot multiply arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dimensions(),
				      [a,b] (const auto& x) { return a(x) * b(x); });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return a(x)*value; });
}

template <typename U, typename Array,
	  std::enable_if_t<is_indexable<Array>::value
			   && ! is_indexable<U>::value>* = nullptr>
auto operator*(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return value*a(x); });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator/(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot divide arrays of different dimensions");
#endif
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<C,Array1::ndim>(a.dimensions(),
				      [a,b] (const auto& x) { return a(x) / b(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto operator/(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return value/a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto operator/(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return a(x)/value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator-(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot sum arrays of different dimensions");
#endif
  return make_delayed<C, Array1::ndim>(a.dimensions(),
				       [a,b](const auto& coords) {
					 return a(coords) - b(coords);
				       });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto operator-(U value, const Array& a)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return value - a(x); });
}

template <typename Array, typename U,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto operator-(const Array& a, U value)
{
  using C = typename std::common_type<typename Array::dtype, U>::type;
  return make_delayed<C,Array::ndim>(a.dimensions(),
				     [a,value] (const auto& x)
				     { return a(x) - value; });
}

template <typename Array1, typename Array2,
	  typename std::enable_if_t<Array1::ndim==Array2::ndim>* = nullptr>
auto operator+(const Array1& a, const Array2& b)
{
  using C = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot sum arrays of different dimensions");
#endif
  return make_delayed<C, Array1::ndim>(a.dimensions(),
				       [a,b](const auto& coords) {
					 return a(coords) + b(coords);
				       });
}

  template <typename T, ArrayIndex N>
  auto operator>(const Array<T,N>& a, const T& val)
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
#ifndef NECOMI_NO_BOUND_CHECKS
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
#ifndef NECOMI_NO_BOUND_CHECKS
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
  
template <typename T=double, ArrayIndex N>
auto zeros(const Dimensions<N>& dims)
{
  return constants<T,N>(dims,0);
}

template <typename T=double,
	  typename ...Dims,
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
template <typename T, typename Array>
auto constants_like(const Array& a, const T&& value)
{
  return make_delayed<T>(a.dimensions(),
			 [value](const auto&){ return value; });
}

/**
 * Create an array with the same dimensions filled with a constant
 * value.
 * \see zeros_like
 */
/*template <typename T, ArrayIndex N, typename Concrete>
auto constants_like(const AbstractArray<Concrete,T,N>& a, const T&& value)
{
  return make_delayed<T>(a.dimensions(), [value](auto&){ return value; });
  }*/

/**
 * Create an array with the same dimensions filled with zero values.
 * \see constants_like
 */
template <typename Array>
auto zeros_like(const Array& a)
{
  return constants_like<typename Array::dtype>(a, 0);
}

/**
 * Create an array with the same dimensions filled with zero values.
 * \see constants_like
 */
template <typename T, typename Array>
auto zeros_like(const Array& a)
{
  return constants_like<T,Array>(a, 0);
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
#ifndef NECOMI_NO_BOUND_CHECKS
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
#ifndef NECOMI_NO_BOUND_CHECKS
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
			     [a,old_strides,new_strides]
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
  necomi::Dimensions<sizeof...(Dimensions)> d =
    {static_cast<std::size_t>(dims)...};
  return reshape(a, d);
}

/**
 * Shift elements on a given axis.
 */
template <typename Array>
auto roll(const Array& a, ArrayIndex shift, ArrayIndex dim)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  if (dim >= Array::ndim)
    throw std::out_of_range("invalid rolling dimension");
#endif
  auto sz = a.dimensions()[dim];
  return make_delayed<typename Array::dtype, Array::ndim>(a.dimensions(),
			   [a,sz,dim,shift]
			   (auto path) {
			     path[dim] = (path[dim] + sz - shift) % sz;
			     return a(path);
			   });
}

template <typename Array,
	  std::enable_if_t<Array::ndim==1>* = nullptr>
auto roll(const Array& a, ArrayIndex shift)
{
  return roll<Array>(a, shift, 0);
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

// TODO: remove (stack() special case)
template <typename Array1, typename Array2,
	  std::enable_if_t<Array1::ndim == Array2::ndim>* = nullptr>
auto zip(const Array1& a, const Array2& b)
{
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (a.dimensions() != b.dimensions())
    throw std::length_error("cannot zip arrays of different dimensions");
#endif
  using T = typename std::common_type<typename Array1::dtype,
				      typename Array2::dtype>::type;
  return make_delayed<T,Array1::ndim+1>(append_coordinate(a.dimensions(), 2),
			     [a,b]
			     (auto& coords) {
					  auto c = remove_coordinate(coords, Array1::ndim);
			       return coords[Array1::ndim] == 0 ? a(c) : b(c);
			     });
}
  
template <typename Array, typename T=typename Array::dtype>
auto shifted(const Array& a,
	     std::array<T,Array::ndim> offset,
	     T default_value = 0)
{
  return make_delayed<T,Array::ndim>(a.dimensions(),
	[a=a,offset=std::move(offset),
         default_value=std::move(default_value)]
	(const auto& coords) {
				       Coordinates<Array::ndim> cx;
	  for (ArrayIndex i = 0; i < Array::ndim; i++) {
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


// TODO: remove, special case of fix_dimension

template <typename Array>
auto slice(Array a, std::size_t i)
{
  static_assert(Array::ndim >= 1,
		"only arrays with more than one dimension are sliceable");
#ifndef NECOMI_NO_BOUND_CHECKS
  // Make sure the dimensions of a and b are the same
  if (i >= a.dim(0))
    throw std::range_error("slice index is too large");
#endif
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dimensions(), 0), [a,i] (const auto& coords) {
      auto c = prepend_coordinate(coords, i);
      return a(c);
    });
}

} // namespace delayed
  
  //////////////////////////////////////////////////////////////////////////
  
template <typename Array>
auto fix_dimension(const Array& a, ArrayIndex dim, ArrayIndex val)
{
  return make_delayed<typename Array::dtype,Array::ndim-1>(remove_coordinate(a.dimensions(), dim),
			     [a,dim,val]
			     (const auto& coords) {
			       return a(add_coordinate(coords, dim, val));
			     });
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
