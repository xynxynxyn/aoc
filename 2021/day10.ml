open Util
module IntMedian = Median.Make (Int)

let char_score1 = function
  | ')' -> 3
  | ']' -> 57
  | '}' -> 1197
  | '>' -> 25137
  | _ -> 0

let char_score2 = function ')' -> 1 | ']' -> 2 | '}' -> 3 | '>' -> 4 | _ -> 0

let matching = function
  | '(' -> ')'
  | '[' -> ']'
  | '{' -> '}'
  | '<' -> '>'
  | _ -> failwith "match"

type error =
  | Ok
  | Incomplete of char list
  | Illegal of { expected : char; found : char }

let score = function
  | Ok -> 0
  | Incomplete _ -> 0
  | Illegal { found; _ } -> char_score1 found

let score2 = List.fold_left (fun acc c -> (5 * acc) + char_score2 c) 0

let check s =
  let rec parse chars stack =
    match (chars, stack) with
    | [], [] -> Ok
    | [], stack -> Incomplete stack
    | c0 :: ctl, c1 :: stl when c0 = c1 -> parse ctl stl
    | ((')' | ']' | '}' | '>') as found) :: _, expected :: _ ->
        Illegal { expected; found }
    | c :: tl, stack -> parse tl (matching c :: stack)
  in
  parse (String.to_seq s |> List.of_seq) []

let main =
  let lines = map_lines Sys.argv.(1) check in
  List.map score lines |> IntList.sum |> Format.printf "part1: %d\n";
  List.filter_map
    (function Incomplete chars -> Some (score2 chars) | _ -> None)
    lines
  |> IntMedian.of_list
  |> function
  | IntMedian.Single s -> Format.printf "part2: %d\n" s
  | _ -> raise (Failure "part2")
