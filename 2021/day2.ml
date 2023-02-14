open Util

type command = Forward of int | Up of int | Down of int

let parse s =
  let value = String.sub s (String.length s - 1) 1 |> int_of_string in
  if String.starts_with ~prefix:"forward" s then Forward value
  else if String.starts_with ~prefix:"down" s then Down value
  else if String.starts_with ~prefix:"up" s then Up value
  else raise (Failure "parse")

let part_one l =
  let rec _part_one l (x, y) =
    match l with
    | Forward dx :: tl -> _part_one tl (x + dx, y)
    | Up dy :: tl -> _part_one tl (x, y - dy)
    | Down dy :: tl -> _part_one tl (x, y + dy)
    | [] -> (x, y)
  in
  _part_one l (0, 0)

let part_two l =
  let rec loop l (x, y) aim =
    match l with
    | Forward dx :: tl -> loop tl (x + dx, y + (aim * dx)) aim
    | Up dy :: tl -> loop tl (x, y) (aim - dy)
    | Down dy :: tl -> loop tl (x, y) (aim + dy)
    | [] -> (x, y)
  in
  loop l (0, 0) 0

let main =
  let lines = fold_lines Sys.argv.(1) parse in
  let p1_x, p1_y = part_one lines in
  let p2_x, p2_y = part_two lines in
  Format.printf "part_one: (%d, %d) -> %d\n" p1_x p1_y (p1_x * p1_y);
  Format.printf "part_two: (%d, %d) -> %d\n" p2_x p2_y (p2_x * p2_y)
