open Util

let rec part_one l =
  match l with
  | a :: b :: tl -> if a < b then 1 + part_one (b :: tl) else part_one (b :: tl)
  | _ -> 0

let rec part_two l =
  match l with
  | a :: b :: c :: d :: tl ->
      let x = part_two (b :: c :: d :: tl) in
      if a + b + c < b + c + d then 1 + x else x
  | _ -> 0

let main =
  let lines = map_lines Sys.argv.(1) int_of_string in
  part_one lines |> Format.printf "part_one: %d\n";
  part_two lines |> Format.printf "part_two: %d\n"
