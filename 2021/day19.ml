open Util
open Format

module Coord = struct
  type t = int * int * int

  let matrices =
    [
      [ [ 1; 0; 0 ]; [ 0; 1; 0 ]; [ 0; 0; 1 ] ];
      [ [ 0; 0; 1 ]; [ 0; 1; 0 ]; [ -1; 0; 0 ] ];
      [ [ -1; 0; 0 ]; [ 0; 1; 0 ]; [ 0; 0; -1 ] ];
      [ [ 0; 0; -1 ]; [ 0; 1; 0 ]; [ 1; 0; 0 ] ];
      [ [ 0; -1; 0 ]; [ 1; 0; 0 ]; [ 0; 0; 1 ] ];
      [ [ 0; 0; 1 ]; [ 1; 0; 0 ]; [ 0; 1; 0 ] ];
      [ [ 0; 1; 0 ]; [ 1; 0; 0 ]; [ 0; 0; -1 ] ];
      [ [ 0; 0; -1 ]; [ 1; 0; 0 ]; [ 0; -1; 0 ] ];
      [ [ 0; 1; 0 ]; [ -1; 0; 0 ]; [ 0; 0; 1 ] ];
      [ [ 0; 0; 1 ]; [ -1; 0; 0 ]; [ 0; -1; 0 ] ];
      [ [ 0; -1; 0 ]; [ -1; 0; 0 ]; [ 0; 0; -1 ] ];
      [ [ 0; 0; -1 ]; [ -1; 0; 0 ]; [ 0; 1; 0 ] ];
      [ [ 1; 0; 0 ]; [ 0; 0; -1 ]; [ 0; 1; 0 ] ];
      [ [ 0; 1; 0 ]; [ 0; 0; -1 ]; [ -1; 0; 0 ] ];
      [ [ -1; 0; 0 ]; [ 0; 0; -1 ]; [ 0; -1; 0 ] ];
      [ [ 0; -1; 0 ]; [ 0; 0; -1 ]; [ 1; 0; 0 ] ];
      [ [ 1; 0; 0 ]; [ 0; -1; 0 ]; [ 0; 0; -1 ] ];
      [ [ 0; 0; -1 ]; [ 0; -1; 0 ]; [ -1; 0; 0 ] ];
      [ [ -1; 0; 0 ]; [ 0; -1; 0 ]; [ 0; 0; 1 ] ];
      [ [ 0; 0; 1 ]; [ 0; -1; 0 ]; [ 1; 0; 0 ] ];
      [ [ 1; 0; 0 ]; [ 0; 0; 1 ]; [ 0; -1; 0 ] ];
      [ [ 0; -1; 0 ]; [ 0; 0; 1 ]; [ -1; 0; 0 ] ];
      [ [ -1; 0; 0 ]; [ 0; 0; 1 ]; [ 0; 1; 0 ] ];
      [ [ 0; 1; 0 ]; [ 0; 0; 1 ]; [ 1; 0; 0 ] ];
    ]

  let compare = compare

  let parse s =
    let x, s = parse_int s in
    let y, s = skip_token "," s |> parse_int in
    let z, _ = skip_token "," s |> parse_int in
    (x, y, z)

  (* Generate all possible rotations of a point around the origin *)
  let rotations (x, y, z) =
    let rec loop mx =
      match mx with
      | [] -> []
      | [ [ a11; a12; a13 ]; [ a21; a22; a23 ]; [ a31; a32; a33 ] ] :: tl ->
          ( (x * a11) + (y * a21) + (z * a31),
            (x * a12) + (y * a22) + (z * a32),
            (x * a13) + (y * a23) + (z * a33) )
          :: loop tl
      | _ -> raise (Failure "rotations")
    in
    loop matrices
end

module CoordSet = Set.Make (Coord)

(* Returns the set of coordinates which extend self based on self's frame of reference *)
let extend_coords_opt self other =
  (* for each coordinate in other, match it to the reference coordinate and then count the number of duplicate coordinates *)
  let offsets =
    Seq.product (CoordSet.to_seq self) (CoordSet.to_seq other)
    |> Seq.map (fun ((ref_x, ref_y, ref_z), (x, y, z)) ->
           (ref_x - x, ref_y - y, ref_z - z))
  in
  let rec loop offsets =
    match offsets with
    | [] -> None
    | (off_x, off_y, off_z) :: tl ->
        let adjusted =
          CoordSet.map
            (fun (x, y, z) -> (x + off_x, y + off_y, z + off_z))
            other
        in
        if CoordSet.inter self adjusted |> CoordSet.cardinal >= 12 then
          Some (CoordSet.union self adjusted, (off_x, off_y, off_z))
        else loop tl
  in
  loop (List.of_seq offsets)

module Scanner = struct
  type t = CoordSet.t list

  let make coords =
    List.map Coord.rotations coords
    |> Matrix.columns |> List.map CoordSet.of_list

  (* extend a set of coordinates by a set of scanner coordinates *)
  let extend self other =
    let rec loop coords =
      match coords with
      | [] -> None
      | cs :: tl -> (
          match extend_coords_opt self cs with
          | Some _ as res -> res
          | None -> loop tl)
    in
    loop other

  let fold scanners =
    let rec try_extend s to_try tried =
      match to_try with
      | [] -> raise (Failure "try_extend")
      | s' :: to_try -> (
          match extend s s' with
          | None -> try_extend s to_try (s' :: tried)
          | Some s -> (s, tried @ to_try))
    in
    let rec loop (acc, scanner_coords) rest =
      match rest with
      | [] -> (acc, scanner_coords)
      | rest ->
          let (acc', coord), rest' = try_extend acc rest [] in
          loop (acc', coord :: scanner_coords) rest'
    in
    loop (List.hd scanners |> List.hd, []) (List.tl scanners)
end

let main =
  let scanners =
    lines Sys.argv.(1)
    |> List.filter (fun s -> not (String.starts_with ~prefix:"---" s))
    |> List.fold_left
         (fun acc line ->
           if line = "" then [] :: acc
           else (Coord.parse line :: List.hd acc) :: List.tl acc)
         [ [] ]
    |> List.map Scanner.make
  in
  Scanner.fold scanners |> fun (beacons, scanner_coords) ->
  printf "beacons: %d\n" (CoordSet.cardinal beacons);
  combinations scanner_coords scanner_coords
  |> List.map (fun ((x, y, z), (x', y', z')) ->
         abs (x - x') + abs (y - y') + abs (z - z'))
  |> List.sort (fun x y -> Int.compare y x)
  |> List.hd |> printf "manhattan: %d\n"
