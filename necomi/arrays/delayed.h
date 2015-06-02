// necomi/arrays/delayed.h – Delayed arrays
//
// Copyright © 2014–2015 University of California, Irvine
// Licensed under the Simplified BSD License.

#pragma once

#include "../traits/arrays.h"
#include "dimarray.h"

namespace necomi
{

/**
 * Represent an array expression.
 */
template <typename T, std::size_t N, typename Expr, typename CExpr=Expr>
class DelayedArray : public DimArray<std::size_t,N>
{
public:
  using dim_type = std::size_t;
  using dims_type = std::array<dim_type, N>;
  using dtype = T;
  enum { ndim = N };

  DelayedArray(const dims_type& dims, Expr e, CExpr ce)
    : DimArray<dim_type,N>(dims)
    , m_e(std::move(e))
    , m_ce(ce)
  {
    static_assert(is_callable<Expr,const dims_type&>::value,
		  "function wrapped in delayed array has invalid arguments");
    static_assert(std::is_convertible<typename std::result_of<Expr(const dims_type&)>::type,T>::value,
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
    return m_ce(coords);
    //return const_cast<typename std::add_lvalue_reference<typename std::add_const<typename std::remove_reference<decltype(m_t())>::type>::type>::type>(m_t());
  }

  decltype(auto) operator()(const dims_type& coords)
  {
    return m_e(coords);
  }

protected:
  Expr m_e;
  CExpr m_ce;
};


template <std::size_t N, typename Expr>
auto make_delayed(const std::array<std::size_t,N>& dims, Expr fun)
{
  using namespace std;
  using T = typename remove_reference<typename result_of<Expr(const array<size_t,N>&)>::type>::type;
  return DelayedArray<T,N,Expr,Expr>(dims, fun, fun);
}

template <std::size_t N, typename Expr, typename CExpr>
auto make_delayed(const std::array<std::size_t,N>& dims, Expr fun, CExpr cfun)
{
  using namespace std;
  using T = typename remove_reference<typename result_of<Expr(const array<size_t,N>&)>::type>::type;
  return DelayedArray<T,N,Expr,CExpr>(dims, fun, cfun);
}


template <typename Array>
auto delay(Array& a)
{
  return make_delayed(a.dims(),
		      [a](const auto& x) mutable -> decltype(auto) {
			return a(x);
		      },
		      [a](const auto& x) -> decltype(auto) {
			return a(x);
		      });
}


template <typename Array>
auto delay(const Array& a)
{
  return make_delayed(a.dims(),
		      [a](const auto& x) -> decltype(auto) {
			return a(x);
		      });
}


} // namespace necomi

// Local Variables:
// mode: c++
// End:
