# Parsing stuff
Node ← /(+×100)-@\0
Path ← ⊗:"LR"
Mapping ← ⊃(⊢|↘1)⊜Node∊:+@A⇡26.
# Takes: current position, step count, path, mappings
Walk ← ⊃(⊡⊃(⊡◿⧻,⋅⊙∘|⊡⊙⋅⋅∘)|+1⋅⊙⊙∘)
PartOne ← ⊙(;;);;⍢(⊙Walk)≠⊃(⊗909090⋅∘|⊗656565⋅∘|0|∘|⊗:⋅⊙∘)
Hash ← ⊂⊃(∘|◿⧻:⋅⊙∘)
PartTwo ← (
  ⊃(⋅∘|¤∘|¤⊗:⋅⊙∘)
  ⊃(⊗▽=0◿100-65|⊗▽=0◿100-90).. # all starting positions
  ⊃(⊙⋅⊙∘|⋅∘)
  :⊚⧻.    # step counters
  ¤¤¯1_¯1 # locations and modulo step count track
  ⬚¯1≡(⍢(⊃(⊂⊃(¤Hash⋅⊙⊙∘|∘)|Walk;))(¬∊⊃(Hash⋅⊙⊙∘|∘)))
  # Now we should be at the beginning of the loops
  # Calculate length of loop by finding the first
  # occurence of our current position
  ⊃(⊙⋅∘|+1≡(⊗:⊙Hash))
  # Loops, Loop Goal, Steps to get into loop
  ⊙(;;)≡(⧻≡⊢⇌↙):⊙⋅∘
)
&fras "input.txt"
⊐⊃(Path⊢|≡Mapping↘1)⊜□≠@\n.
⊃PartOne PartTwo
