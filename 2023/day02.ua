Input = &fras "input.txt"
SplitLines ← ⊜□≠@\n.
IsDigit ← ≤@9
Default ← (
  # This checks if there exists a r/g/b value
  # If it does not create a placeholderr
  ¬∊@r≡⊢.
  (∘|⊂ [□@r □0])
  ¬∊@g≡⊢.
  (∘|⊂ [□@g □0])
  ¬∊@b≡⊢.
  (∘|⊂ [□@b □0])
)
# Parse one set of dice by separating by commas
# and cleaning up the data
PSet ← (
  # Each set becomes { r1 g20 b1 } for example
  ⊜(⊂⊃(⊢▽¬IsDigit.|parse▽ IsDigit.)▽≠@ .)≠@,.
  # Insert default values by making a char array of the
  # colors present
  Default
  # Sort so eveything is rgb
  ⊏⍖.
  # Then remove the redundant character so we end up
  # with a nice [3] num array for each set of dice
  ≡⊡1
)
# Parse the input proper
P ← (
  SplitLines
  ⊐≡(⊡1⊜∘≠@:.) # Get rid of the game prefix
  # Split each line by semicolon
  ⊐≡(⊜PSet≠@;.)
)

# For part one we check if any of the dice are larger
# than the reference set. Then we sum up all the ones
# to find the games in which this is the case
#
PartOne ← (
  # Parse the Input
  P Input
  # Find all the rows where there are more dice taken than
  # allowed
  ⊐≡(/+/+≡(>[12 13 14]))
  # If a set has a 0 that means no rules broken
  =0
  # Get the indices of each game and sum them up
  /+▽:+1⇡⧻.
)

PartTwo ← /+⊐≡(/×/↥) P Input

PartOne
PartTwo
