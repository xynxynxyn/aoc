&fras "input.txt"
⊜∘≠@\n.
⊢⊚=@S.
# Start/Cur pos, Trail, Field
# start by going one step to the right
⊃(+1_0∘|¤∘|⋅∘)
⍢(
  ⊃(⊡⊙⋅∘|⊙⊙∘)
  ⊙.⊗:"|-LJ7F"
  (
    ⊃(+1_0)(-1_0)
  | ⊃(+0_1)(-0_1)
  | ⊃(-1_0)(+0_1)
  | ⊃(-1_0)(-0_1)
  | ⊃(+1_0)(-0_1)
  | ⊃(+1_0)(+0_1)
  )
  ↯¯1_2⊂
  ⊢▽⊃(¬≡≍⊙(¤⊢)⊙⋅∘|∘|⊂⋅⊙⊙∘)
)(⊢¬∊¤)
⊃(⋅⊙∘|÷2⧻⋅∘)
°⊚
⬚0×⊙(⊗:".|FL7J")
# Go through each line and toggle inside/outside
# This is complex because we have to remember the
# last angle change.
# Eg: F7 should toggle twice while FJ only toggles once
# We only toggle on 7 if the previous symbol was L
# and J only if the previous was a F 
/+≡(
  :0:0:0
  ∧(
    # Symbol, Last Symbol (NA/Up/Down), Inside?, Count
    (0(0∘|1+1);|0¬;|2¬;|1¬;|0(∘|∘|¬)|0(∘|¬|∘)|∘)
  )
  ;;
)
