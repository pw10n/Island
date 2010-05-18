#pragma once

// takes fn (filename) and returns int (texture id)
unsigned int BindTextureBMP(const char *fn, bool tile);
// used to save time with missing textures
unsigned int BindFailSafe(bool tile);