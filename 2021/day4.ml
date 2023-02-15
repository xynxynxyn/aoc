open Util

type field = { value : int; mutable marked : bool }

let rec parse_bingo_line s =
  match skip_ws s |> parse_int with
  | i, s when s = "" -> [ { value = i; marked = false } ]
  | i, s -> { value = i; marked = false } :: parse_bingo_line s

let rec parse_boards l =
  if List.length l < 6 then []
  else
    let l = List.to_seq l in
    let bl =
      Seq.drop 1 l |> Seq.take 5 |> Seq.map parse_bingo_line |> List.of_seq
    in
    bl :: parse_boards (Seq.drop 6 l |> List.of_seq)

let has_bingo board =
  (* check rows *)
  List.exists (List.for_all (fun f -> f.marked)) board
  (* check columns *)
  || Seq.exists
       (fun i -> List.for_all (fun row -> (List.nth row i).marked) board)
       (Seq.init 5 (fun n -> n))

(** return a bingo board if there exists a bingo *)
let rec check_bingo boards =
  match boards with
  | [] -> None
  | b :: tl -> if has_bingo b then Some b else check_bingo tl

let rec find num board =
  match board with
  | [] -> None
  | b :: tl -> (
      match List.find_opt (fun f -> f.value = num) b with
      | None -> find num tl
      | field -> field)

(** play a number on one board *)
let play num board =
  (match find num board with Some field -> field.marked <- true | None -> ());
  board

let score board =
  List.fold_left
    (fun acc row ->
      acc
      + List.fold_left
          (fun acc field -> if field.marked then acc else acc + field.value)
          0 row)
    0 board

let rec play_game numbers boards =
  match numbers with
  | [] -> raise (Failure "no bingos")
  | n :: tl -> (
      let boards = List.map (play n) boards in
      match check_bingo boards with
      | None -> play_game tl boards
      | Some b -> n * score b)

let play_all numbers boards =
  let rec _play_all numbers boards last_score =
    if List.length boards = 0 then Option.get last_score
    else
      match numbers with
      | [] -> Option.get last_score
      | n :: tl -> (
          let boards = List.map (play n) boards in
          match check_bingo boards with
          | None -> _play_all tl boards last_score
          | Some winner ->
              (* remove the boards that won and record the score *)
              _play_all tl
                (List.filter (fun b -> not (has_bingo b)) boards)
                (Some (n * score winner)))
  in
  _play_all numbers boards None

let rec print_board board =
  match board with
  | [] -> ()
  | row :: tl ->
      List.iter
        (fun f ->
          if f.marked then Format.printf "%3d!" f.value
          else Format.printf "%4d" f.value)
        row;
      Format.printf "\n";
      print_board tl

let main =
  let lines = fold_lines Sys.argv.(1) (fun s -> s) in
  let numbers = List.hd lines |> IntList.of_string in
  let boards = parse_boards (List.tl lines) in
  Format.printf "part one: %d\n" (play_game numbers boards);
  Format.printf "part two: %d\n" (play_all numbers boards)
