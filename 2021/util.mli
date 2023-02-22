val flip : ('a -> 'b -> 'c) -> 'b -> 'a -> 'c
val map_lines : string -> (string -> 'a) -> 'a list
val lines : string -> string list

val parse_int : string -> int * string
(** parse_int s *)

val skip_ws : string -> string
(** skip_ws s *)

val skip_token : string -> string -> string
(** skip_token token s *)

val combinations : 'a list -> 'b list -> ('a * 'b) list

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

module Median : sig
  module type S = sig
    type elt
    type t = Single of elt | Dual of elt * elt

    val of_list : elt list -> t
  end

  module type OrderedType = sig
    type t

    val compare : t -> t -> int
  end

  module Make (O : OrderedType) : S with type elt = O.t
end

module Counter : sig
  module type S = sig
    type key
    type t

    val empty : t
    val incr : key -> t -> t
    val incr_by : key -> int -> t -> t
    val decr : key -> t -> t
    val decr_by : key -> int -> t -> t
    val count : key -> t -> int
    val cardinal : t -> int
    val of_list : key list -> t
    val max : t -> key * int
    val min : t -> key * int
    val bindings : t -> (key * int) list
  end

  module type OrderedType = sig
    type t

    val compare : t -> t -> int
  end

  module Make (Ord : OrderedType) : S with type key = Ord.t
end

module IntList : sig
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
  val all_neighbor_coords : int * int -> 'a t -> (int * int) list
  val neighbors : int * int -> 'a t -> 'a list
  val neighborsi : int * int -> 'a t -> ((int * int) * 'a) list
  val map : (int * int -> 'a -> 'b) -> 'a t -> 'b t
  val filteri : (int * int -> 'a -> bool) -> 'a t -> 'a list
  val filter : ('a -> bool) -> 'a t -> 'a list
end
