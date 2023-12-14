Tilt! ← ≡(⍜⊜□≡(□⊏^1.°□)≠@#.)
Score ← /+≡(×/+=@O):+1⇌⇡⧻.
PartOne ← Score ⍜⍉Tilt!⍖
Cycle ← Tilt!⍏⍜⍉Tilt!⍏Tilt!⍖⍜⍉Tilt!⍖
PartTwo ← (
  :▽[0][∘].         # Prepare stack for loop
  ⊙⇌⍢(Cycle ⊙⊂.|¬∊) # Cycle until we reach a prev grid
  ⊙-⊃(.⊗|⧻⋅∘|⋅∘)    # Calc loop length and steps to enter
  ⊡+◿⊃(⋅∘|-:1e9|∘)  # Check where we end up
  Score
)
&fras "input.txt"
⊜∘≠@\n.
⊃PartTwo PartOne
