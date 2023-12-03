Input ← &fras "input.txt"
IsDigit ← ↧≥@0:≤@9.
Symbols ← ↧¬IsDigit:≠@..
Shift ← ⬚0≡↻
# Shift and copy the matrix in all 8 directions
Neighbors ← (
  ⊃(
    Shift1             # Left
  | Shift¯1            # Right
  | ⍜⍉(Shift1)         # Up
  | ⍜⍉(Shift¯1)        # Down
  | Shift1⍜⍉(Shift1)   # Up Left
  | Shift¯1⍜⍉(Shift1)  # Up Right
  | Shift1⍜⍉(Shift¯1)  # Down Left
  | Shift¯1⍜⍉(Shift¯1) # Down Right
  )
  [⊙⊙⊙⊙⊙⊙⊙∘]
)

NumsAdjacentTo ← (
  /↥ Neighbors
  IsDigit.:
  ⊃(↧⊙⋅∘|⊙∘)                 # [digits next to symbols] [digits] [input]
  ⍥(↥⊃(=2+⊙⋅∘|⋅⊙∘)Shift1.)2  # Shift left/right to nsum
  ⍥(↥⊃(=2+⊙⋅∘|⋅⊙∘)Shift¯1.)2 # adjacent digits
  ▽≠0.♭⬚0≡(⊜parse);:         # Parse the numbers and put them all in one array
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
