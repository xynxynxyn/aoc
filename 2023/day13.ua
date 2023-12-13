Solve! ← (
  /+≡(
    ∩(
      # Take even number of rows from both directions
      # Only works because every field has odd length
      ×2↙⌊÷2⧻.↘1⇡⧻.
      ⊃(∘|¤⋅∘|∘|¤⇌⋅∘)
      ⊂⊃(∘|⇌⋅∘)∩(≡(^2⇌.↙))
    )⍉.°□
    (+1⊙;|×1e2+1⊗1;)=⊃(⧻|.⊗1)
  )
)
&fras "input.txt"
⊜(□⊜∘≠@\n.)¬⌕"\n\n".
# Comparison functions
⊃(Solve!(=2/+♭≠)|Solve!≍)
