&fras "input.txt"
⊜(⊗:".#S")≠@\n.
Input ←
Default ← ×≠2.Input
Dims ← △ Default
Neighbors ← +[1_0 0_1 ¯1_0 0_¯1]¤
InBounds ← /↧[⊃(≥0⊢|≥0⊢⇌|>∩⊢|>∩(⊢⇌))]
Ex ← ⊙;⍥(⊂⊙.)⊃(-1⊙∘|⋅∘):
ExtendE ← ⊃∘(
  ≡⊂⊃(
    ∘
  | Ex Default÷∩⊢Dims△
  )⋅∘
)
ExtendS ← ⊃∘(
  ⊂⊃(
    ∘
  | ≡Ex Default¤÷∩(⊢⇌)Dims △
  )⋅∘
)
ExtendN ← ⊃(
  +¤×1_0Dims∘
| ⊂⊃(
    ≡Ex Default¤÷∩(⊢⇌)Dims △
  | ∘
  )⋅∘
)
ExtendW ← ⊃(
  +¤×0_1Dims∘
| ≡⊂⊃(
    Ex Default÷∩⊢Dims△
  | ∘
  )⋅∘
)
Any! ← ¬↧⊃(=0⊢|≍↻1.)≡^2
Step ← (
  ☇1≡Neighbors⊚=2.
  (⊙∘|ExtendN)Any!(<⊙⊢)0.
  (⊙∘|ExtendW)Any!(<⊙(⊢⇌))0.
  (⊙∘|ExtendS)Any!(≥∩⊢)⊃(¤△⋅∘|∘|⊙∘)
  (⊙∘|ExtendE)Any!(≥∩(⊢⇌))⊃(¤△⋅∘|∘|⊙∘)
  ▽⊃(≡InBounds⊙(¤△)⊙∘|⊙∘)
  ▽⊃(≡(≠1⊡)⊙¤|⊙∘)
  ⬚0+⊃(×2≠0°⊚∘|×≠2.⋅∘)
)
Input
≡(&p$"_")⊏+65×131⇡3⇌;⍥(⊃(Step|⊂/+♭=2))330:[]
