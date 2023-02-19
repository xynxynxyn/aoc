open Util
open Format
module PointSet = Set.Make (Point)
module PointMap = Map.Make (Point)

let dijkstra start goal cave =
  let dist =
    ref
      (Matrix.coords cave |> List.to_seq
      |> Seq.map (fun c -> (c, 1_000_000))
      |> PointMap.of_seq)
  in
  let q = ref (PointSet.empty |> PointSet.add start) in
  dist := PointMap.add start 0 !dist;

  while not (PointSet.is_empty !q) do
    (* find the vertext with the lowest distance *)
    let u =
      PointSet.to_seq !q
      |> Seq.map (fun c -> (c, PointMap.find c !dist))
      |> List.of_seq
      |> List.sort (fun (_, d) (_, d') -> Int.compare d d')
      |> List.map (fun (c, _) -> c)
      |> List.hd
    in
    (* early return if we find target *)
    if u = goal then q := PointSet.empty
    else (
      q := PointSet.remove u !q;
      List.iter
        (fun v ->
          let new_dist = PointMap.find u !dist + Matrix.get v cave in
          if new_dist < PointMap.find v !dist then (
            q := PointSet.add v !q;
            dist := PointMap.add v new_dist !dist))
        (Matrix.neighbor_coords u cave))
  done;
  PointMap.find goal !dist

let main =
  let cave =
    map_lines Sys.argv.(1) (fun s ->
        String.to_seq s
        |> Seq.map (fun c -> int_of_char c - int_of_char '0')
        |> List.of_seq)
  in
  let goal = Matrix.dimensions cave |> fun (x, y) -> (x - 1, y - 1) in
  dijkstra (0, 0) goal cave |> printf "part1: %d\n";
  let incr_risk i r = if i + r > 9 then ((i + r) mod 10) + 1 else i + r in
  let extended_cave =
    List.map
      (fun row ->
        List.init 5 (fun i -> List.map (incr_risk i) row) |> List.concat)
      cave
    |> fun section ->
    List.init 5 (fun i -> List.map (List.map (incr_risk i)) section)
    |> List.concat
  in
  let goal = Matrix.dimensions extended_cave |> fun (x, y) -> (x - 1, y - 1) in
  dijkstra (0, 0) goal extended_cave |> printf "part2: %d\n"
