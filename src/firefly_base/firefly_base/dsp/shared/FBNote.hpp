#pragma once

#include <compare>

struct FBNote final
{
  int id = -1;
  int channel = -1;
  int keyUntuned = -1;

  bool Matches(FBNote const& rhs) const;
  auto operator<=>(FBNote const&) const = default;
};

inline bool 
FBNote::Matches(FBNote const& rhs) const
{
  bool matchId = id == -1 || rhs.id == -1 || id == rhs.id;
  bool matchChannel = channel == -1 || rhs.channel == -1 || channel == rhs.channel;
  bool matchKey = keyUntuned == -1 || rhs.keyUntuned == -1 || keyUntuned == rhs.keyUntuned;
  return matchChannel && matchKey && matchId;
}