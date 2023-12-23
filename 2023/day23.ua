# Experimental!
&fras "input.txt"
# Part 1
InBounds ← /↧[⊃(≥0⊢|≥0⊢⇌|>∩⊢|>∩(⊢⇌))]
Next ← (
  +[1_0 0_1 ¯1_0 0_¯1]⊃(¤|⋅∘)
  ▽⊃≡InBounds∘⊃(∘|¤△.⋅∘)
  ▽⊃≡(≠3⊡)∘⊙¤
)
Move ← (¤|⊟+0_1.|⊟+1_0.)⊃⊡∘
Step ← (
  ⊂⊃(Move☇0⊙⋅∘|⋅⊙∘)
  ⊃(¤∘|Next ⊢|⊙∘)
  ▽⊃(¬≡∊:|⋅∘)
)
Walk ← ↬(
  Step
  ⍢(Step|=1⧻)
  ⊃(¬≍∩⧻⊝.⋅∘|⊙∘)
  ((¤□;|∧(⊂°□):[]≡(|3 □↫¤)⊙∩¤)±⧻.|[])
)
⊜(⊗:".>v#")≠@\n.
PartOne ← (
  ⊃(∘|¤-1_2△)
  /↥-1≡⧻▽⊃(≡(≍⊢°□)|∘)Walk ¤0_1 []
)
⊃(≠3|&p$"1: _"PartOne)
# Part Two
Next ← (
  ⊃(+[1_0 0_1 ¯1_0 0_¯1]¤|⋅∘)
  ▽⊃≡InBounds∘⊃(∘|¤△.⋅∘)
  ▽⊃≡⊡∘⊙¤
)
RemoveVisited ← (
  ▽⊃(¬≡∊|∘)⊙¤
)
Step ← (
  ⊃(RemoveVisited⊙⋅∘|⋅⊙∘)⊃(Next|⋅∘|⊂⊙⋅∘)
)
Walk ← ↬(
  Step
  ⍢(Step☇0|=1⧻)
  ⊃(⊢|⧻)↘¯1⋅⋅∘
)
ConnectsTo ← ∩□≡(Walk⊃(∘|⋅⋅∘|¤⋅∘))⊃(Next|∩¤∘∘)
# Find all points of interest
⇌⊂⊂+¤1_1⊚≡≡(≥5/+⊂∩(⊡1)⊃∘⍉)◫3_3⊃(∘|-1_2△|0_1|∘)
# Walk from each PoI until we reach another PoI
⊃(≡ConnectsTo⊙¤|∘)
# Now we got the start/end and distance between
# each PoI
⊃(≡(⍜°□≡⊗)⊙(¤¤)⊙⋅∘|⋅⊙∘)
# Create a matrix showing the distance between PoIs
# 0 represents no connection
≡(⍜⊏;⊙:∩°□)⊃(⊙∘|⬚0↯:[]⊟.⧻⋅⋅∘)
# Current Index, Visited
Paths ← ↬(
  =1.
  (
    ⊃(∊|⊙⊙∘)
    (∧(⊂°□):[]≡(|3 □↫)⊙∩¤⊃(⊚≠0⊡⊙⋅∘|⊂⊙∘|⋅⋅∘)|[];;;)
  | ¤□⊂1⋅⊙;
  )
)
# Start at PoI 0 and recursively try to get to PoI 1
⊃(Paths 0 []|∘)
# Measure the length of a path of PoIs
Hike ← /+≡⊡⊙¤◫2°□
&p$"2: _"/↥≡Hike⊙¤
