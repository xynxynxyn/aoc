open Util

let parse s = String.to_seq s |> List.of_seq

let rec count acc l =
  match (l, acc) with
  | '0' :: tl, (zeros, ones) :: tacc -> (zeros + 1, ones) :: count tacc tl
  | '1' :: tl, (zeros, ones) :: tacc -> (zeros, ones + 1) :: count tacc tl
  | _, _ -> []

let bin_to_dec l =
  let rec loop l acc =
    match l with
    | [] -> acc / 2
    | '0' :: tl -> loop tl (acc * 2)
    | '1' :: tl -> loop tl ((acc + 1) * 2)
    | _ -> raise (Failure "bin_to_dec")
  in
  loop l 0

let count l =
  match l with
  | h :: tl ->
      List.fold_left count
        (List.map
           (function
             | '0' -> (1, 0) | '1' -> (0, 1) | _ -> raise (Failure "fold"))
           h)
        tl
  | _ -> raise (Failure "most_common")

let part_one l =
  let reduced = count l in
  ( List.map (fun (x, y) -> if x > y then '0' else '1') reduced |> bin_to_dec,
    List.map (fun (x, y) -> if x <= y then '0' else '1') reduced |> bin_to_dec
  )

let rec find l reduce =
  let rec loop l red offset =
    let crit = reduce l in
    match (l, crit) with
    | [ h ], _ -> h
    | _, [] | [], _ -> raise (Failure "find")
    | l, crit ->
        if offset >= List.length (List.hd l) then raise (Failure "find offset")
        else
          loop
            (List.filter
               (function
                 | ll when List.nth ll offset == List.nth crit offset -> true
                 | _ -> false)
               l)
            reduce (offset + 1)
  in
  loop l reduce 0

let oxygen_reduce l =
  List.map (fun (x, y) -> if x > y then '0' else '1') (count l)

let co2_reduce l =
  List.map (fun (x, y) -> if x <= y then '0' else '1') (count l)

let part_two l =
  (find l oxygen_reduce |> bin_to_dec, find l co2_reduce |> bin_to_dec)

let main =
  let lines = fold_lines Sys.argv.(1) parse in
  let p1_gamma, p1_epsilon = part_one lines in
  Format.printf "part_one: %d, %d -> %d\n" p1_gamma p1_epsilon
    (p1_gamma * p1_epsilon);
  let oxy, co2 = part_two lines in
  Format.printf "part+two: %d, %d -> %d\n" oxy co2 (oxy * co2)
