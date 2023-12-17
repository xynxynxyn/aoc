InBounds ← /↧[⊃(≥0⊢|≥0⊢⇌|>∩⊢|>∩(⊢⇌))]
Pos ← ↙2↘1
Ex! ← [⍥(+^0.)]
Drive ← (
  ⊃(⊢|3|Pos)
  (Ex!0_1|Ex!1_0|Ex!0_¯1|Ex!¯1_0)
  ↘1⇌
)
UltraDrive ← (
  ⊃(⊢|10|Pos)
  (Ex!0_1|Ex!1_0|Ex!0_¯1|Ex!¯1_0)
  ↘1⇌
)
OtherDirections ← (
  (1_3|0_2)◿2
)
Next ← (
  ⊃(⊢|▽⊃(≡InBounds ⊙(¤△)⊙∘|∘)Drive|⋅∘)
  ⊙(⊃(∘|⊂.\+≡⊡⊙¤))
  ⊙¤OtherDirections
  ↯¯1_3≡(≡⊂¤)
)
UltraNext ← (
  ⊃(⊢|▽⊃(≡InBounds ⊙(¤△)⊙∘|∘)UltraDrive|⋅∘)
  ⊙(⊃(↘3∘|⊂.↘3\+≡⊡⊙¤))
  ⊙¤OtherDirections
  ↯¯1_3≡(≡⊂¤)
)
# Get the current distance to a node
D ← (
  ⊡⍜⊢(◿2)
)
# Update the distance to the min
U ← (
  ⍜⊢(◿2)
  ⊙:
  ⍜⊡↧
)
Solve! ← (
  ⊙(⊂¤.):⍜⊡(0;)0_0×∞⬚1↯:[]△.
  ⊃(☇1⇡⊂4△|⊙∘)
  ⍢(
    ⊃(⊢⍏≡D ⊙¤⊙⋅∘|⊙⊙∘)
    ⊃(⬚1▽¬°⊚|⊡)
    ⊙(
      ⊃(^2.2|D ⊙⋅∘|⋅⊙∘)
      ⊙+
      ⊃(⋅⋅∘|∧U ⊙⊙⋅∘)
    )
  | ≠0⧻
  )
  ↧⊃(⊡⊙∘|⊡⊙⋅∘)-1△.°⊟;;
)
&fras "input.txt"
⊜(∵⋕)≠@\n.
⊃(Solve!UltraNext|Solve!Next)
