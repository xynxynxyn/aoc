# Mark as visited
V ← ⊃(⊙∘|⍜⊡(↥1)⊙⋅∘)
# Mark as triggered
T ← ⊃(⊙∘|⍜⊡(2;)⊙⋅∘)
# Check if triggered
C ← ⊃(=2⊡⊙⋅∘|⊙⊙∘)
SplitV ← ⊂⊃(¤⊂1+1_0|¤⊂3-1_0)
SplitH ← ⊂⊃(¤⊂0+0_1|¤⊂2-0_1)
TurnR ← (¤⊂1+1_0|¤⊂2-0_1|¤⊂3-1_0|¤⊂0+0_1)
TurnL ← (¤⊂3-1_0|¤⊂0+0_1|¤⊂1+1_0|¤⊂2-0_1)
Cont ← (¤⊂0+0_1|¤⊂1+1_0|¤⊂2-0_1|¤⊂3-1_0)
OOB ← /↧[⊃(≥0⊢|>∩⊢|≥0⊢⇌|>∩(⊢⇌))]
# [D X Y], Field, Triggered/Energized
Step ← (
  ⊃(⊢|↘1)
  ⊙V
  ⊃(∘|/+[⊃(=@||×2=@-|×3=@\\|×4=@/)]⊡⋅⊙∘|⋅⊙⊙∘)
  (
    (Cont0|(SplitV T|[];)C|Cont0|TurnR0|TurnL0)
  | (Cont1|Cont1|(SplitH T|[];)C|TurnL1|TurnR1)
  | (Cont2|(SplitV T|[];)C|Cont2|TurnR2|TurnL2)
  | (Cont3|Cont3|(SplitH T|[];)C|TurnL3|TurnR3)
  )
)
Data ← ⊜∘≠@\n.
# Field, Triggered/Energized
# (0 = Nothing, 1 = Energized, 2 = Triggered)
Solve ← (
  ⊃(¤|∩¤:⬚0↯:[]△.⋅∘)
  ⍢(⊙∩¤▽≡(OOB↘1),¤△,⊙⊢⊙⊙(/↥)⊐/⊂≡(□Step)|≠0⧻.)
  /+>0♭;;
)
&fras "input.txt"
Data
⊃(
  °⊟△.
  ⊂⊂⊂⊃(
    ≡(⊂0⊂:0)⊙;⇡
  | ≡(⊂2⊂)⊙(¤-1)⇡
  | ≡(⊂1⊂0)⇡;
  | ≡(⊂3⊂:)⇡:¤-1
  )
  /↥≡Solve⊙¤
| Solve 0_0_0
)
