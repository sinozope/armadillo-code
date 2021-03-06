// Copyright 2008-2016 Conrad Sanderson (http://conradsanderson.id.au)
// Copyright 2008-2016 National ICT Australia (NICTA)
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ------------------------------------------------------------------------


//! \addtogroup CoMat
//! @{



template<typename eT>
inline
CoMat<eT>::~CoMat()
  {
  arma_extra_debug_sigprint_this(this);
  
  reset();
  
  if(map_ptr)  { delete map_ptr; }
  
  // try to expose buggy user code that accesses deleted objects
  if(arma_config::debug)  { map_ptr = NULL; }
  
  arma_type_check(( is_supported_elem_type<eT>::value == false ));
  }



template<typename eT>
inline
CoMat<eT>::CoMat()
  : n_rows (0)
  , n_cols (0)
  , n_elem (0)
  , map_ptr(NULL)
  {
  arma_extra_debug_sigprint_this(this);
  
  init_cold();
  }



template<typename eT>
inline
CoMat<eT>::CoMat(const uword in_n_rows, const uword in_n_cols)
  : n_rows (in_n_rows)
  , n_cols (in_n_cols)
  , n_elem (in_n_rows * in_n_cols)
  , map_ptr(NULL)
  {
  arma_extra_debug_sigprint_this(this);
  
  init_cold();
  }



template<typename eT>
inline
CoMat<eT>::CoMat(const SizeMat& s)
  : n_rows (s.n_rows)
  , n_cols (s.n_cols)
  , n_elem (s.n_rows * s.n_cols)
  , map_ptr(NULL)
  {
  arma_extra_debug_sigprint_this(this);
  
  init_cold();
  }



template<typename eT>
inline
CoMat<eT>::CoMat(const CoMat<eT>& x)
  : n_rows (0)
  , n_cols (0)
  , n_elem (0)
  , map_ptr(NULL)
  {
  arma_extra_debug_sigprint_this(this);
  
  init_cold();
  
  (*this).operator=(x);
  }



template<typename eT>
inline
void
CoMat<eT>::operator=(const CoMat<eT>& x)
  {
  arma_extra_debug_sigprint();
  
  if(this == &x)  { return; }
  
  access::rw(n_rows) = x.n_rows;
  access::rw(n_cols) = x.n_cols;
  access::rw(n_elem) = x.n_elem;
  
  (*map_ptr) = *(x.map_ptr);
  }



template<typename eT>
inline
CoMat<eT>::CoMat(const SpMat<eT>& x)
  : n_rows (0)
  , n_cols (0)
  , n_elem (0)
  , map_ptr(NULL)
  {
  arma_extra_debug_sigprint_this(this);
  
  init_cold();
  
  (*this).operator=(x);
  }



template<typename eT>
inline
void
CoMat<eT>::operator=(const SpMat<eT>& x)
  {
  arma_extra_debug_sigprint();
  
  (*this).zeros(x.n_rows, x.n_cols);
  
  typename SpMat<eT>::const_iterator it     = x.begin();
  typename SpMat<eT>::const_iterator it_end = x.end();
  
  map_type& map_ref = (*map_ptr);
  
  const uword local_n_rows = n_rows;
  
  for(; it != it_end; ++it)
    {
    const eT val = (*it);
    
    if(val != eT(0))
      {
      const uword row = it.row();
      const uword col = it.col();
      
      const uword index = (local_n_rows * col) + row;
      
      #if defined(ARMA_USE_CXX11)
        map_ref.emplace_hint(map_ref.cend(), index, val);
      #else
        map_ref.operator[](index) = val;
      #endif
      }
    }
  }



#if defined(ARMA_USE_CXX11)

  template<typename eT>
  inline
  CoMat<eT>::CoMat(CoMat<eT>&& x)
    : n_rows (x.n_rows )
    , n_cols (x.n_cols )
    , n_elem (x.n_elem )
    , map_ptr(x.map_ptr)
    {
    arma_extra_debug_sigprint_this(this);
    
    access::rw(x.n_rows)  = 0;
    access::rw(x.n_cols)  = 0;
    access::rw(x.n_elem)  = 0;
    access::rw(x.map_ptr) = NULL;
    }
  
  
  
  template<typename eT>
  inline
  void
  CoMat<eT>::operator=(CoMat<eT>&& x)
    {
    arma_extra_debug_sigprint();
    
    reset();
    
    if(map_ptr)  { delete map_ptr; }
    
    access::rw(n_rows)  = x.n_rows;
    access::rw(n_cols)  = x.n_cols;
    access::rw(n_elem)  = x.n_elem;
    access::rw(map_ptr) = x.map_ptr;
    
    access::rw(x.n_rows)  = 0;
    access::rw(x.n_cols)  = 0;
    access::rw(x.n_elem)  = 0;
    access::rw(x.map_ptr) = NULL;
    }

