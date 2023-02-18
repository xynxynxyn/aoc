open Util
open Format
module PointSet = Set.Make (Point)

type fold = FoldAlongX of int | FoldAlongY of int

let parse_folds =
  let extract_fold_int s =
    String.sub s 13 (String.length s - 13) |> int_of_string
  in
  let parse_fold s =
    if String.starts_with ~prefix:"fold along x" s then
      FoldAlongX (extract_fold_int s)
    else if String.starts_with ~prefix:"fold along y" s then
      FoldAlongY (extract_fold_int s)
    else raise (Failure "parse_folds")
  in
  Seq.map parse_fold

let parse_points =
  let parse_point s =
    String.split_on_char ',' s |> List.map int_of_string |> fun l ->
    (List.nth l 0, List.nth l 1)
  in
  Seq.map parse_point

let rec parse xs =
  List.to_seq xs |> fun s ->
  ( Seq.take_while (( <> ) "") s |> parse_points |> PointSet.of_seq,
    Seq.drop_while (( <> ) "") s |> Seq.drop 1 |> parse_folds |> List.of_seq )

let fold points instruction =
  match instruction with
  | FoldAlongX fold_x ->
      PointSet.map
        (fun (x, y) ->
          if x > fold_x then (x - (2 * (x - fold_x)), y) else (x, y))
        points
  | FoldAlongY fold_y ->
      PointSet.map
        (fun (x, y) ->
          if y > fold_y then (x, y - (2 * (y - fold_y))) else (x, y))
        points

let print_points points =
  let rec loop points cur_line cur_char =
    match points with
    | [] -> printf "\n"
    | (x, y) :: tl ->
        if cur_line < y then (
          printf "\n%s#" (String.make x ' ');
          loop tl y (x + 1))
        else (
          printf "%s#" (String.make (x - cur_char) ' ');
          loop tl y (x + 1))
  in
  loop (PointSet.to_seq points |> List.of_seq) 0 0

let main =
  let points, folds = lines Sys.argv.(1) |> parse in
  printf "part1: %d\n" (fold points (List.hd folds) |> PointSet.cardinal);
  List.fold_left (fun points instr -> fold points instr) points folds
  |> fun p ->
  printf "part2:\n";
  print_points p
