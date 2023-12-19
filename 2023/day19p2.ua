# Experimental!
# Part: x = 0, m = 1, a = 2, s = 3
# Op: < = 0, > = 1
# Target: Index (0 = Accept, 1 = Refuse)
# Rule: [Part, Op, Value, Target]
# Workflow: [[Rule]] Default
# Component: [x m a s]
PPart ← ⊗:"xmas"
POp ← ⊗:"<>"
PMappings ← ⊂{"A""R"}⊜(□↙⊗@{.)≠@\n.
PRule ← [⊃(PPart⊢|∊@>|⋕⊢♭regex"\\d+"|⊗□↘+1⊗@:.)]
PWorkflow ← (
  ⊃(⊃(□≡(PRule°□)⊙¤↘¯1|⊗⊢⇌)⊜□≠@,.⍜⇌(↘1)↘+1|⊗□↙)⊗@{.°□
)
# Returns a new limit and next workflow with the
# rule applied
Applies ← (
  ⍜⊡((⊂⊢|⊂+1:⊢⇌):)⊃(⊢|⋅∘|⊡1|⊡2|⊡3)
)
# Returns a new limit where the inverse of the rule
# is applied
NotApplies ← (
  ⍜⊡((⊂:⊢⇌|⊂⊙(+1)⊢):)⊃(⊢|⋅∘|⊡1|⊡2)
)
Score ← (/×|0;)/↥≤0.≡(/-)
# Cur Index, Limits, Rules, Defaults, Mapping
Solve ← ↬(
  >1.
  (
    # Calculate score or 0 when we reach A/R
    (Score⊙(;;;)|0;;;;)
  | # Preprocessing to get the correct
    # index from the mapping
    ⊃(°□⊡⊙∘|⋅⋅⋅∘|⊡⊙⋅∘)⊃(⊗⊙⋅⋅⋅∘|⋅⋅⊙∘|⋅∘|⋅⋅⊙⊙∘)
    # Using fold we apply all the rules after each other
    # Collecting the intermediate results and next
    # workflows
    ∧(⊃(NotApplies|⊂⊟∩□Applies))⊃(⊙∘|↯¯1_2[])
    # Add the final rule with the default next workflow
    ⊂⊃(⊂∩□⊙⋅∘|⋅∘)
    ⊙(⋔¤⊙⊙∘)
    # Recursively go through all the new limits
    /+≡(|4 ↫:∩°□°⊟)
  )
)
&fras "input.txt"
°□⊢⊜□¬⌕"\n\n".
⊃(∘|PMappings)
⊃(⊙∘|⊗□"in"⋅∘)
≡PWorkflow⊙¤⊜□≠@\n.
[...1_4001]
Solve ⊃(⋅⋅⋅⋅∘|⊙⊙⊙∘)
