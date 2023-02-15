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
