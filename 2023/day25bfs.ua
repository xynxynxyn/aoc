# Experimental!
&fras "input.txt"
⊜(⊃(□↘1|⊢)regex"\\w{3}")≠@\n.
⊃(∘|⊝⇌∧(⊂°□)⊙⇌)
⊙.
⊃(≡(⍜°□⊗)⊙¤|⬚0↯⊂.:[]⧻⋅∘)
↥⍉.⬚0↥⬚0≡(°⊚°□)
Choose ← ⌊×⚂⧻
Next ← ⊚⊡
RemoveVisited ← ▽¬⊃(≡(has)⊙¤|∘)
Backtrack ← ↘1;;⍢(
  ⊃(⊙⋅∘|⊂⊏⍏.⊂⊙⊙⋅∘)⊃(⊡|∘|⋅⊙∘)
| ≥0
)⊙⊙(↘1[0_0])
Bfs ← memo(
  ⊃(⊙¤|{}|-1⊚⧻.⋅⋅∘)
  # Goal, Queue, Visited, Parents, Adjacency Matrix
  ⍢⊙(
    ⊃(↘1|⊢)
    ⊙(
      # Cur, Visited, Parents, Adj M
      ⊃(∘|Next ⊙⋅⋅∘|insert:0|⋅⋅⊙∘)
      # Cur, Next, Visited, Parents, Adj M
      ⊃(∘|RemoveVisited ⋅⊙∘|⋅⋅⊙⊙∘)
      # Cur, FilteredNext, Visited, Parents, Adj M
      ⊃(
        ⋅⊙∘
        | ∧(⍜⊡;⊙:):¤⊙⊙⋅⊙∘
      )
      # FilteredNext, Visited, UpdatedParents, Adj M
    )
    ⊝⊂
  )(≠⊙⊢)
  Backtrack⊙⋅⋅∘
)
⊙{}
⍥(
  Bfs⊃(Choose|Choose|∘)
  ⊃(
    ⋅∘
  | ∧(insert:+1⊃(⍣get⋅⋅⋅0|⊙∘))⊙⋅∘
  )
)1000
⊏↙3⍖:°map;
≡≡⊡⊙(¤¤)
