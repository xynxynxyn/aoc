h ← ◿256×17+
Hash ← ∧h:0-@\0
PartOne ← /+≡(Hash°□)
Insert ← ⊃(⍜⊡(□⊂□:°□)⊙:⊙⊙⋅∘|⍜⊡(□⊂:°□)⊙:⊙⋅⊙⋅∘)Hash.
Delete ← (
  ≡(¬≍):⊃(□⋅∘|°□⊡⊙⋅∘|∘|⋅⋅⊙∘)Hash.
  ⊃(⍜⊡(□▽:°□)⊙:⊙⊙∘|⍜⊡(□▽:°□)⊙:⊙⊙⋅∘):
)
Replace ← (
  ⊃(⊗□:°□⊡⊙:⊙⊙⋅∘|⊙⋅⊙⋅∘|⋅⋅⋅∘)Hash.
  :⍜⊡(□⍜⊡;:°□)⊃(⋅⊙⋅∘|⊙⋅∘)
)
Check ← (
  ⊙;∊□:°□⊡⊙:⊙⊙∘Hash.
)
Update ← (Insert|Replace)⊃(Check ⊙⋅⊙∘|⊙⊙⊙∘)
PartTwo ← (
  ⊙(.[⍥(□[])256])
  ∧((Delete ↙⊗@-.|Update ⊃↙(⋕↘+1)⊗@=.)∊@=.°□)
  /+≡(/+×:×+1⇡⧻.⇌°□:)+1⇡⧻.;
)
⊜□≠@,.⍜⇌(↘1)&fras "input.txt"
⊃PartTwo PartOne
