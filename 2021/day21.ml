open Util
open Format

type player = { pos : int; score : int }
type game = { p1 : player; p2 : player; die : int; turn : int }

let print_game game =
  printf "p1 with score %d at %d, p2 with score %d at %d\n" game.p1.score
    (game.p1.pos + 1) game.p2.score (game.p2.pos + 1)

let move steps player =
  let pos = (player.pos + steps) mod 10 in
  { pos; score = player.score + pos + 1 }

module Normal = struct
  let roll die =
    (* rolls the die three times and returns the die again with the resulting value *)
    match die with
    | 100 -> (103, 3)
    | 99 -> (200, 2)
    | 98 -> (297, 1)
    | n -> ((3 * n) + 3, n + 3)

  let turn game =
    let steps, die = roll game.die in
    if game.turn mod 2 = 0 then
      { game with p1 = move steps game.p1; turn = game.turn + 1; die }
    else { game with p2 = move steps game.p2; turn = game.turn + 1; die }

  let rec play game =
    if game.p1.score >= 1000 then game.p2.score * game.turn * 3
    else if game.p2.score >= 1000 then game.p1.score * game.turn * 3
    else turn game |> play
end

module Quantum = struct
  let die_rolls = [ (3, 1); (4, 3); (5, 6); (6, 7); (7, 6); (8, 3); (9, 1) ]

  (* returns the list of game results and how often the occur *)
  let turn game =
    let p1_turn = game.turn mod 2 = 0 in
    List.map
      (fun (steps, factor) ->
        if p1_turn then
          ({ game with p1 = move steps game.p1; turn = game.turn + 1 }, factor)
        else
          ({ game with p2 = move steps game.p2; turn = game.turn + 1 }, factor))
      die_rolls

  module State = struct
    type t = player * player * int

    let compare = compare
  end

  module StateMap = Map.Make (State)

  let play game =
    let m = ref StateMap.empty in
    let rec play' game =
      try StateMap.find (game.p1, game.p2, game.turn mod 2) !m
      with Not_found ->
        let score =
          if game.p1.score >= 21 then (1, 0)
          else if game.p2.score >= 21 then (0, 1)
          else
            turn game
            |> List.map (fun (game, factor) ->
                   let x, y = play' game in
                   (x * factor, y * factor))
            |> List.fold_left (fun (x, y) (x', y') -> (x + x', y + y')) (0, 0)
        in
        m := StateMap.add (game.p1, game.p2, game.turn mod 2) score !m;
        score
    in
    play' game
end

let main =
  let p1_pos, p2_pos =
    lines Sys.argv.(1) |> List.map (fun s -> String.sub s 28 1 |> int_of_string)
    |> function
    | p1 :: p2 :: _ -> (p1, p2)
    | _ -> failwith "parse"
  in
  let game =
    {
      p1 = { pos = p1_pos - 1; score = 0 };
      p2 = { pos = p2_pos - 1; score = 0 };
      die = 1;
      turn = 0;
    }
  in
  Normal.play game |> printf "part1: %d\n";
  Quantum.play game |> fun (p1, p2) -> printf "part2: %d\n" (Int.max p1 p2)
