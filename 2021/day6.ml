open Util

type population = (int, int) Hashtbl.t

let new_pop =
  let open Hashtbl in
  let pop = create 8 in
  add pop 0 0;
  add pop 1 0;
  add pop 2 0;
  add pop 3 0;
  add pop 4 0;
  add pop 5 0;
  add pop 6 0;
  add pop 7 0;
  add pop 8 0;
  pop

let add_fish pop fish =
  let prev = Hashtbl.find pop fish in
  Hashtbl.replace pop fish (prev + 1)

let day pop =
  let open Hashtbl in
  let zeros = find pop 0 in
  let ones = find pop 1 in
  let twos = find pop 2 in
  let threes = find pop 3 in
  let fours = find pop 4 in
  let fives = find pop 5 in
  let sixes = find pop 6 in
  let sevens = find pop 7 in
  let eights = find pop 8 in
  replace pop 8 zeros;
  replace pop 7 eights;
  replace pop 6 (sevens + zeros);
  replace pop 5 sixes;
  replace pop 4 fives;
  replace pop 3 fours;
  replace pop 2 threes;
  replace pop 1 twos;
  replace pop 0 ones

let print_pop pop =
  Hashtbl.iter (fun k v -> Format.printf "(%d, %d) " k v) pop;
  Format.printf "\n"

let rec simulate pop days =
  if days > 0 then (
    day pop;
    simulate pop (days - 1))
  else Hashtbl.fold (fun _ v acc -> acc + v) pop 0

let main =
  let fish = fold_lines Sys.argv.(1) parse_list_of_ints |> List.hd in
  let pop = new_pop in
  List.iter (add_fish pop) fish;
  let pop2 = Hashtbl.copy pop in
  simulate pop 80 |> Format.printf "part1: %d\n";
  simulate pop2 256 |> Format.printf "part2: %d\n"
