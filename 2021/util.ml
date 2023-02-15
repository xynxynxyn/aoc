(** fold_lines file f *)
let fold_lines file f =
  let in_ch = open_in file in
  let rec loop () =
    try
      let next = input_line in_ch in
      f next :: loop ()
    with End_of_file ->
      close_in in_ch;
      []
  in
  loop ()

let is_digit = function '0' .. '9' -> true | _ -> false
let is_ws = function ' ' | '\t' | '\r' | '\n' -> true | _ -> false

let parse_int s =
  let s = String.to_seq s in
  let i = Seq.take_while is_digit s |> String.of_seq |> int_of_string in
  let rest = Seq.drop_while is_digit s |> String.of_seq in
  (i, rest)

let skip_ws s = String.to_seq s |> Seq.drop_while is_ws |> String.of_seq

(** skip_token token s *)
let skip_token token s = String.(sub s (length token) (length s - length token))

let rec parse_list_of_ints s =
  match parse_int s with
  | i, rest when rest = "" -> [ i ]
  | i, rest -> i :: parse_list_of_ints (skip_ws rest |> skip_token ",")

let range s e = Seq.init (e - s + 1) (fun i -> i + s)
let sum = List.fold_left (fun acc x -> acc + x) 0

module Grid = struct
  type 'a t = 'a Array.t Array.t

  let get x y g = g.(y).(x)
  let set x y e g = g.(y).(x) <- e

  let mut x y f g =
    let e = get x y g in
    set x y (f e) g

  let make width height default = Array.make_matrix width height default

  let iter f g =
    Array.iteri (fun y row -> Array.iteri (fun x e -> f x y e) row) g
end
