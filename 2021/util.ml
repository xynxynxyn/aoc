let flip f y x = f x y

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

let map_lines file f = List.map f (lines file)
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

let combinations xs ys =
  List.map (fun x -> List.map (fun y -> (x, y)) ys) xs |> List.flatten

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

  let neighbors x y g =
    List.concat
      [
        (if x > 0 then [ g.(y).(x - 1) ] else []);
        (if x < Array.length g.(0) - 1 then [ g.(y).(x + 1) ] else []);
        (if y > 0 then [ g.(y - 1).(x) ] else []);
        (if y < Array.length g - 1 then [ g.(y + 1).(x) ] else []);
      ]

  let of_list_list ll =
    Array.init (List.length ll) (fun i -> List.nth ll i |> Array.of_list)
end

module Median = struct
  module type S = sig
    type elt
    type t = Single of elt | Dual of elt * elt

    val of_list : elt list -> t
  end

  module type OrderedType = sig
    type t

    val compare : t -> t -> int
  end

  module Make (Ord : OrderedType) = struct
    type elt = Ord.t
    type t = Single of elt | Dual of elt * elt

    let of_list l =
      let len = List.length l in
      let l = List.sort Ord.compare l in
      if len mod 2 = 0 then
        Dual (List.nth l ((len / 2) - 1), List.nth l (len / 2))
      else Single (List.nth l (len / 2))
  end
end

module Counter = struct
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

  module Make (Ord : OrderedType) = struct
    module KeyMap = Map.Make (Ord)

    type key = Ord.t
    type t = int KeyMap.t

    let empty = KeyMap.empty

    let incr_by key i =
      KeyMap.update key (function Some c -> Some (c + i) | None -> Some i)

    let incr key = incr_by key 1
    let decr_by key i = incr_by key (-i)
    let decr key = decr_by key 1
    let count key map = KeyMap.find_opt key map |> Option.value ~default:0
    let cardinal = KeyMap.cardinal
    let of_list = List.fold_left (fun map key -> incr key map) empty

    let max ctr =
      KeyMap.bindings ctr
      |> List.sort (fun (_, x) (_, x') -> Int.compare x x')
      |> List.rev |> Fun.flip List.nth 0

    let min ctr =
      KeyMap.bindings ctr
      |> List.sort (fun (_, x) (_, x') -> Int.compare x x')
      |> Fun.flip List.nth 0

    let bindings = KeyMap.bindings
  end
end

module IntList = struct
  let sum = List.fold_left (fun acc x -> acc + x) 0

  let range start stop =
    Seq.init (stop - start + 1) (fun i -> i + start) |> List.of_seq

  let of_string s = String.split_on_char ',' s |> List.map int_of_string
end

module Point = struct
  type t = int * int

  let x (x, _) = x
  let y (_, y) = y

  let compare p0 p1 =
    let x_comp = compare (x p0) (x p1) in
    let y_comp = compare (y p0) (y p1) in
    if y_comp = 0 then x_comp else y_comp
end

module Matrix = struct
  type 'a t = 'a list list

  let dimensions m = (List.length (List.nth m 0), List.length m)
  let get (x, y) m = List.nth (List.nth m y) x

  let coords m =
    let width, height = dimensions m in
    IntList.range 0 (width - 1)
    |> List.map (fun x ->
           IntList.range 0 (height - 1) |> List.map (fun y -> (x, y)))
    |> List.flatten

  let neighbor_coords (x, y) m =
    let height = List.length m in
    let width = List.length (List.nth m 0) in
    List.filter
      (fun (x, y) -> 0 <= x && x < width && 0 <= y && y < height)
      [ (x - 1, y); (x + 1, y); (x, y - 1); (x, y + 1) ]

  let all_neighbor_coords (x, y) m =
    let height = List.length m in
    let width = List.length (List.nth m 0) in
    List.filter
      (fun (x, y) -> 0 <= x && x < width && 0 <= y && y < height)
      [
        (x - 1, y);
        (x - 1, y - 1);
        (x - 1, y + 1);
        (x + 1, y);
        (x + 1, y - 1);
        (x + 1, y + 1);
        (x, y - 1);
        (x, y + 1);
      ]

  let neighborsi p m = neighbor_coords p m |> List.map (fun p -> (p, get p m))
  let neighbors p m = neighbor_coords p m |> List.map (Fun.flip get m)

  let map f m =
    let rec map_row y f m =
      match m with
      | [] -> []
      | row :: tl ->
          List.mapi (fun x e -> f (x, y) e) row :: map_row (y + 1) f tl
    in
    map_row 0 f m

  let filteri f m =
    let rec filter_row y f m =
      match m with
      | [] -> []
      | row :: tl ->
          List.filteri (fun x e -> f (x, y) e) row :: filter_row (y + 1) f tl
    in
    filter_row 0 f m |> List.flatten

  let filter f m = filteri (fun _ e -> f e) m
end
