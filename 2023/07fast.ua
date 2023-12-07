Type ← ⬚0↯[5]⊏⍖.⊕⧻⊛.
Card ← ⊗:"23456789TJQKA"
TypeJ ← ⊡⊢⍖.≡(Type⍜⊏(+)⊚=0.:)+1⇡12¤ # Try all joker options
CardJ ← ⊗:"J23456789TQKA"
Solve!! ← (
  ⊜(⊟∩□⊃(^1↙5|⋕↘6))≠@\n.
  ∩⊏⊙:.⍏.≡(⊐^1⊢).   # Sort by type of hand
  ⍜⊜□(≡(□⊏⍏.°□))+1⊛ # Split by type of hand
  /+×+1⇡⧻.≡(°□⊢⇌)   # and sort lexographically
)
&fras "input.txt"
⊃(Solve!!Type Card|Solve!!TypeJ CardJ)