#endif



template<typename eT>
inline
void
CoMat<eT>::reset()
  {
  arma_extra_debug_sigprint();
  
  init_warm(0, 0);
  }



template<typename eT>
inline
void
CoMat<eT>::set_size(const uword in_n_rows)
  {
  arma_extra_debug_sigprint();
  
  init_warm(in_n_rows, 1);
  }



template<typename eT>
inline
void
CoMat<eT>::set_size(const uword in_n_rows, const uword in_n_cols)
  {
  arma_extra_debug_sigprint();
  
  init_warm(in_n_rows, in_n_cols);
  }



template<typename eT>
inline
void
CoMat<eT>::set_size(const SizeMat& s)
  {
  arma_extra_debug_sigprint();
  
  init_warm(s.n_rows, s.n_cols);
  }



template<typename eT>
inline
void
CoMat<eT>::zeros()
  {
  arma_extra_debug_sigprint();
  
  (*map_ptr).clear();
  }



template<typename eT>
inline
void
CoMat<eT>::zeros(const uword in_n_rows)
  {
  arma_extra_debug_sigprint();
  
  init_warm(in_n_rows, 1);
  
  (*map_ptr).clear();
  }



template<typename eT>
inline
void
CoMat<eT>::zeros(const uword in_n_rows, const uword in_n_cols)
  {
  arma_extra_debug_sigprint();
  
  init_warm(in_n_rows, in_n_cols);
  
  (*map_ptr).clear();
  }



template<typename eT>
inline
void
CoMat<eT>::zeros(const SizeMat& s)
  {
  arma_extra_debug_sigprint();
  
  init_warm(s.n_rows, s.n_cols);
  
  (*map_ptr).clear();
  }



template<typename eT>
inline
void
CoMat<eT>::eye()
  {
  arma_extra_debug_sigprint();
  
  (*this).eye(n_rows, n_cols);
  }



template<typename eT>
inline
void
CoMat<eT>::eye(const uword in_n_rows, const uword in_n_cols)
  {
  arma_extra_debug_sigprint();
  
  zeros(in_n_rows, in_n_cols);
  
  const uword N = (std::min)(in_n_rows, in_n_cols);
  
  map_type& map_ref = (*map_ptr);
  
  for(uword i=0; i<N; ++i)
    {
    const uword index = (in_n_rows * i) + i;
    
    #if defined(ARMA_USE_CXX11)
      map_ref.emplace_hint(map_ref.cend(), index, eT(1));
    #else
      map_ref.operator[](index) = eT(1);
    #endif
    }
  }



template<typename eT>
inline
void
CoMat<eT>::eye(const SizeMat& s)
  {
  arma_extra_debug_sigprint();
  
  (*this).eye(s.n_rows, s.n_cols);
  }



template<typename eT>
inline
void
CoMat<eT>::speye()
  {
  arma_extra_debug_sigprint();
  
  (*this).eye();
  }



template<typename eT>
inline
void
CoMat<eT>::speye(const uword in_n_rows, const uword in_n_cols)
  {
  arma_extra_debug_sigprint();
  
  (*this).eye(in_n_rows, in_n_cols);
  }



template<typename eT>
inline
void
CoMat<eT>::speye(const SizeMat& s)
  {
  arma_extra_debug_sigprint();
  
  (*this).eye(s);
  }



template<typename eT>
arma_inline
CoMat_elem<eT>
CoMat<eT>::elem(const uword index, uword& sync_state, uword& n_nonzero)
  {
  return CoMat_elem<eT>(*this, index, sync_state, n_nonzero);
  }



