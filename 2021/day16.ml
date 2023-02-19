open Util
open Format

let bin_to_dec xs =
  let rec b2d acc = function [] -> acc | i :: tl -> b2d ((acc * 2) + i) tl in
  b2d 0 xs

let hex_to_bin = function
  | '0' -> [ 0; 0; 0; 0 ]
  | '1' -> [ 0; 0; 0; 1 ]
  | '2' -> [ 0; 0; 1; 0 ]
  | '3' -> [ 0; 0; 1; 1 ]
  | '4' -> [ 0; 1; 0; 0 ]
  | '5' -> [ 0; 1; 0; 1 ]
  | '6' -> [ 0; 1; 1; 0 ]
  | '7' -> [ 0; 1; 1; 1 ]
  | '8' -> [ 1; 0; 0; 0 ]
  | '9' -> [ 1; 0; 0; 1 ]
  | 'A' -> [ 1; 0; 1; 0 ]
  | 'B' -> [ 1; 0; 1; 1 ]
  | 'C' -> [ 1; 1; 0; 0 ]
  | 'D' -> [ 1; 1; 0; 1 ]
  | 'E' -> [ 1; 1; 1; 0 ]
  | 'F' -> [ 1; 1; 1; 1 ]
  | _ -> raise (Failure "hex_to_bin")

type packet_info = { packet_version : int; packet_type : int }
type operator = Sum | Product | Minimum | Maximum | Greater | Less | Equal

let operator_of_int = function
  | 0 -> Sum
  | 1 -> Product
  | 2 -> Minimum
  | 3 -> Maximum
  | 5 -> Greater
  | 6 -> Less
  | 7 -> Equal
  | _ -> raise (Failure "operator_of_int")

type packet =
  | Literal of { info : packet_info; value : int }
  | Operator of {
      info : packet_info;
      packets : packet list;
      operator : operator;
    }

let rec parse_literal bin =
  match bin with
  | 0 :: a :: b :: c :: d :: tl -> ([ a; b; c; d ], tl)
  | 1 :: a :: b :: c :: d :: tl ->
      let data, tl = parse_literal tl in
      (a :: b :: c :: d :: data, tl)
  | _ -> raise (Failure "parse_literal")

let rec parse_packet bin =
  match bin with
  | v0 :: v1 :: v2 :: t0 :: t1 :: t2 :: rest -> (
      let info =
        {
          packet_version = bin_to_dec [ v0; v1; v2 ];
          packet_type = bin_to_dec [ t0; t1; t2 ];
        }
      in
      if info.packet_type = 4 then
        let value, rest = parse_literal rest in
        (Literal { info; value = bin_to_dec value }, rest)
      else
        match rest with
        | 0 :: rest ->
            let data_length =
              List.to_seq rest |> Seq.take 15 |> List.of_seq |> bin_to_dec
            in
            let packets =
              parse_packet_list
                (List.to_seq rest |> Seq.drop 15 |> Seq.take data_length
               |> List.of_seq)
            in
            ( Operator
                { info; packets; operator = operator_of_int info.packet_type },
              List.to_seq rest |> Seq.drop (15 + data_length) |> List.of_seq )
        | 1 :: rest ->
            let n =
              List.to_seq rest |> Seq.take 11 |> List.of_seq |> bin_to_dec
            in
            let packets, rest =
              parse_n_packets n (List.to_seq rest |> Seq.drop 11 |> List.of_seq)
            in
            ( Operator
                { info; packets; operator = operator_of_int info.packet_type },
              rest )
        | _ -> raise (Failure "parse_operator"))
  | _ -> raise (Failure "parse_packet")

and parse_packet_list bin =
  match parse_packet bin with
  | packet, [] -> [ packet ]
  | packet, rest -> packet :: parse_packet_list rest

and parse_n_packets n bin =
  if n > 0 then
    let p, bin = parse_packet bin in
    let remaining, bin = parse_n_packets (n - 1) bin in
    (p :: remaining, bin)
  else ([], bin)

let rec collect_versions packet =
  match packet with
  | Literal { info = { packet_version; _ }; _ } -> packet_version
  | Operator { info = { packet_version; _ }; packets } ->
      packet_version
      + List.fold_left ( + ) 0 (List.map collect_versions packets)

let rec eval = function
  | Literal { value; _ } -> value
  | Operator { operator; packets; _ } -> (
      let values = List.map eval packets in
      match operator with
      | Sum -> IntList.sum values
      | Product -> List.fold_left ( * ) 1 values
      | Minimum -> List.sort Int.compare values |> List.hd
      | Maximum ->
          List.sort (fun x x' -> ~-(Int.compare x x')) values |> List.hd
      | Greater -> if List.nth values 0 > List.nth values 1 then 1 else 0
      | Less -> if List.nth values 0 < List.nth values 1 then 1 else 0
      | Equal -> if List.nth values 0 = List.nth values 1 then 1 else 0)

let main =
  let packets =
    map_lines Sys.argv.(1) (fun s ->
        String.to_seq s |> Seq.map hex_to_bin |> List.of_seq |> List.concat)
    |> List.map parse_packet
    |> List.map (fun (p, _) -> p)
  in
  List.map collect_versions packets |> List.iter (printf "part1: %d\n");
  List.map eval packets |> List.iter (printf "part2: %d\n")
