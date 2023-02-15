open Util
module CharSet = Set.Make (Char)
module CodeMap = Map.Make (CharSet)

let parse s =
  String.split_on_char '|' s
  |> List.map (fun s ->
         String.split_on_char ' ' s
         |> List.filter (fun s -> String.length s != 0)
         |> List.map (fun s -> String.to_seq s |> CharSet.of_seq))

let decoder codes =
  let open CharSet in
  let code_with_len i = List.find (fun s -> i = cardinal s) codes in
  let one = code_with_len 2 in
  let four = code_with_len 4 in
  let seven = code_with_len 3 in
  let eight = code_with_len 7 in
  let zero =
    List.find
      (fun zero ->
        6 = cardinal zero && cardinal (union zero (diff four one)) = 7)
      codes
  in
  let six =
    List.find
      (fun six -> 6 = cardinal six && cardinal (inter six one) = 1)
      codes
  in
  let nine =
    List.find
      (fun nine -> 6 = cardinal nine && nine != zero && nine != six)
      codes
  in
  let three =
    List.find
      (fun three -> 5 = cardinal three && cardinal (inter three one) = 2)
      codes
  in
  let two =
    List.find
      (fun two -> 5 = cardinal two && cardinal (union two four) = 7)
      codes
  in
  let five =
    List.find
      (fun five -> 5 = cardinal five && five != two && five != three)
      codes
  in
  CodeMap.(
    empty |> add zero 0 |> add one 1 |> add two 2 |> add three 3 |> add four 4
    |> add five 5 |> add six 6 |> add seven 7 |> add eight 8 |> add nine 9)

let solve1 lines =
  List.map (fun l -> List.nth l 1) lines
  |> List.flatten
  |> List.filter (fun s ->
         match CharSet.cardinal s with 2 | 3 | 4 | 7 -> true | _ -> false)
  |> List.length

let decode digits decoder =
  List.fold_left
    (fun acc chars -> (acc * 10) + CodeMap.find chars decoder)
    0 digits

let string_of_charset cs = CharSet.to_seq cs |> String.of_seq

let solve2 lines =
  List.map
    (fun l ->
      let dc = decoder (List.nth l 0) in
      decode (List.nth l 1) dc)
    lines
  |> IntList.sum

let main =
  let lines = fold_lines Sys.argv.(1) parse in
  solve1 lines |> Format.printf "part1: %d\n";
  solve2 lines |> Format.printf "part2: %d\n"
