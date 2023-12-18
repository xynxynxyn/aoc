Delta ← (0_1|1_0|0_¯1|¯1_0)
Area ← (
  +⊃(
    # Trapezoid formula
    ÷2⌵/+≡(×⊃(+⊃(⊡0_0|⊡1_0)|-⊃(⊡1_1|⊡0_1)))
    # Count the number of edge tiles we have to duplicate
  | +1÷2/+≡(⌵/+-°⊟)
  )◫2
)
Solve ← (
  ⊃(⊙∘|[0_0])
  # Walk all the steps
  ∧(⊂+⊃(×:Delta⊙∘|⊢⋅⋅∘|⋅⋅∘))
  Area
)
Hex ← ∧(+⊙(×16)):0⊗:"0123456789abcdef"
PartOne ← Solve⊙⊙;
PartTwo ← Solve≡(⊃(⋕↘|Hex↙)5);;
&fras "input.txt"
⊜(⊃(⊗:"RDLU"⊢|⋕⊢♭regex"\\d+"|↙6↘+2⊗@(.))≠@\n.
⊃PartTwo PartOne
