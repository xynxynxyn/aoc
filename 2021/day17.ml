open Util
open Format

let parse_target s =
  let x_part, y_part =
    match String.split_on_char ',' s with
    | [ x; y ] ->
        ( String.split_on_char '=' x |> Fun.flip List.nth 1,
          String.sub y 3 (String.length y - 3) )
    | _ -> raise (Failure "parse_target")
  in
  let extract_range s =
    String.split_on_char '.' s |> function
    | [ lower; _; higher ] -> (int_of_string lower, int_of_string higher)
    | _ -> raise (Failure "parse_target")
  in
  (extract_range x_part, extract_range y_part)

let lands_on_target ((x0, x1), (y0, y1)) velocity =
  let rec loop (x, y) (dx, dy) =
    if x0 <= x && x <= x1 && y0 <= y && y <= y1 then true
    else if x1 < x || y < y0 then false
    else
      loop
        (x + dx, y + dy)
        ((if 0 < dx then dx - 1 else if dx < 0 then dx + 1 else dx), dy - 1)
  in
  loop (0, 0) velocity

let solve (((x0, x1), (y0, y1)) as target) =
  printf "%d..%d, %d..%d\n" x0 x1 y0 y1;
  let velocities = combinations (IntList.range 0 x1) (IntList.range y0 x1) in
  List.filter_map
    (fun v -> if lands_on_target target v then Some v else None)
    velocities

let part1 target =
  solve target
  |> List.map (fun (_, dy) -> dy * (dy + 1) / 2)
  |> List.sort (fun y y' -> ~-(Int.compare y y'))
  |> List.hd

let main =
  let targets = map_lines Sys.argv.(1) parse_target in
  List.map part1 targets |> List.iter (printf "part1: %d\n");
  List.map solve targets |> List.map List.length
  |> List.iter (printf "part2: %d\n")
