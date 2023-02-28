open Util
open Format

let print_matrix =
  List.iter (fun row ->
      List.iter
        (function 0 -> printf "." | 1 -> printf "#" | n -> printf "%d" n)
        row;
      printf "\n")

let pad n i m =
  let w, _ = Matrix.dimensions m in
  let padding = List.init n (fun _ -> i) in
  let pad_row = List.init ((2 * n) + w) (fun _ -> i) in
  let buffer = List.init n (fun _ -> pad_row) in
  buffer @ List.map (fun row -> padding @ row @ padding) m @ buffer

let parse input =
  let c2i = function
    | '.' -> 0
    | '#' -> 1
    | c -> failwith (sprintf "c2i %c" c)
  in
  let iea = List.hd input |> String.to_seq |> Seq.map c2i |> List.of_seq in
  let image =
    List.to_seq input |> Seq.drop 2
    |> Seq.map (fun s -> String.to_seq s |> Seq.map c2i |> List.of_seq)
    |> List.of_seq
  in
  (iea, image)

let bin_to_dec xs =
  let rec b2d acc = function [] -> acc | i :: tl -> b2d ((acc * 2) + i) tl in
  b2d 0 xs

let enhance n iea image =
  let rec enhance_row row1 row2 row3 =
    match (row1, row2, row3) with
    | ( a11 :: a12 :: a13 :: a1x,
        a21 :: a22 :: a23 :: a2x,
        a31 :: a32 :: a33 :: a3x ) ->
        let index =
          bin_to_dec [ a11; a12; a13; a21; a22; a23; a31; a32; a33 ]
        in
        List.nth iea index
        :: enhance_row (a12 :: a13 :: a1x) (a22 :: a23 :: a2x)
             (a32 :: a33 :: a3x)
    | _ -> []
  in
  let rec enhance_rows image =
    match image with
    | r1 :: r2 :: r3 :: rx ->
        enhance_row r1 r2 r3 :: enhance_rows (r2 :: r3 :: rx)
    | _ -> []
  in
  let rec loop n prev_padding image =
    if n > 0 then
      let padding =
        match prev_padding with
        | 0 -> List.nth iea 0
        | 1 -> List.nth iea 255
        | _ -> failwith "enhance"
      in
      loop (n - 1) padding (pad 2 padding (enhance_rows image))
    else image
  in
  loop n 0 (pad 2 0 image)

let main =
  let iea, image = lines Sys.argv.(1) |> parse in
  enhance 50 iea image
  |> List.fold_left (fun acc row -> IntList.sum row + acc) 0
  |> printf "part1: %d\n"