template<typename eT>
arma_inline
CoMat_elem<eT>
CoMat<eT>::elem(const uword in_row, const uword in_col, uword& sync_state, uword& n_nonzero)
  {
  const uword index = (n_rows * in_col) + in_row;
  
  return CoMat_elem<eT>(*this, index, sync_state, n_nonzero);
  }



template<typename eT>
arma_inline
arma_warn_unused
CoMat_val<eT>
CoMat<eT>::operator[](const uword index)
  {
  return CoMat_val<eT>(*this, index);
  }



template<typename eT>
arma_inline
arma_warn_unused
eT
CoMat<eT>::operator[](const uword index) const
  {
  map_type& map_ref = (*map_ptr);
  
  typename map_type::const_iterator it     = map_ref.find(index);
  typename map_type::const_iterator it_end = map_ref.end();
  
  return (it != it_end) ? eT((*it).second) : eT(0);
  }



template<typename eT>
arma_inline
arma_warn_unused
CoMat_val<eT>
CoMat<eT>::operator()(const uword index)
  {
  arma_debug_check( (index >= n_elem), "CoMat::operator(): index out of bounds" );
  
  return CoMat_val<eT>(*this, index);
  }



template<typename eT>
arma_inline
arma_warn_unused
eT
CoMat<eT>::operator()(const uword index) const
  {
  arma_debug_check( (index >= n_elem), "CoMat::operator(): index out of bounds" );
  
  map_type& map_ref = (*map_ptr);
  
  typename map_type::const_iterator it     = map_ref.find(index);
  typename map_type::const_iterator it_end = map_ref.end();
  
  return (it != it_end) ? eT((*it).second) : eT(0);
  }



template<typename eT>
arma_inline
arma_warn_unused
CoMat_val<eT>
CoMat<eT>::at(const uword in_row, const uword in_col)
  {
  const uword index = (n_rows * in_col) + in_row;
  
  return CoMat_val<eT>(*this, index);
  }



template<typename eT>
arma_inline
arma_warn_unused
eT
CoMat<eT>::at(const uword in_row, const uword in_col) const
  {
  const uword index = (n_rows * in_col) + in_row;
  
  map_type& map_ref = (*map_ptr);
  
  typename map_type::const_iterator it     = map_ref.find(index);
  typename map_type::const_iterator it_end = map_ref.end();
  
  return (it != it_end) ? eT((*it).second) : eT(0);
  }



template<typename eT>
arma_inline
arma_warn_unused
CoMat_val<eT>
CoMat<eT>::operator()(const uword in_row, const uword in_col)
  {
  arma_debug_check( ((in_row >= n_rows) || (in_col >= n_cols)), "CoMat::operator(): index out of bounds" );
  
  const uword index = (n_rows * in_col) + in_row;
  
  return CoMat_val<eT>(*this, index);
  }



template<typename eT>
arma_inline
arma_warn_unused
eT
CoMat<eT>::operator()(const uword in_row, const uword in_col) const
  {
  arma_debug_check( ((in_row >= n_rows) || (in_col >= n_cols)), "CoMat::operator(): index out of bounds" );
  
  const uword index = (n_rows * in_col) + in_row;
  
  map_type& map_ref = (*map_ptr);
  
  typename map_type::const_iterator it     = map_ref.find(index);
  typename map_type::const_iterator it_end = map_ref.end();
  
  return (it != it_end) ? eT((*it).second) : eT(0);
  }



template<typename eT>
inline
arma_warn_unused
bool
CoMat<eT>::is_empty() const
  {
  return (n_elem == 0);
  }



template<typename eT>
inline
arma_warn_unused
bool
CoMat<eT>::is_vec() const
  {
  return ( (n_rows == 1) || (n_cols == 1) );
  }



template<typename eT>
inline
arma_warn_unused
bool
CoMat<eT>::is_rowvec() const
  {
  return (n_rows == 1);
  }



//! returns true if the object can be interpreted as a column vector
template<typename eT>
inline
arma_warn_unused
bool
CoMat<eT>::is_colvec() const
  {
  return (n_cols == 1);
  }



template<typename eT>
inline
arma_warn_unused
bool
CoMat<eT>::is_square() const
  {
  return (n_rows == n_cols);
  }



