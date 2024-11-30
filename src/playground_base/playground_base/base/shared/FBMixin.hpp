#pragma once

template <class DerivedT>
class FBMixin
{
public:
  DerivedT& Self() 
  { return static_cast<DerivedT&>(*this); }
  DerivedT const& Self() const
  { return static_cast<DerivedT const&>(*this); }
};