Input ← &fras "input.txt"
Data ← ⍉⊜(⊜⋕↧⊃(≥@0|≤@9).)≠@\n.
TrueData ← ¤⊜(⋕▽↧⊃(≥@0|≤@9).)≠@\n.

Solve ← /×≡(/+>:×⇌.⇡+1⊃(⊢|⊢⇌))
∩Solve ⊃Data TrueData Input
