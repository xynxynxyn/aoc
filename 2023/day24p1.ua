Normalize ← ÷⊃(√/+ⁿ2|∘)
Parallel ← ↥⊃(≍|≍¯)∩Normalize
Inv ← ×⊃(
  ×[1_¯1 ¯1_1][⊃(⊏[3 1]|⊏[2 0])]
| ÷:1-∩/×⊃(⊏[1 2]|⊏[0 3])
)♭
Mul ← ≡(/+×)⊙¤
MatrixA ← ⍉⍜(⊡1)¯[∩(⊢⍉)]
SolveEquation ← Mul⊃(Inv MatrixA|-∩(⊡1⍉))
ApplySolution ← +×⊃(⊢|°⊟⍉⊢⋅∘)
Solve!! ← (
  ⊝≡(⊏⍖.)☇3⊠⊟.≡(↘¯1)                          # Get rid of Z and create combinations
  ▽⊃(¬≡(Parallel∩(⊢⍉)°⊟)|∘)                   # Remove parallel couples
  ⊃(▽⊙∘|▽⊙⋅∘)≡(/↧>0).≡(⊃(SolveEquation °⊟|∘)) # Solve the equation set
  ≡ApplySolution                              # Insert solution back into first formula
  ▽↧⊃(≡(/↧≤^0)|≡(/↧≥^0)|∘)                    # Check for bounds
  &p$"1: _"⧻
)
&fras "input.txt"
≡(⍉⊟)⊜(⊃↘↙3⋕♭regex"-?\\d+")≠@\n.
Solve!!4e14 2e14
