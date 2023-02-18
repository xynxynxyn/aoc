open Util
open Format
module PointCounter = Counter.Make (Point)

module Octopus = struct
  (* Normal is the default state, flashed means 0 and ready means it is about to flash *)
  type t = Normal of int | Flashed | AboutToFlash

  let rec update x = function
    | Normal i -> if i + x > 9 then AboutToFlash else Normal (i + x)
    | AboutToFlash -> Flashed
    | Flashed -> Flashed

  let of_int x = Normal x

  let to_char = function
    | Normal i -> if i = 0 then 'F' else char_of_int (i + int_of_char '0')
    | AboutToFlash -> 'a'
    | Flashed -> 'f'
end

let print_octopi =
  let rec print_row r =
    List.to_seq r |> Seq.map Octopus.to_char |> String.of_seq |> printf "%s\n"
  in
  List.iter print_row

let parse_line s =
  String.to_seq s
  |> Seq.map (fun c -> int_of_char c - int_of_char '0' |> Octopus.of_int)
  |> List.of_seq

(** trigger all flashes, then return new octopi *)
let flash octopi =
  (* get the coordinates that have to be incremented *)
  let to_incr octopi =
    Matrix.map
      (fun c o ->
        match o with
        | Octopus.AboutToFlash -> Matrix.all_neighbor_coords c octopi
        | _ -> [])
      octopi
    |> List.flatten |> List.flatten
    |> List.filter (fun c ->
           match Matrix.get c octopi with
           | Octopus.Normal _ -> true
           | _ -> false)
    |> PointCounter.of_list
  in
  (* recursively increment everything that would flash and adjacent neighbors *)
  let rec loop octopi incr =
    (* if there is nothing left to update, we're done *)
    if PointCounter.cardinal incr = 0 then octopi
    else
      (* increment those coordinates unless they're 0 *)
      let octopi =
        Matrix.map
          (fun c o ->
            Octopus.update
              (PointCounter.count c incr |> Option.value ~default:0)
              o)
          octopi
      in
      loop octopi (to_incr octopi)
  in
  (* compute all the flashes *)
  let n = ref 0 in
  let new_octopi =
    Matrix.map
      (fun _ o ->
        match o with
        | Octopus.AboutToFlash | Octopus.Flashed ->
            n := !n + 1;
            Octopus.Normal 0
        | o -> o)
      (loop octopi (to_incr octopi))
  in
  (new_octopi, !n)

(** run the simulation for one step, returns new octopi and number of flashes *)
let step octopi =
  let octopi = Matrix.map (fun _ o -> Octopus.update 1 o) octopi in
  flash octopi

let rec simulate n octopi =
  if n > 0 then
    let octopi, flashed = step octopi in
    flashed + simulate (n - 1) octopi
  else 0

let rec solve2 octopi =
  let dim_x, dim_y = Matrix.dimensions octopi in
  let octopi, flashed = step octopi in
  if flashed = dim_x * dim_y then 1 else 1 + solve2 octopi

let main =
  let octopi = map_lines Sys.argv.(1) parse_line in
  printf "part1: %d\n" (simulate 100 octopi);
  printf "part2: %d\n" (solve2 octopi)
