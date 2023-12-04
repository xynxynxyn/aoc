Input ← &fras "input.txt"
IsDigit ← ↧≥@0:≤@9.
Symbols ← ↧¬IsDigit:≠@..
# Shift and copy the matrix in all 8 directions
Neighbors ← (
  ☇1-1⇡3_3 # Create all rotations [-1 -1]..[1 1]
  ⊙¤       # Fix the matrix for row to work
  ≡⬚0↻     # Apply all rotations
)

NumsAdjacentTo ← (
  /↥ Neighbors
  IsDigit.:
  ⊃(↧⊙⋅∘|⊙∘)               # [digits next to symbols] [digits] [input]
  ⍥(↥⊃(=2+⊙⋅∘|⋅⊙∘)⬚0≡↻1.)2 # Shift left/right to mark all adjacent digits
  ⍥(↥⊃(=2+⊙⋅∘|⋅⊙∘)⬚0≡↻¯1.)
  ▽≠0.♭⬚0≡(⊜parse);: # Parse the numbers and put them all in one array
)

PartOne ← (
  ⊜∘≠@\n.Input
  Symbols.
  NumsAdjacentTo
  /+
)
PartTwo ← (
  ⊜∘≠@\n.Input
  ⊚=@*.                         # Get the coords of every *
  ⊙(¤⬚0↯:0△.)                   # Create a blank copy of the field
  ≡(⍜⊡(1;))                     # Make a new field for each *
  ≡((0|/×)=2⧻.NumsAdjacentTo)⊙¤ # Do part one and multiply
  /+
)
PartOne
PartTwo
