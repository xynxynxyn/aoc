# Takes: Hand
# Returns: 6 = 5ofakind, 5 = 4ofakind, 4 = fullhouse
#   3 = 3ofakind, 2 = 2pair, 1 = 1pair, 0 = nothing
Type ← (
  ⊏⍖.⊕⧻⊛.
  [⊃(×6≍[5]|×5≍4_1|×4≍3_2|×3≍3_1_1|×2≍2_2_1|≍2_1_1_1)]
  /+
)
TypeJ ← (
  # Try every possible option for the jokers
  /↥≡(Type⍜⊏(+)⊚=@1.:)+1⇡12¤
)
# Takes: Replacement Match String
# Returns: String
R ← ⍜⊏(+⊚⧻)⊙::⊙(⊚=⊙.)
PP ← R@e@A R@d@K R@c@Q R@b@J R@a@T
PPJ ← R@=@A R@<@K R@;@Q R@:@T R@1@J
# Takes: Hand Hand (no bids only hand)
# Returns: 0 if equal or worse, 1 if first is better
Compare ← (
  ≍,,
  (((0;;|>∩⊡,:⊢⊚≠,,)|1;;;)⊃(>|=)∩Type,,|0)
)
CompareJ ← (
  ≍,,
  (((0;;|>∩⊡,:⊢⊚≠,,)|1;;;)⊃(>|=)∩TypeJ,,|0)
)
Hand ← ⊃(↙5|⋕↘6)
PartOne ← /+×wait≡spawn(+1/+≡(Compare):¤)⊃∘¤⊜Hand≠@\n.PP
PartTwo ← /+×wait≡spawn(+1/+≡(CompareJ):¤)⊃∘¤⊜Hand≠@\n.PPJ
PartTwo &fras "input.txt"
