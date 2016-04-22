// necomi/arrays/delayed.h – Delayed arrays
//
// Copyright © 2016 Émilien Tlapale
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../traits/arrays.h"
#include "dimarray.h"

namespace necomi
{

template <typename T, std::size_t N, typename Expr, bool modifiable=false>
class DelayedArray : public DimArray<std::size_t,N>
{
public:
  using dim_type = std::size_t;
  using dims_type = std::array<dim_type, N>;
  using dtype = T;
  using Self = DelayedArray<T,N,Expr,modifiable>;

  constexpr static std::size_t ndim() { return N; }

  DelayedArray(const dims_type& dims, Expr e)
    : DimArray<dim_type,N>(dims)
    , m_e(std::move(e))
  {
    static_assert(is_callable<Expr,const dims_type&>::value,
		  "function wrapped in delayed array takes invalid arguments");
    static_assert(std::is_convertible<element_type_t<Self>,T>::value,
		  "function wrapped in delayed array has invalid return type");
  }

  template <typename ...Indices,
	    typename std::enable_if_t<sizeof...(Indices)==N
                      && all_convertible<Indices..., dim_type>()>* = nullptr>
  decltype(auto) operator()(Indices... indices) const
  {
    dims_type idx{static_cast<dim_type>(indices)...};
    return this->operator()(idx);
  }
  
  template <typename ...Indices,
	    typename std::enable_if_t<sizeof...(Indices)==N
                      && all_convertible<Indices..., dim_type>()>* = nullptr>
  decltype(auto) operator()(Indices... indices)
  {
    dims_type idx{static_cast<dim_type>(indices)...};
    return this->operator()(idx);
  }
  
  decltype(auto) operator()(const dims_type& coords) const
  {
    return m_e(coords);
  }


  using const_elem_type = decltype(std::declval<const Expr>()(std::declval<const dims_type&>()));
  using elem_type = remove_const_keep_reference_t<const_elem_type>;

  template <typename Dims,
	    typename std::enable_if_t<modifiable
				      && std::is_same<Dims, dims_type>::value
				      && std::is_reference<elem_type>::value>* = nullptr>
  decltype(auto) operator()(const Dims& coords)
  {
    return const_cast<elem_type>(static_cast<const Self*>(this)->operator()(coords));
  }

  template <typename Dims,
	    typename std::enable_if_t<modifiable
				      && std::is_same<Dims, dims_type>::value
				      && ! std::is_reference<elem_type>::value>* = nullptr>
  decltype(auto) operator()(const Dims& coords)
  {
    return static_cast<elem_type>(static_cast<const Self*>(this)->operator()(coords));
  }

protected:
  Expr m_e;
};



template <typename Array, std::size_t N, typename Expr,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto make_delayed(Array&, const std::array<std::size_t,N>& dims, Expr fun)
{
  constexpr bool modifiable = is_modifiable<Array>::value;
  using T = std::remove_reference_t<decltype(fun(dims))>;
  return DelayedArray<T,N,Expr,modifiable>(dims, fun);
}

template <typename Array, typename Expr,
	  std::enable_if_t<is_indexable<Array>::value>* = nullptr>
auto make_delayed(Array& a, Expr fun)
{
  constexpr bool modifiable = is_modifiable<Array>::value;
  using T = std::remove_reference_t<decltype(fun(a.dims()))>;
  return DelayedArray<T,Array::ndim(),Expr,modifiable>(a.dims(), fun);
}

template <std::size_t N, typename Expr>
auto make_delayed(const std::array<std::size_t,N>& dims, Expr fun)
{
  using T = std::remove_reference_t<decltype(fun(dims))>;
  return DelayedArray<T,N,Expr,false>(dims, fun);
}

template <typename Array>
auto delay(Array& a)
{
  return make_delayed(a, [a](const auto& x) -> decltype(auto) {
      return a(x);
    });
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