// this function is for debugging purposes only
template<typename eT>
inline
void
CoMat<eT>::sprandu(const uword in_n_rows, const uword in_n_cols, const double density)
  {
  arma_extra_debug_sigprint();
  
  zeros(in_n_rows, in_n_cols);
  
  const uword N = uword(density * double(n_elem));
  
  const Col<eT>    vals(N, fill::randu);
  const Col<uword> indx = linspace< Col<uword> >(0, ((n_elem > 0) ? uword(n_elem-1) : uword(0)) , N);
  
  const eT*    vals_mem = vals.memptr();
  const uword* indx_mem = indx.memptr();
  
  map_type& map_ref = (*map_ptr);
  
  for(uword i=0; i < N; ++i)
    {
    const uword index = indx_mem[i];
    const eT    val   = vals_mem[i];
    
    #if defined(ARMA_USE_CXX11)
      map_ref.emplace_hint(map_ref.cend(), index, val);
    #else
      map_ref.operator[](index) = val;
    #endif
    }
  }



// this function is for debugging purposes only
template<typename eT>
inline
void
CoMat<eT>::print(const std::string& extra_text) const
  {
  arma_extra_debug_sigprint();
  
  if(extra_text.length() != 0)
    {
    const std::streamsize orig_width = ARMA_COUT_STREAM.width();
    
    ARMA_COUT_STREAM << extra_text << '\n';
    
    ARMA_COUT_STREAM.width(orig_width);
    }
  
  map_type& map_ref = (*map_ptr);
  
  const uword n_nonzero = uword(map_ref.size());
  
  const double density = (n_elem > 0) ? ((double(n_nonzero) / double(n_elem))*double(100)) : double(0);
  
  ARMA_COUT_STREAM
    << "[matrix size: " << n_rows << 'x' << n_cols << "; n_nonzero: " << n_nonzero
    << "; density: " << density << "%]\n\n";
  
  if(n_nonzero > 0)
    {
    typename map_type::const_iterator it = map_ref.begin();

    for(uword i=0; i < n_nonzero; ++i)
      {
      const std::pair<uword, eT>& entry = (*it);
      
      const uword index = entry.first;
      const eT    val   = entry.second;
      
      const uword row = index % n_rows;
      const uword col = index / n_rows;
      
      ARMA_COUT_STREAM << '(' << row << ", " << col << ") ";
      ARMA_COUT_STREAM << val << '\n';
      
      ++it;
      }
    }
  
  ARMA_COUT_STREAM.flush();
  }



template<typename eT>
inline
uword
CoMat<eT>::get_n_nonzero() const
  {
  arma_extra_debug_sigprint();
  
  return uword((*map_ptr).size());
  }



template<typename eT>
inline
void
CoMat<eT>::get_locval_format(umat& locs, Col<eT>& vals) const
  {
  arma_extra_debug_sigprint();
  
  map_type& map_ref = (*map_ptr);
  
  typename map_type::const_iterator it = map_ref.begin();
  
  const uword N = uword(map_ref.size());
  
  locs.set_size(2,N);
  vals.set_size(N);
  
  eT* vals_mem = vals.memptr();
  
  for(uword i=0; i<N; ++i)
    {
    const std::pair<uword, eT>& entry = (*it);
    
    const uword index = entry.first;
    const eT    val   = entry.second;
    
    const uword row = index % n_rows;
    const uword col = index / n_rows;
    
    uword* locs_colptr = locs.colptr(i);
    
    locs_colptr[0] = row;
    locs_colptr[1] = col;
    
    vals_mem[i] = val;
    
    ++it;
    }
  }



template<typename eT>
inline
void
CoMat<eT>::init_cold()
  {
  arma_extra_debug_sigprint();
  
  // ensure that n_elem can hold the result of (n_rows * n_cols)
  
  #if (defined(ARMA_USE_CXX11) || defined(ARMA_64BIT_WORD))
    const char* error_message = "CoMat(): requested size is too large";
  #else
    const char* error_message = "CoMat(): requested size is too large; suggest to compile in C++11 mode or enable ARMA_64BIT_WORD";
  #endif
  
  arma_debug_check
    (
      (
      ( (n_rows > ARMA_MAX_UHWORD) || (n_cols > ARMA_MAX_UHWORD) )
        ? ( (double(n_rows) * double(n_cols)) > double(ARMA_MAX_UWORD) )
        : false
      ),
    error_message
    );
  
  map_ptr = new (std::nothrow) map_type;
  
  arma_check_bad_alloc( (map_ptr == NULL), "CoMat(): out of memory" );
  }



