val fold_lines : string -> (string -> 'a) -> 'a list
(** fold_lines file f *)

val parse_int : string -> int * string
(** parse_int s *)

val skip_ws : string -> string
(** skip_ws s *)

val skip_token : string -> string -> string
(** skip_token token s *)
