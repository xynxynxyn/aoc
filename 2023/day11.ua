PartOne ← (
  ⍥(⍉▽+1≡(=0/+).)2
  ⊝≡(♭⊏⍖.)☇2⊠⊟.⊚
  /+≡(+⊃(⌵-⊙⋅∘|⌵-⋅⊙⋅∘)°[⊙⊙⊙∘])
)
Extra ← -:×1e6./+↧⊃(<⋅⊙∘|>⊙⋅∘)⊃↧↥
PartTwo ← (
  ⍥(⍉:¤⊚≡(=0/+).)2
  ⊝≡(♭⊏⍖.)☇2⊠⊟.⊚
  ≡(
    °[⊙⊙⊙∘]
    +++⊃(⌵-⊙⋅∘|⌵-⋅⊙⋅∘|∩Extra⊃(⊙⋅⊙⋅⋅∘|⋅⊙⋅⊙∘))
  )
  /+
)
&fras "input.txt"
=@#⊜∘≠@\n.
⊃PartOne PartTwo
