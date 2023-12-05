ToRange ← [⊃(⊢|/+)]
IsDigit ← ↧⊃(≥@0|≤@9)
ToIntArr ← ⊜parse IsDigit.
Data ← (
  # Seeds, Mappings
  ⊜□¬⌕"\n\n".&fras"input.txt"
  ⊃(
    ToIntArr ⊔⊢↙
  | ⊐≡(≡(⍜⊢;:/-⊏1_0.⍜⊏ToRange1_2⊔)↘1⊜ToIntArr≠@\n.)↘
  )1
)

# Takes: [[offset start end]] seed
# Returns: new seed
Map ← (
  ⬚:⊢▽≠⊙.,≡((;|+)↧⊃(≥⊡1|≤⊡2|⊡0))⊙(..)⊔
)
# Takes: range0 range1
# Returns: intersection (might be empty)
Intersection ← (
  ⊂⊃(↥∩⊢|↧∩(⊢⇌))
)
# Takes: Int Range
# Returns: [0|1]
InRange ← ↧⊃(≥⊢|≤⊢⇌):
IsEmpty ← ≤0≡/-
# Takes: [offset start end] [first last]
# Returns: mapped_range [0, 1 or 2 ranges not covered]
MapOneRange ← (
  ⊃(⊢|Intersection⊏1_2)
  ⊂
)
# Takes: [Mappings] Range
# Returns: [Range]
MapRange ← (
  ⊙(¤.)⊔
  ≡MapOneRange
  ▽¬IsEmpty≡↘1.
  ⊙:≡↘1. # Split up the original range into unused parts
  ↯¯1_2⊏⍏.⊂♭
  ▽¬IsEmpty.
  ⊂⊙(≡(+⊃(↘1|⊢)))
)
PartOne ← /↧≡(∧Map:)⊙¤
PartTwo ← (
  ≡ToRange↯¯1_2 # Convert seeds to ranges
  ⊙¤
  /↧≡(
    :¤
    ∧(
      ⊐∧⊂:[]≡(□MapRange)
    )
    /↧≡⊢
  )
)
⊃PartOne PartTwo Data
