val flip : ('a -> 'b -> 'c) -> 'b -> 'a -> 'c

val fold_lines : string -> (string -> 'a) -> 'a list
(** fold_lines file f *)

val parse_int : string -> int * string
(** parse_int s *)

val skip_ws : string -> string
(** skip_ws s *)

val skip_token : string -> string -> string
(** skip_token token s *)

module Grid : sig
  type 'a t

  val get : int -> int -> 'a t -> 'a
  (** get x y g *)

  val set : int -> int -> 'a -> 'a t -> unit
  (** set x y elem g *)

  val mut : int -> int -> ('a -> 'a) -> 'a t -> unit
  (** mut x y f g 
   mutate element at x y with function f *)

  val make : int -> int -> 'a -> 'a t
  (** make width height default *)

  val iter : (int -> int -> 'a -> unit) -> 'a t -> unit
  (** iter (f x y elem) g 
    run the provided function on each cell in the grid *)
end

module IntList : sig
  module Median : sig
    type t = Single of int | Dual of int * int

    val median : int list -> t
  end

  val sum : int list -> int
  val of_string : string -> int list
  val range : int -> int -> int list
end
