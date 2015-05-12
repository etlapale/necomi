// necomi/arrays/delayed.h – Delayed arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "dimarray.h"

namespace necomi
{
/**
 * Represent an array expression.
 * Make sure to register each dependency with add_reference.
 */
template <typename T, std::size_t N, typename Expr>
class DelayedArray : public DimArray<std::size_t,N>
{
public:
  using dim_type = std::size_t;
  using dims_type = std::array<dim_type, N>;
  using dtype = T;
  enum { ndim = N };
  
  template <typename U, dim_type M, typename Expr2> friend class DelayedArray;
  
  DelayedArray(const dims_type& dims, Expr e)
    : DimArray<dim_type,N>(dims)
    , m_e(std::move(e))
  {
    static_assert(is_callable<Expr,const dims_type&>::value,
		  "function wrapped in delayed array has invalid arguments");
    static_assert(std::is_convertible<typename std::result_of<Expr(const dims_type&)>::type,T>::value,
		  "function wrapped in delayed array has invalid return type");
  }
  
  /**
   * Indicates whether the wrapped function returns references.
   */
  static constexpr bool is_modifiable()
  {
    return std::is_convertible<
      typename std::result_of<Expr(const dims_type&)>::type,
      T&>::value;
  }
  
  typedef typename std::conditional<is_modifiable(), T&, T>::type ReturnType;

  /**
   * Return the value of a single element.
   */
  template <typename ...Indices>
  std::enable_if_t<sizeof...(Indices)==N && all_convertible<Indices..., dim_type>(), T>
    operator()(Indices... indices) const
  {
    dims_type idx{static_cast<dim_type>(indices)...};
    return this->operator()(idx);
  }

  template <typename ...Indices>
  std::enable_if_t<sizeof...(Indices)==N && all_convertible<Indices..., dim_type>(), ReturnType>
    operator()(Indices... indices)
  {
    dims_type idx{static_cast<dim_type>(indices)...};
    return this->operator()(idx);
  }

  /// Return the value of a single element.
  T operator()(const dims_type& coords) const
  {
    return m_e(coords);
  }

  ReturnType operator()(const dims_type& coords)
  {
    return m_e(coords);
  }

  template <typename ConstMapOperation>
  void map(ConstMapOperation f) const
  {
    dims_type coords;
    const_for_looper<DelayedArray<T,N,Expr>,0,ConstMapOperation>(*this, coords, f);
  }

protected:
  Expr m_e;
};


template <typename T=double, std::size_t N=1, typename Expr>
DelayedArray<T,N,Expr>
make_delayed(const std::array<std::size_t,N>& dims, Expr fun)
{
  // TODO: pass dimensions by value since we copy them in the constructor
  return DelayedArray<T,N,Expr>(dims, std::move(fun));
}

template <typename T, typename Expr,
	  typename std::enable_if_t<is_callable<Expr,const std::array<std::size_t,1>&>::value>* = nullptr>
auto make_delayed(std::size_t size, Expr fun)
{
  return DelayedArray<T,1,Expr>({size}, std::move(fun));
}

/// Converts any array into a delayed one.
template <typename Array>
auto delay(const Array& a)
{
  return make_delayed<typename Array::dtype, Array::ndim>(a.dims(),
							  [a](const auto& x)
							  { return a(x); });
}
  
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
    (a.dims(), std::forward<Expr>(e));
}

} // namespace necomi

// Local Variables:
// mode: c++
// End:
