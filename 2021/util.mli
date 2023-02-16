val flip : ('a -> 'b -> 'c) -> 'b -> 'a -> 'c

val map_lines : string -> (string -> 'a) -> 'a list
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

  val neighbors : int -> int -> 'a t -> 'a list
  val of_list_list : 'a list list -> 'a t
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

module Point : sig
  type t = int * int

  val x : t -> int
  val y : t -> int
  val compare : t -> t -> int
end

module Matrix : sig
  type 'a t = 'a list list

  val dimensions : 'a t -> int * int
  val get : int * int -> 'a t -> 'a
  val coords : 'a t -> (int * int) list
  val neighbor_coords : int * int -> 'a t -> (int * int) list
  val neighbors : int * int -> 'a t -> 'a list
  val map : (int * int -> 'a -> 'b) -> 'a t -> 'b t
  val filteri : (int * int -> 'a -> bool) -> 'a t -> 'a list
  val filter : ('a -> bool) -> 'a t -> 'a list
end
