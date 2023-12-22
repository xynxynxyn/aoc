Lowest ← ▽=¤/↧.≡(⊢⇌).
Occupied ← °⊚⊐/⊂
Drop ← (-¤0_0_1|∘)↥⊃(/↥≡⊡:¤|/↥≤0≡(⊢⇌)⋅∘)⊙(-¤0_0_1Lowest.)
Fall ← ⊙;⍢(≡(□Drop⊙°□)⊃(¤Occupied|∘).⊙;|¬≍):[]
SupportedBy ← (
  ⊃(∘|Occupied.⋅∘)
  ⊃(-¤0_0_1Lowest|¤⋅∘)
  ▽⊃(≡⊡|∘)
  □⊚≡(/↥∊°□):¤
)
PreProcess ← (
  Fall≡(□+⊃(↯¯1_3⇡+1-|¤)°⊟)
  ≡(SupportedBy°□)⊃∘¤
  ≡((∘|□¤¯1;)⊃(=0⧻°□|∘))
)
PartOne ← &p$"1:_"/+≡(=⊃(/+|⧻)≡(>1⧻)▽⊃(≡(∊⊙°□)|⋅∘)¤)⊃(⇡⧻|¤)
DroppedBy ← (
  ⊃⊙∘[]
  ⊙⋅;⍢(
    ⊃(⊙∘|∘)⊙⊙;
    ⊃(⊚≡(=0⧻)|∘)≡(□▽⊃(¬∊|∘)°□):¤
  | ¬≍⊙⋅∘
  )
)
PartTwo ← &p$"\n2:_"/+≡(&pf"*"⧻DroppedBy)⊃(⇌⇡⧻|¤)
&fras "input.txt"
⊜(⊟⊃↙↘3⋕♭regex"\\d+")≠@\n.
PreProcess
⊃PartTwo PartOne
