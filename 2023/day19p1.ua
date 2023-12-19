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
PComponents ← ⊜(⋕☇0regex"\\d+")≠@\n.
PRule ← [⊃(PPart⊢|∊@>|⋕⊢♭regex"\\d+"|⊗□↘+1⊗@:.)]
PWorkflow ← (
  ⊃(⊃(□≡(PRule°□)⊙¤↘¯1|⊗⊢⇌)⊜□≠@,.⍜⇌(↘1)↘+1|⊗□↙)⊗@{.°□
)
# Rule, Component
# Returns index of workflow or `1 on failure
Apply ← (
  (¯1;|∘)(<|>)⊃(⊡1|⊡2|⊡⊢⊙∘|⊡3)
)
# WorkflowIndex, Component,
# WorkflowRules, WorkflowDefault, IndexMapping
Process ← ↬(
  >1.
  (
    (⊙(;;;)/+|0;;;;)
  | |5 ⊃(⊗⊙⋅⋅⋅∘|⋅⋅⊙∘|¤⋅∘|⋅⊙⊙⊙∘)
    ⊢▽≠¯1.⊂≡Apply⊃(°□⊡⊙∘|⋅⋅⋅∘|⊡⊙⋅∘)
    ↫
  ) # Early return if rejected or accepted
)

&fras "input.txt"
∩°□°⊟⊜□¬⌕"\n\n".
⊃(∘|PMappings|PComponents⋅∘)
⊃(⊙∘|⊗□"in"⋅∘|⋅⋅∘)
≡PWorkflow⊙¤⊜□≠@\n.
⊃(⋅⋅⋅⋅∘|⋅⋅⋅∘|⊙⊙∘)
# Components, Start, WorkflowRules,
# WorkflowDefault, Mapping
/+≡Process:⊙(⋔¤⊙⊙⊙∘)
