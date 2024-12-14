#pragma once

struct FBNote
{
  int id = -1;
  int key = -1;
  int channel = -1;

  bool Matches(FBNote const& rhs) const;
  auto operator<=>(FBNote const&) const = default;
};

inline bool 
FBNote::Matches(FBNote const& rhs) const
{
  bool matchId = id == -1 || rhs.id == -1 || id == rhs.id;
  bool matchKey = key == -1 || rhs.key == -1 || key == rhs.key;
  bool matchChannel = channel == -1 || rhs.channel == -1 || channel == rhs.channel;
  return matchChannel && matchKey && matchId;
}