template<typename eT>
inline
void
CoMat<eT>::init_warm(const uword in_n_rows, const uword in_n_cols)
  {
  arma_extra_debug_sigprint();
  
  if( (n_rows == in_n_rows) && (n_cols == in_n_cols))  { return; }
  
  // ensure that n_elem can hold the result of (n_rows * n_cols)
  
  #if (defined(ARMA_USE_CXX11) || defined(ARMA_64BIT_WORD))
    const char* error_message = "CoMat(): requested size is too large";
  #else
    const char* error_message = "CoMat(): requested size is too large; suggest to compile in C++11 mode or enable ARMA_64BIT_WORD";
  #endif
  
  arma_debug_check
    (
      (
      ( (in_n_rows > ARMA_MAX_UHWORD) || (in_n_cols > ARMA_MAX_UHWORD) )
        ? ( (double(in_n_rows) * double(in_n_cols)) > double(ARMA_MAX_UWORD) )
        : false
      ),
    error_message
    );
  
  const uword new_n_elem = in_n_rows * in_n_cols;
  
  access::rw(n_rows) = in_n_rows;
  access::rw(n_cols) = in_n_cols;
  access::rw(n_elem) = new_n_elem;
  
  if( (new_n_elem == 0) && ((*map_ptr).empty() == false) )  { (*map_ptr).clear(); }
  }



template<typename eT>
arma_inline
void
CoMat<eT>::set_val(const uword index, const eT& in_val)
  {
  arma_extra_debug_sigprint();
  
  if(in_val != eT(0))
    {
    #if defined(ARMA_USE_CXX11)
      {
      map_type& map_ref = (*map_ptr);
      
      if( (map_ref.empty() == false) && (index > uword(map_ref.crbegin()->first)) )
        {
        map_ref.emplace_hint(map_ref.cend(), index, in_val);
        }
      else
        {
        map_ref.operator[](index) = in_val;
        }
      }
    #else
      {
      (*map_ptr).operator[](index) = in_val;
      }
    #endif
    }
  else
    {
    (*this).erase_val(index);
    }
  }



template<typename eT>
inline
void
CoMat<eT>::erase_val(const uword index)
  {
  arma_extra_debug_sigprint();
  
  map_type& map_ref = (*map_ptr);
  
  typename map_type::iterator it     = map_ref.find(index);
  typename map_type::iterator it_end = map_ref.end();
  
  if(it != it_end)  { map_ref.erase(it); }
  }






// CoMat_val



template<typename eT>
arma_inline
CoMat_val<eT>::CoMat_val(CoMat<eT>& in_parent, const uword in_index)
  : parent(in_parent)
  , index (in_index )
  {
  arma_extra_debug_sigprint();
  }



