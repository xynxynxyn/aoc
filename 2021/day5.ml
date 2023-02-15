open Util

type line = { src : int * int; tgt : int * int }

let parse_line s =
  let src_x, s = parse_int s in
  let src_y, s = skip_token "," s |> parse_int in
  let tgt_x, s = skip_token " -> " s |> parse_int in
  let tgt_y, s = skip_token "," s |> parse_int in
  { src = (src_x, src_y); tgt = (tgt_x, tgt_y) }

let is_aligned { src = x0, y0; tgt = x1, y1 } = x0 = x1 || y0 = y1
let is_horizontal { src = _, y0; tgt = _, y1 } = y0 = y1
let is_vertical l = not (is_horizontal l)
let is_diagonal { src = x0, y0; tgt = x1, y1 } = abs (x1 - x0) = abs (y1 - y0)

let length { src = x0, y0; tgt = x1, y1 } =
  max (abs (x1 - x0)) (abs (y1 - y0)) + 1

let covers ({ src = x0, y0; tgt = x1, y1 } as line) =
  if is_horizontal line && y0 = y1 then
    let x0 = min x0 x1 and x1 = max x0 x1 in
    range x0 x1 |> Seq.map (fun x -> (x, y0))
  else if is_vertical line && x0 = x1 then
    let y0 = min y0 y1 and y1 = max y0 y1 in
    range y0 y1 |> Seq.map (fun y -> (x0, y))
  else if is_diagonal line then
    (* find out what kind of diagonal *)
    let x0, y0, x1, y1 =
      if x0 < x1 then (x0, y0, x1, y1) else (x1, y1, x0, y0)
    in
    if y0 < y1 then
      (* line goes down *)
      range 0 (length line - 1) |> Seq.map (fun i -> (x0 + i, y0 + i))
    else
      (* line goes up *)
      range 0 (length line - 1) |> Seq.map (fun i -> (x0 + i, y0 - i))
  else raise (Failure "covers")

let solve lines =
  let grid = Grid.make 1000 1000 0 in
  let mark line =
    Seq.iter (fun (x, y) -> Grid.mut x y (fun i -> i + 1) grid) (covers line)
  in
  let () = List.iter mark lines in
  let count = ref 0 in
  Grid.iter (fun _ _ e -> if e >= 2 then count := !count + 1) grid;
  !count

let main =
  let lines = fold_lines Sys.argv.(1) parse_line in
  Format.printf "part1: %d\n" (List.filter is_aligned lines |> solve);
  Format.printf "part2: %d\n" (solve lines)
