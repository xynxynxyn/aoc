let fits l xs =
  List.length xs >= l
  && List.to_seq xs |> Seq.take l |> Seq.for_all (fun n -> n > 0)
  && List.nth_opt xs l |> Option.value ~default:0 < 2

let place n xs = List.to_seq xs |> Seq.drop (n + 1) |> List.of_seq

let solve =
  let h = Hashtbl.create 1000 in
  let rec solve' xs set =
    try Hashtbl.find h (xs, set)
    with Not_found -> (
      match (xs, set) with
      | _, [] -> if List.find_opt (( == ) 2) xs |> Option.is_none then 1 else 0
      | [], _ -> 0
      (* do not skip forced values *)
      | 2 :: tl, next :: rest ->
          if fits next xs then solve' (place next xs) rest else 0
      | _ :: tl, next :: rest ->
          let res =
            (if fits next xs then solve' (place next xs) rest else 0)
            +
            if List.fold_left ( + ) 0 set <= List.length xs then solve' tl set
            else 0
          in
          Hashtbl.add h (xs, set) res;
          res)
  in
  solve'

let parse str =
  match String.split_on_char ' ' str with
  | springs :: nums :: _ ->
      ( String.to_seq springs
        |> Seq.map (function
             | '.' -> 0
             | '?' -> 1
             | '#' -> 2
             | _ -> raise (Failure "parse"))
        |> List.of_seq,
        String.split_on_char ',' nums |> List.map int_of_string )
  | _ -> raise (Failure "parse")

let expand (xs, set) =
  ( xs @ [ 1 ] @ xs @ [ 1 ] @ xs @ [ 1 ] @ xs @ [ 1 ] @ xs,
    set @ set @ set @ set @ set )

let lines file =
  let in_ch = open_in file in
  let rec loop () =
    try
      let next = input_line in_ch in
      next :: loop ()
    with End_of_file ->
      close_in in_ch;
      []
  in
  loop ()

let main =
  let data = lines "input.txt" |> List.map (fun s -> parse s) in
  List.map (fun (x, y) -> solve x y) data
  |> List.fold_left ( + ) 0 |> Format.printf "I:  %d\n";
  List.map expand data
  |> List.mapi (fun i (x, y) -> solve x y)
  |> List.fold_left ( + ) 0 |> Format.printf "II: %d\n"
