#pragma once

template <class DerivedT>
class FBMixin
{
public:
  DerivedT& This() 
  { return *static_cast<DerivedT*>(this); }
  DerivedT const& This() const
  { return *static_cast<DerivedT const*>(this); }
};