open Util

type median = Single of int | Dual of int * int

let median l =
  let len = List.length l in
  let l = List.sort Int.compare l in
  if len mod 2 = 0 then Dual (List.nth l ((len / 2) - 1), List.nth l (len / 2))
  else Single (List.nth l (len / 2))

let fuel_cost goal crabs = List.map (fun x -> abs (goal - x)) crabs |> sum

let exp_fuel_cost goal crabs =
  List.map
    (fun x ->
      let n = abs (goal - x) in
      n * (n + 1) / 2)
    crabs
  |> sum

let brute_force crabs =
  let len = List.length crabs in
  let rec loop goal min_cost crabs =
    if goal >= len then min_cost
    else
      let cost = exp_fuel_cost goal crabs in
      if cost <= min_cost then loop (goal + 1) cost crabs else min_cost
  in
  loop 0 Int.max_int crabs

let main =
  let crabs = fold_lines Sys.argv.(1) parse_list_of_ints |> List.hd in
  let cost =
    match median crabs with
    | Single g -> fuel_cost g crabs
    | Dual (g0, g1) -> min (fuel_cost g0 crabs) (fuel_cost g1 crabs)
  in
  Format.printf "part1: %d\n" cost;
  brute_force crabs |> Format.printf "part2: %d\n"
