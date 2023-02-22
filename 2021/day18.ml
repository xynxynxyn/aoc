open Util
open Format

type token = Open | Close | Comma | Number of int

let rec tokenize s =
  if String.length s = 0 then []
  else if String.starts_with ~prefix:"[" s then
    Open :: tokenize (skip_token "[" s)
  else if String.starts_with ~prefix:"]" s then
    Close :: tokenize (skip_token "]" s)
  else if String.starts_with ~prefix:"," s then
    Comma :: tokenize (skip_token "," s)
  else
    let i, rest = parse_int s in
    Number i :: tokenize rest

let to_string tokens =
  List.map
    (function
      | Open -> "[" | Close -> "]" | Comma -> "," | Number i -> string_of_int i)
    tokens
  |> String.concat ""

let rec add_to_first_digit x tokens =
  match tokens with
  | [] -> []
  | Number i :: tl -> Number (i + x) :: tl
  | t :: tl -> t :: add_to_first_digit x tl

let rec explode tokens =
  let rec loop prior tokens depth =
    match tokens with
    | [] -> List.rev prior
    | Open :: Number x :: Comma :: Number y :: Close :: tl ->
        if 4 <= depth then
          (add_to_first_digit x prior |> List.rev)
          @ (Number 0 :: add_to_first_digit y tl)
          |> explode
        else
          loop
            (Close :: Number y :: Comma :: Number x :: Open :: prior)
            tl depth
    | Open :: tl -> loop (Open :: prior) tl (depth + 1)
    | Close :: tl -> loop (Close :: prior) tl (depth - 1)
    | t :: tl -> loop (t :: prior) tl depth
  in
  loop [] tokens 0

let rec split tokens =
  match tokens with
  | [] -> []
  | Number i :: tl ->
      if i < 10 then Number i :: split tl
      else if i mod 2 = 0 then
        Open :: Number (i / 2) :: Comma :: Number (i / 2) :: Close :: tl
      else
        Open :: Number (i / 2) :: Comma :: Number ((i / 2) + 1) :: Close :: tl
  | t :: tl -> t :: split tl

let rec reduce tokens =
  let exploded = explode tokens in
  let next = split exploded in
  if next = tokens then next else reduce next

let add t0 t1 = (Open :: t0) @ (Comma :: t1) @ [ Close ] |> reduce

type tree = Leaf of int | Branch of tree * tree

let rec to_tree tokens =
  match tokens with
  | Number i :: tl -> (Leaf i, tl)
  | Open :: tl ->
      let lhs, rest = to_tree tl in
      let rhs, rest = to_tree (List.tl rest) in
      (Branch (lhs, rhs), List.tl rest)
  | _ -> raise (Failure "to_tree")

let rec magnitude_of_tree tree =
  match tree with
  | Leaf i -> i
  | Branch (l, r) -> (magnitude_of_tree l * 3) + (magnitude_of_tree r * 2)

let magnitude tokens = to_tree tokens |> fun (t, _) -> magnitude_of_tree t

let main =
  let numbers = map_lines Sys.argv.(1) tokenize in
  List.fold_left add (List.hd numbers) (List.tl numbers)
  |> magnitude |> printf "part1 %d\n";
  combinations numbers numbers
  |> List.map (fun (x, y) -> add x y |> magnitude)
  |> List.sort (fun x y -> -Int.compare x y)
  |> List.hd |> printf "part2: %d\n"
