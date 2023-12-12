Input ← &fras "input.txt"
PNums ← ⊜parse↧⊃(≥@0|≤@9).
Data ← ⊜(⊟⊃(□PNums↙|□PNums↘+1)⊢⊚=@|. ↘+1⊢⊚=@:.)≠@\n.Input
PartOne ← (
  /+≡(
    ∩⊔⊃(⊡0|⊡1)
    (ⁿ:2-1|;0)=0./+∊
  )
)
PartTwo ← (
  ⊐≡⊟⇡⧻.
  :+1⊚⧻. # Array to store the number of duplicates
  /+∧(
    ⊃(∩⊔⊃(⊡0|⊡1)⊔⊡1|⊔⊡0)
    /+∊      # Wins, Index, Acc
    ⊙⊃(⊙∘|⊡) # Get the duplicate count of current card
    ++1:⇡    # New card indices, Acc, Amount to add
    ⍜⊏(+)
  )
)
PartOne Data
PartTwo Data
