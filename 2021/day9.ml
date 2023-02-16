open Util
module PointSet = Set.Make (Point)

let is_min p m =
  let lvl = Matrix.get p m in
  Matrix.neighbors p m |> List.for_all (fun i -> i > lvl)

let parse s =
  String.to_seq s
  |> Seq.map (fun c -> int_of_char c - int_of_char '0')
  |> List.of_seq

let solve1 m =
  Matrix.filteri (fun p _ -> is_min p m) m |> fun l ->
  List.length l + IntList.sum l

let flows_to p m =
  Matrix.(
    let p_elem = get p m in
    if p_elem = 9 then []
    else List.filter (fun c -> get c m < p_elem) (neighbor_coords p m))

let flows_from p m =
  Matrix.(
    let p_elem = get p m in
    List.filter
      (fun c ->
        let elem = get c m in
        if elem = 9 then false else elem > p_elem)
      (neighbor_coords p m))

let rec expand_basin m coords =
  let adjacent =
    PointSet.to_seq coords
    |> Seq.flat_map (fun c ->
           (* only look for adjacent fields that are higher than the basin *)
           flows_from c m |> List.to_seq)
    (* ignore coords that are already explored and ones that flow outside of the basin *)
    |> Seq.filter (fun c ->
           (not (PointSet.mem c coords))
           (* all fields that the neighbor flows to have to be in the basin *)
           && List.for_all (fun c -> PointSet.mem c coords) (flows_to c m))
  in
  if Seq.is_empty adjacent then coords
  else
    let new_coords = PointSet.add_seq adjacent coords in
    expand_basin m new_coords

let basins m =
  Matrix.coords m
  |> List.filter (fun p -> is_min p m)
  |> List.map (fun start -> expand_basin m PointSet.(empty |> add start))

let main =
  let map = map_lines Sys.argv.(1) parse in
  Format.printf "part1: %d\n" (solve1 map);
  basins map |> List.map PointSet.cardinal
  |> List.sort (fun x y -> -Int.compare x y)
  |> List.to_seq |> Seq.take 3 |> Seq.fold_left ( * ) 1
  |> Format.printf "part2: %d\n"
