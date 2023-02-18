open Util
open Format
module PairCounter = Counter.Make (Point)
module IntCounter = Counter.Make (Int)

type state = { pairs : PairCounter.t; chars : IntCounter.t }

let rec extract_pairs ctr xs =
  match xs with
  | [] -> ctr
  | [ a ] -> ctr
  | a :: b :: tl -> extract_pairs ctr (b :: tl) |> PairCounter.incr (a, b)

let parse_header s =
  let pairs =
    String.to_seq s |> Seq.map int_of_char |> List.of_seq
    |> extract_pairs PairCounter.empty
  in
  let chars =
    String.to_seq s |> List.of_seq |> List.map int_of_char |> IntCounter.of_list
  in
  { pairs; chars }

let parse_rule s =
  String.to_seq s |> Seq.map int_of_char |> List.of_seq |> function
  | a :: b :: _ :: _ :: _ :: _ :: c :: _ -> ((a, b), c)
  | _ -> raise (Failure "parse_rule")

let grow rules state =
  PairCounter.bindings state.pairs
  |> List.fold_left
       (fun state ((a, b), i) ->
         let c = List.assoc (a, b) rules in
         {
           pairs =
             PairCounter.decr_by (a, b) i state.pairs
             |> PairCounter.incr_by (a, c) i
             |> PairCounter.incr_by (c, b) i;
           chars = IntCounter.incr_by c i state.chars;
         })
       state

let solve steps rules state =
  let { chars = cctr; _ } =
    List.init steps Fun.id
    |> List.fold_left (fun state _ -> grow rules state) state
  in
  let _, min_quant = IntCounter.min cctr in
  let _, max_quant = IntCounter.max cctr in
  max_quant - min_quant

let main =
  let ls = lines Sys.argv.(1) in
  let start = parse_header (List.hd ls) in
  let rules =
    List.to_seq ls |> Seq.drop 2 |> Seq.map parse_rule |> List.of_seq
  in
  solve 10 rules start |> printf "part1: %d\n";
  solve 40 rules start |> printf "part2: %d\n"
