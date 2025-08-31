#pragma once

#include <firefly_base/dsp/shared/FBNoteMatrix.hpp>

void
FBNoteMatrixInitArrayFromScalar(
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array,
  FBNoteMatrix<float> const& scalar)
{
  array.last.velo.raw.Fill(scalar.last.velo.raw);
  array.last.velo.smooth.Fill(scalar.last.velo.smooth);
  array.last.keyUntuned.raw.Fill(scalar.last.keyUntuned.raw);
  array.last.keyUntuned.smooth.Fill(scalar.last.keyUntuned.smooth);
  array.lowKey.velo.raw.Fill(scalar.lowKey.velo.raw);
  array.lowKey.velo.smooth.Fill(scalar.lowKey.velo.smooth);
  array.lowKey.keyUntuned.raw.Fill(scalar.lowKey.keyUntuned.raw);
  array.lowKey.keyUntuned.smooth.Fill(scalar.lowKey.keyUntuned.smooth);
  array.highKey.velo.raw.Fill(scalar.highKey.velo.raw);
  array.highKey.velo.smooth.Fill(scalar.highKey.velo.smooth);
  array.highKey.keyUntuned.raw.Fill(scalar.highKey.keyUntuned.raw);
  array.highKey.keyUntuned.smooth.Fill(scalar.highKey.keyUntuned.smooth);
  array.lowVelo.velo.raw.Fill(scalar.lowVelo.velo.raw);
  array.lowVelo.velo.smooth.Fill(scalar.lowVelo.velo.smooth);
  array.lowVelo.keyUntuned.raw.Fill(scalar.lowVelo.keyUntuned.raw);
  array.lowVelo.keyUntuned.smooth.Fill(scalar.lowVelo.keyUntuned.smooth);
  array.highVelo.velo.raw.Fill(scalar.highVelo.velo.raw);
  array.highVelo.velo.smooth.Fill(scalar.highVelo.velo.smooth);
  array.highVelo.keyUntuned.raw.Fill(scalar.highVelo.keyUntuned.raw);
  array.highVelo.keyUntuned.smooth.Fill(scalar.highVelo.keyUntuned.smooth);
}

void
FBNoteMatrixInitScalarFromArrayLast(
  FBNoteMatrix<float>& scalar,
  FBNoteMatrix<FBSArray<float, FBFixedBlockSamples>>& array)
{
  scalar.last.velo.raw = array.last.velo.raw.Last();
  scalar.last.velo.smooth = array.last.velo.smooth.Last();
  scalar.last.keyUntuned.raw = array.last.keyUntuned.raw.Last();
  scalar.last.keyUntuned.smooth = array.last.keyUntuned.smooth.Last();
  scalar.lowKey.velo.raw = array.lowKey.velo.raw.Last();
  scalar.lowKey.velo.smooth = array.lowKey.velo.smooth.Last();
  scalar.lowKey.keyUntuned.raw = array.lowKey.keyUntuned.raw.Last();
  scalar.lowKey.keyUntuned.smooth = array.lowKey.keyUntuned.smooth.Last();
  scalar.highKey.velo.raw = array.highKey.velo.raw.Last();
  scalar.highKey.velo.smooth = array.highKey.velo.smooth.Last();
  scalar.highKey.keyUntuned.raw = array.highKey.keyUntuned.raw.Last();
  scalar.highKey.keyUntuned.smooth = array.highKey.keyUntuned.smooth.Last();
  scalar.lowVelo.velo.raw = array.lowVelo.velo.raw.Last();
  scalar.lowVelo.velo.smooth = array.lowVelo.velo.smooth.Last();
  scalar.lowVelo.keyUntuned.raw = array.lowVelo.keyUntuned.raw.Last();
  scalar.lowVelo.keyUntuned.smooth = array.lowVelo.keyUntuned.smooth.Last();
  scalar.highVelo.velo.raw = array.highVelo.velo.raw.Last();
  scalar.highVelo.velo.smooth = array.highVelo.velo.smooth.Last();
  scalar.highVelo.keyUntuned.raw = array.highVelo.keyUntuned.raw.Last();
  scalar.highVelo.keyUntuned.smooth = array.highVelo.keyUntuned.smooth.Last();
}