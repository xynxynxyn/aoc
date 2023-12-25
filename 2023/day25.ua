$ jqt: rhn xhk nvd
$ rsh: frs pzl lsr
$ xhk: hfx
$ cmg: qnr nvd lhk bvb
$ rhn: xhk bvb hfx
$ bvb: xhk hfx
$ pzl: lsr hfx nvd
$ qnr: nvd
$ ntq: jqt hfx bvb xhk
$ nvd: lhk
$ lsr: lhk
$ rzs: qnr cmg lsr rsh
$ frs: qnr lhk lsr
⊜(⊃(□↘1|⊢)regex"\\w{3}")≠@\n.
⊃(∘|⊝⇌∧(⊂°□)⊙⇌)
⊃(≡(⍜°□⊗)⊙¤|⬚0↯⊂.:[]⧻⋅∘)
↥⍉.⬚0↥⬚0≡(°⊚°□)
Empty ← =0/+
Size ← /+¬≡Empty
Choose ← ⊡⌊×⚂⧻.
Repair ← ∧(⍜⊡(0;))≡⊟.⇡⧻.
Merge ← (
  ⊟
  ⊃(∘|⍉⍜⊏([⊃(/+|×0⊢)]))
  ⍉⍜⊏([⊃(/+|×0⊢)])
  Repair
)
FastMerge ← (
  ⊟
  ⊃(∘|⍜⊏([⊃(/+|×0⊢)]))
  ≡(⍜⊏([⊃(/+|0)]))¤
  Repair
)
PickPair ← ⍜⊟(⊏⍏.)⊃(Choose⊚>0⊡|∘)⊃(Choose⊚¬≡Empty|∘)
PP ← &p""≡(&p↘1♭≡⊂ @,)∵($"_")
TrackerInit ← ∵(□¤)⇡⧻
TrackerUpdate ← ⍜⊏(⊟∩□:[]⊂∩°□°⊟)⊟
Karger ← (
  ⊃(∘|TrackerInit)
  ⍢(
    ⊃(PickPair|⊙∘)
    ⊃(FastMerge|TrackerUpdate⊙⊙⋅∘)
  | >2Size
  )
  ⊃(⊢⊏⍖.♭|⊢⊚>0)
)
⊃(0 0 0|∘)
⍢(
  ⊃(Karger|∘);;;
| ≠3
)
/×∵⧻⊏⋅⊙⊙;