template<typename eT>
arma_inline
CoMat_val<eT>::operator eT() const
  {
  arma_extra_debug_sigprint();
  
  const CoMat<eT>& const_parent = parent;
  
  return const_parent.operator[](index);
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator=(const CoMat_val<eT>& x)
  {
  arma_extra_debug_sigprint();
  
  const eT in_val = eT(x);
  
  parent.set_val(index, in_val);
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  parent.set_val(index, in_val);
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator+=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  if(in_val != eT(0))
    {
    eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
    
    val += in_val;
    
    if(val == eT(0))  { map_ref.erase(index); }
    }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator-=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  if(in_val != eT(0))
    {
    eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
    
    val -= in_val;
    
    if(val == eT(0))  { map_ref.erase(index); }
    }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator*=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  typename CoMat<eT>::map_type::iterator it     = map_ref.find(index);
  typename CoMat<eT>::map_type::iterator it_end = map_ref.end();
  
  if(it != it_end)
    {
    if(in_val != eT(0))
      {
      eT& val = (*it).second;
      
      val *= in_val;
      
      if(val == eT(0))  { map_ref.erase(it); }
      }
    else
      {
      map_ref.erase(it);
      }
    }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator/=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  typename CoMat<eT>::map_type::iterator it     = map_ref.find(index);
  typename CoMat<eT>::map_type::iterator it_end = map_ref.end();
  
  if(it != it_end)
    {
    eT& val = (*it).second;
    
    val /= in_val;
    
    if(val == eT(0))  { map_ref.erase(it); }
    }
  else
    {
    // silly operation, but included for completness
    
    const eT val = eT(0) / in_val;
    
    if(val != eT(0))  { parent.set_val(index, val); }
    }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator++()
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  val += eT(1);  // can't use ++,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator++(int)
  {
  arma_extra_debug_sigprint();
  
  (*this).operator++();
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator--()
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  val -= eT(1);  // can't use --,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  }



template<typename eT>
arma_inline
void
CoMat_val<eT>::operator--(int)
  {
  arma_extra_debug_sigprint();
  
  (*this).operator--();
  }





// CoMat_elem



template<typename eT>
arma_inline
CoMat_elem<eT>::CoMat_elem(CoMat<eT>& in_parent, const uword in_index, uword& in_sync_state, uword& in_n_nonzero)
  : parent    (in_parent    )
  , index     (in_index     )
  , sync_state(in_sync_state)
  , n_nonzero (in_n_nonzero )
  {
  arma_extra_debug_sigprint();
  }



template<typename eT>
arma_inline
CoMat_elem<eT>::operator eT() const
  {
  arma_extra_debug_sigprint();
  
  const CoMat<eT>& const_parent = parent;
  
  return const_parent.operator[](index);
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator=(const CoMat_elem<eT>& x)
  {
  arma_extra_debug_sigprint();
  
  const eT in_val = eT(x);
  
  parent.set_val(index, in_val);
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  parent.set_val(index, in_val);
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator+=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  if(in_val != eT(0))
    {
    eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
    
    val += in_val;
    
    if(val == eT(0))  { map_ref.erase(index); }
    
    sync_state = 1;
    n_nonzero  = parent.get_n_nonzero();
    }
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator-=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  if(in_val != eT(0))
    {
    eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
    
    val -= in_val;
    
    if(val == eT(0))  { map_ref.erase(index); }
    
    sync_state = 1;
    n_nonzero  = parent.get_n_nonzero();
    }
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator*=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  typename CoMat<eT>::map_type::iterator it     = map_ref.find(index);
  typename CoMat<eT>::map_type::iterator it_end = map_ref.end();
  
  if(it != it_end)
    {
    if(in_val != eT(0))
      {
      eT& val = (*it).second;
      
      val *= in_val;
      
      if(val == eT(0))  { map_ref.erase(it); }
      }
    else
      {
      map_ref.erase(it);
      }
    
    sync_state = 1;
    n_nonzero  = parent.get_n_nonzero();
    }
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator/=(const eT in_val)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  typename CoMat<eT>::map_type::iterator it     = map_ref.find(index);
  typename CoMat<eT>::map_type::iterator it_end = map_ref.end();
  
  if(it != it_end)
    {
    eT& val = (*it).second;
    
    val /= in_val;
    
    if(val == eT(0))  { map_ref.erase(it); }
    
    sync_state = 1;
    n_nonzero  = parent.get_n_nonzero();
    }
  else
    {
    // silly operation, but included for completness
    
    const eT val = eT(0) / in_val;
    
    if(val != eT(0))
      {
      parent.set_val(index, val);
      
      sync_state = 1;
      n_nonzero  = parent.get_n_nonzero();
      }
    }
  
  return *this;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator++()
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  val += eT(1);  // can't use ++,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return *this;
  }



template<typename eT>
arma_inline
eT
CoMat_elem<eT>::operator++(int)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  const eT old_val = val;
  
  val += eT(1);  // can't use ++,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return old_val;
  }



template<typename eT>
arma_inline
CoMat_elem<eT>&
CoMat_elem<eT>::operator--()
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  val -= eT(1);  // can't use --,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return *this;
  }



template<typename eT>
arma_inline
eT
CoMat_elem<eT>::operator--(int)
  {
  arma_extra_debug_sigprint();
  
  typename CoMat<eT>::map_type& map_ref = *(parent.map_ptr);
  
  eT& val = map_ref.operator[](index);  // creates the element if it doesn't exist
  
  const eT old_val = val;
  
  val -= eT(1);  // can't use --,  as eT can be std::complex
  
  if(val == eT(0))  { map_ref.erase(index); }
  
  sync_state = 1;
  n_nonzero  = parent.get_n_nonzero();
  
  return old_val;
  }



//! @}
